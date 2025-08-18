package main

import (
	"encoding/csv"
	"encoding/json"
	"fmt"
	"log"
	"os"
	"runtime"
	"sync"
	"sync/atomic"
	"time"

	polycall "github.com/metacall/core/source/ports/go_port/source"
)

var (
	batchesIssued   int32 = 0
	batchesFinished int32 = 0
)

// Time + memory metrics for each event
type OpMetrics struct {
	Timestamp   time.Time `json:"timestamp"`
	Step        string    `json:"step"`
	ElapsedMS   int64     `json:"elapsed_ms"`
	MemMB       uint64    `json:"memory_mb"`
	BatchSize   int       `json:"batch_size,omitempty"`
	WorkerID    int       `json:"worker_id,omitempty"`
}

// Summary info for a single batch
type BatchStats struct {
	BatchID       int32  `json:"batch_id"`
	ItemsHandled  int    `json:"items_handled"`
	DurationMS    int64  `json:"duration_ms"`
	MemUsage      uint64 `json:"memory_mb"`
	WorkerID      int    `json:"worker_id"`
}

// A processed record from Python
type Record struct {
	Index         int     `json:"index"`
	Text          string  `json:"text"`
	Label         string  `json:"label"`
	Score         float64 `json:"score"`
	BatchID       int32   `json:"batch_id"`
	WorkerID      int     `json:"worker_id"`
	DurationMS    int64   `json:"duration_ms"`
}

// BatchManager distributes work across workers
type BatchManager struct {
	batchSize   int
	workerCount int
	inChan      chan []string
	outChan     chan []Record
	errChan     chan error
	opMetrics   []OpMetrics
	batchStats  []BatchStats
	wg          sync.WaitGroup
	totalItems  int
	doneCount   int32
	startTime   time.Time
	mtx         sync.Mutex
}

func memUsageMB() uint64 {
	var m runtime.MemStats
	runtime.ReadMemStats(&m)
	return m.Alloc / 1024 / 1024
}

func NewBatchManager(batchSize, workers, total int) *BatchManager {
	return &BatchManager{
		batchSize:   batchSize,
		workerCount: workers,
		inChan:      make(chan []string, workers*2),
		outChan:     make(chan []Record, workers*2),
		errChan:     make(chan error, workers),
		opMetrics:   make([]OpMetrics, 0, total/batchSize+1),
		batchStats:  make([]BatchStats, 0, total/batchSize+1),
		totalItems:  total,
		startTime:   time.Now(),
	}
}

func (bm *BatchManager) logStep(step string, start time.Time, extra map[string]interface{}) {
	duration := time.Since(start).Milliseconds()
	memory := memUsageMB()
	entry := OpMetrics{
		Timestamp: time.Now(),
		Step:      step,
		ElapsedMS: duration,
		MemMB:     memory,
	}
	if bs, ok := extra["batch_size"]; ok {
		entry.BatchSize = bs.(int)
	}
	if wid, ok := extra["worker_id"]; ok {
		entry.WorkerID = wid.(int)
	}
	bm.mtx.Lock()
	bm.opMetrics = append(bm.opMetrics, entry)
	bm.mtx.Unlock()
	fmt.Printf("[Worker %d] %s took %dms (RAM %dMB)\n", entry.WorkerID, step, duration, memory)
}

func (bm *BatchManager) workerLoop(id int) {
	defer bm.wg.Done()
	start := time.Now()
	for batch := range bm.inChan {
		if err := bm.handleBatch(batch, id); err != nil {
			bm.errChan <- fmt.Errorf("worker %d error: %v", id, err)
		}
	}
	bm.logStep("worker_done", start, map[string]interface{}{"worker_id": id})
}

func (bm *BatchManager) handleBatch(batch []string, workerID int) error {
	begin := time.Now()
	batchID := atomic.AddInt32(&batchesIssued, 1)

	fmt.Printf("Worker %d -> Batch %d (size %d)\n", workerID, batchID, len(batch))

	// Python call
	result, err := polycall.Call("process_batch", batch)
	if err != nil {
		return fmt.Errorf("pycall failed: %v", err)
	}

	results, ok := result.([]interface{})
	if !ok {
		return fmt.Errorf("invalid Python return type: %T", result)
	}

	elapsed := time.Since(begin).Milliseconds()
	records := make([]Record, 0, len(results))

	for i, r := range results {
		if asMap, ok := r.(map[string]interface{}); ok {
			rec := Record{
				Index:      (int(batchID)-1)*bm.batchSize + i,
				Text:       batch[i],
				Label:      asMap["label"].(string),
				Score:      asMap["score"].(float64),
				BatchID:    batchID,
				WorkerID:   workerID,
				DurationMS: elapsed,
			}
			records = append(records, rec)
		}
	}

	bm.outChan <- records
	atomic.AddInt32(&bm.doneCount, int32(len(records)))

	memNow := memUsageMB()
	bm.mtx.Lock()
	bm.batchStats = append(bm.batchStats, BatchStats{
		BatchID:      batchID,
		ItemsHandled: len(batch),
		DurationMS:   elapsed,
		MemUsage:     memNow,
		WorkerID:     workerID,
	})
	bm.mtx.Unlock()

	fmt.Printf("Batch %d done by worker %d in %dms (RAM %dMB)\n", batchID, workerID, elapsed, memNow)
	atomic.AddInt32(&batchesFinished, 1)
	return nil
}

func writeResults(records []Record, file string) error {
	output := struct {
		Meta struct {
			Total       int   `json:"total"`
			NegCount    int   `json:"neg_count"`
			PosCount    int   `json:"pos_count"`
			ProcMS      int64 `json:"proc_time_ms"`
			AvgBatchMS  int64 `json:"avg_batch_time_ms"`
			BatchTotal  int32 `json:"batch_total"`
		} `json:"meta"`
		Data []Record `json:"data"`
	}{Data: records}

	for _, r := range records {
		if r.Label == "NEGATIVE" {
			output.Meta.NegCount++
		} else {
			output.Meta.PosCount++
		}
	}
	output.Meta.Total = len(records)
	output.Meta.BatchTotal = atomic.LoadInt32(&batchesFinished)

	jsonBytes, err := json.MarshalIndent(output, "", "  ")
	if err != nil {
		return err
	}
	return os.WriteFile(file, jsonBytes, 0644)
}

func readCSV(file string, targetCol string) ([]string, error) {
	f, err := os.Open(file)
	if err != nil {
		return nil, fmt.Errorf("cannot open file: %v", err)
	}
	defer f.Close()

	reader := csv.NewReader(f)
	headers, err := reader.Read()
	if err != nil {
		return nil, fmt.Errorf("cannot read headers: %v", err)
	}

	colIdx := -1
	for i, h := range headers {
		if h == targetCol {
			colIdx = i
			break
		}
	}
	if colIdx == -1 {
		return nil, fmt.Errorf("CSV missing '%s' column", targetCol)
	}

	rows, err := reader.ReadAll()
	if err != nil {
		return nil, fmt.Errorf("cannot read rows: %v", err)
	}

	out := make([]string, 0, 1000)
	for i, row := range rows {
		if i >= 1000 {
			break
		}
		if colIdx < len(row) && row[colIdx] != "" {
			out = append(out, row[colIdx])
		}
	}
	return out, nil
}

func main() {
	runtime.GOMAXPROCS(runtime.NumCPU())
	start := time.Now()

	if err := polycall.Initialize(); err != nil {
		log.Fatalf("Polycall init failed: %v", err)
	}
	fmt.Printf("Polycall initialized in %dms\n", time.Since(start).Milliseconds())

	if err := polycall.LoadFromFile("py", []string{"ml_processor.py"}); err != nil {
		log.Fatalf("Python script load failed: %v", err)
	}

	descs, err := readCSV("7817_1.csv", "categories")


	if err != nil {
		log.Fatalf("CSV error: %v", err)
	}
	fmt.Printf("Loaded %d descriptions\n", len(descs))

	batchSize := 32
	workers := runtime.NumCPU()
	manager := NewBatchManager(batchSize, workers, len(descs))

	// build batches
	var allBatches [][]string
	temp := make([]string, 0, batchSize)
	for _, d := range descs {
		temp = append(temp, d)
		if len(temp) == batchSize {
			allBatches = append(allBatches, temp)
			temp = make([]string, 0, batchSize)
		}
	}
	if len(temp) > 0 {
		allBatches = append(allBatches, temp)
	}

	// collect results
	var allRecords []Record
	done := make(chan bool)
	go func() {
		for recs := range manager.outChan {
			allRecords = append(allRecords, recs...)
			fmt.Printf("Progress: %d/%d done\n", len(allRecords), len(descs))
		}
		done <- true
	}()

	// spawn workers
	manager.wg.Add(workers)
	for i := 0; i < workers; i++ {
		go manager.workerLoop(i)
	}

	for _, b := range allBatches {
		manager.inChan <- b
	}
	close(manager.inChan)

	manager.wg.Wait()
	close(manager.outChan)
	<-done

	totalMS := time.Since(start).Milliseconds()
	neg, pos := 0, 0
	for _, r := range allRecords {
		if r.Label == "NEGATIVE" {
			neg++
		} else {
			pos++
		}
	}

	_ = os.MkdirAll("results", 0755)
	stamp := time.Now().Format("20060102_150405")

	_ = writeResults(allRecords, fmt.Sprintf("results/output_%s.json", stamp))
	metJSON, _ := json.MarshalIndent(manager.opMetrics, "", "  ")
	_ = os.WriteFile(fmt.Sprintf("results/metrics_%s.json", stamp), metJSON, 0644)
	batchJSON, _ := json.MarshalIndent(manager.batchStats, "", "  ")
	_ = os.WriteFile(fmt.Sprintf("results/batches_%s.json", stamp), batchJSON, 0644)

	fmt.Println("\n--- Run Summary ---")
	fmt.Printf("Total items: %d\n", len(allRecords))
	fmt.Printf("NEG: %d | POS: %d\n", neg, pos)
	fmt.Printf("Total time: %dms\n", totalMS)
	fmt.Printf("Workers: %d | Batch size: %d | Batches: %d\n",
		workers, batchSize, len(allBatches))
	fmt.Printf("Final RAM: %dMB\n", memUsageMB())
}

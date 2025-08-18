# Polycall ML Processing: A Cross-Language Performance Analysis

## Introduction

In modern software development, efficiently combining multiple programming languages can be challenging. This experiment demonstrates how Polycall enables seamless integration between Go and Python
for ML processing, providing significant performance benefits.

```mermaid
graph TD
    A[Challenge] --> B[Multiple Language Integration]
    A --> C[ML Processing Efficiency]
    A --> D[Resource Management]

    B --> E[Polycall Solution]
    C --> E
    D --> E

    E --> F[Go Performance]
    E --> G[Python ML Capabilities]
```

### Project Overview

We process 1000 wine reviews using sentiment analysis, comparing three approaches:

1. Pure Python (Single-threaded)
2. Python with Multiprocessing
3. Go + Polycall Integration

## Installation and Setup

### Prerequisites

```bash
# System Requirements
- Go 1.22 or later
- Python 3.8 or later
- GNU/Linux system (tested on Fedora/Ubuntu)
```

### Installing Polycall

1. Clone and Build Polycall Core:

```bash
# Clone Polycall repository
git clone https://github.com/Polycall/core.git
cd core

# Create build directory
mkdir build && cd build

# Configure with required loaders
cmake \
    -DOPTION_BUILD_LOADERS=ON \
    -DOPTION_BUILD_LOADERS_PY=ON \
    -DOPTION_BUILD_PORTS=ON \
    -DOPTION_BUILD_PORTS_GO=ON \
    -DNODEJS_CMAKE_DEBUG=ON \
    ..

# Build and install
sudo HOME="$HOME" cmake --build . --target install

# Update library cache
sudo ldconfig
```

2. Set Environment Variables:

```bash
# Add to ~/.bashrc or ~/.zshrc
export LOADER_LIBRARY_PATH="/usr/local/lib"
export LOADER_SCRIPT_PATH="`pwd`"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
```

### Project Dependencies

1. Python Dependencies:

```bash
pip install transformers torch pandas tqdm
```

2. Go Dependencies:

```bash
go mod init text-classification
go get github.com/Polycall/core/source/ports/go_port/source
```

### Project Structure

```
project/
├── wine-reviews.csv
├── ml_processor.py
├── textClassifier.py
├── multithreadedTextClassifier.py
├── main.go
└── go.mod
```

### Verifying Installation

Test your setup with:

```bash
# Check Polycall installation
Polycall --version

# Verify Python environment
python -c "from transformers import pipeline; print('Transformers OK')"

# Test Go environment
go version
```

### Dataset Preparation

The wine-reviews.csv should contain:

-  A 'description' column with wine reviews
-  Text data suitable for sentiment analysis

## Initial Configuration

1. Create ml_processor.py:

```python
from transformers import pipeline

print("Initializing model...")
classifier = pipeline('text-classification',
    model="distilbert-base-uncased-finetuned-sst-2-english",
    device="cpu")
print("Model loaded!")

def process_batch(texts: list[str]) -> list[dict]:
    try:
        results = classifier(texts)
        return results
    except Exception as e:
        print(f"Error processing batch: {str(e)}")
        return []
```

2. Verify Environment:

```bash
# Run a simple test
export LOADER_SCRIPT_PATH="`pwd`"
Polycall ml_processor.py
```

# Technical Background

## Understanding Our Processing Pipeline

```mermaid
graph TD
    subgraph "Input Processing"
        A[Wine Reviews Dataset] --> B[Data Loading]
    end

    subgraph "Processing Approaches"
        subgraph "Pure Python"
            F[Single Thread] --> G1[Sequential Batches]
            G1 --> G2[Single Model Instance]
            G2 --> G3[Sequential Results]
        end

        subgraph "Python Multiprocessing"
            H[8 Processes] --> I1[Parallel Batches]
            I1 --> I2[8 Model Instances]
            I2 --> I3[Process Pool Results]
        end

        subgraph "Go + Polycall"
            J[Go Routines] --> K1[Concurrent Batches]
            K1 --> K2[Single Shared Model]
            K2 --> K3[Channel-based Results]
        end
    end

    subgraph "Resource Impact"
        G2 --> R1[Memory: 500MB]
        I2 --> R2[Memory: 4GB]
        K2 --> R3[Memory: 94MB]
    end

    B --> F
    B --> H
    B --> J
```

## Python Implementation Analysis

### Single-Threaded Version

```python
# Processing Flow: Sequential
results = classifyTextsBatches(descriptions, batchSize=8)
# Performance: ~38 seconds for 1000 records
# Memory: ~500MB (Python runtime + Model)
```

Key Points:

1. PyTorch/TensorFlow Internal Optimization

   -  Already uses multiple cores for computation
   -  Optimized for tensor operations
   -  Built-in parallelization

2. Memory Pattern:

```mermaid
graph LR
    A[Python Runtime ~50MB] --> B[Model in Memory ~400MB]
    B --> C[Data Buffers ~50MB]
```

### Python Multiprocessing Version

```mermaid
graph TD
    A[Main Process] --> B[Process 1]
    A --> C[Process 2]
    A --> D[Process 3]
    A --> E[Process 4]

    B --> F[Model Copy 1]
    C --> G[Model Copy 2]
    D --> H[Model Copy 3]
    E --> I[Model Copy 4]
```

Memory Impact:

-  Each process: ~500MB
-  Total for 8 cores: ~4GB but due to Pythons shared memory optimization could be around 2GB
-  Performance degradation due to:
   -  Resource contention
   -  Model replication
   -  IPC overhead

## Go + Polycall Implementation

```mermaid
graph TD
    A[Go Main Process] --> B[Single Python Runtime]
    A --> C[Goroutine 1]
    A --> D[Goroutine 2]
    A --> E[Goroutine n]

    C --> B
    D --> B
    E --> B
```

### Key Components:

1. Batch Processor:

```go
type BatchProcessor struct {
    batchSize   int          // Optimized to 32
    numWorkers  int          // Set to CPU count
    inputChan   chan []string
    resultChan  chan []DetailedResult
    // ... other fields
}
```

2. Worker Pool:

```mermaid
graph LR
    A[Input Channel] --> B[Worker 1]
    A --> C[Worker 2]
    A --> D[Worker 3]

    B --> E[Result Channel]
    C --> E
    D --> E
```

3. Memory Management:

-  Single Python runtime (~500MB)
-  Minimal Go overhead (~2KB per goroutine)
-  Shared model instance
-  Efficient channel communication

Performance Results:

```
Go + Polycall:
- Processing Time: ~40s
- Memory Usage: ~94MB
- Worker Count: 8
- Batch Size: 32
```

# Performance Analysis

## Implementation Comparisons

```mermaid
graph TB
    subgraph "Processing Times"
        A[Python Single Thread] --> B[38 seconds]
        C[Python Multiprocessing] --> D[220+ seconds]
        E[Go + Polycall] --> F[42 seconds]
    end

    subgraph "Memory Usage"
        G[Python Single] --> H[~500MB]
        I[Python Multi] --> J[~4GB]
        K[Go + Polycall] --> L[~110MB]
    end
```

## Detailed Metrics

### 1. Processing Speed

```markdown
| Implementation | Time (s) | Items/second | Batch Size |
| -------------- | -------- | ------------ | ---------- |
| Python Single  | 38       | 26.3         | 8          |
| Python Multi   | 220+     | 4.5          | 8          |
| Go + Polycall  | 42       | 23.8         | 30         |
```

### 2. Memory Efficiency

```mermaid
graph LR
    A[Memory Usage] --> B[Python Single: 500MB]
    A --> C[Python Multi: 4GB]
    A --> D[Go + Polycall: 110MB]

    B --> E[Model + Runtime]
    C --> F[8x Model + 8x Runtime]
    D --> G[Shared Model + Minimal Overhead]
```

### 3. Batch Processing Analysis

Go + Polycall Batch Metrics:

```json
{
	"batch_metrics": {
		"batch_size": 32,
		"average_processing_time": "39.84ms",
		"memory_per_batch": "~3MB",
		"concurrent_batches": 8
	}
}
```

## Resource Utilization

### CPU Usage Patterns:

1. Python Single-Thread:

-  4-5 cores at 100% (PyTorch optimization)
-  Context switching overhead minimal
-  Predictable performance

2. Python Multiprocessing:

```mermaid
graph TD
    A[8 Cores] --> B[100% Usage]
    B --> C[Resource Contention]
    C --> D[Performance Degradation]
    D --> E[Increased Memory Bus Traffic]
```

3. Go + Polycall:

-  Efficient core utilization
-  Minimal context switching
-  Better resource sharing
-  Single model instance benefit

### Memory Analysis

Memory Growth Pattern:

```python
Go + Polycall:
Initial Memory: ~84MB
Processing Memory: ~94MB
Memory Growth: ~10MB

Python Multi:
Initial Memory: ~500MB
Final Memory: ~4GB
Memory Growth: ~3.5GB
```

## Result Consistency

```markdown
Both implementations showed identical results:

-  Total Records: 1000
-  Negative Results: 46
-  Positive Results: 954
```

## Performance Bottlenecks

1. Python Implementation:

```mermaid
graph TD
    A[Bottlenecks] --> B[GIL Limitations]
    A --> C[Memory Overhead]
    A --> D[Process Creation Cost]

    B --> E[Sequential Execution]
    C --> F[Model Duplication]
    D --> G[IPC Overhead]
```

2. Go + Polycall Benefits:

```mermaid
graph TD
    A[Advantages] --> B[Single Runtime]
    A --> C[Efficient Concurrency]
    A --> D[Resource Sharing]

    B --> E[Memory Efficiency]
    C --> F[Better Scaling]
    D --> G[Reduced Overhead]
```

# Real-World Applications

## Use Cases and Benefits

```mermaid
graph TD
    A[Real-World Applications] --> B[ML Microservices]
    A --> C[Data Processing Pipeline]
    A --> D[Edge Computing]
    A --> E[Resource-Constrained Systems]

    B --> B1[API Services]
    C --> C1[ETL Operations]
    D --> D1[IoT Devices]
    E --> E1[Cloud Cost Optimization]
```

## 1. ML Microservices Architecture

### Traditional Approach

```mermaid
graph LR
    A[API Server] --> B[Python ML Service]
    B --> C[New Process per Request]
    C --> D[High Memory Usage]
```

### Polycall Approach

```mermaid
graph LR
    A[Go API Server] --> B[Shared ML Runtime]
    B --> C[Concurrent Processing]
    C --> D[Efficient Resource Use]
```

Example Benefits:

```markdown
| Metric          | Traditional | Polycall | Savings |
| --------------- | ----------- | -------- | ------- |
| Memory/Instance | ~500MB      | ~100MB   | 80%     |
| Startup Time    | ~2-3s       | ~1s      | 50%     |
| Cost/1000 Reqs  | $X          | $X/5     | 80%     |
```

## 2. Edge Computing Applications

```mermaid
graph TD
    subgraph "Edge Device Constraints"
        A[Limited Memory] --> B[Processing Power]
        A --> C[Cost Considerations]
    end

    subgraph "Polycall Solution"
        D[Efficient Resource Use] --> E[Shared Runtime]
        D --> F[Concurrent Processing]
    end
```

Benefits for Edge Deployment:

1. Lower Memory Footprint

   -  Single model instance
   -  Efficient concurrent processing
   -  Minimal overhead

2. Better Resource Utilization
   -  Shared Python runtime
   -  Go's efficient scheduling
   -  Optimized batch processing

## 3. Scaling Considerations

### Vertical Scaling

```mermaid
graph TD
    A[Resource Addition] --> B[Memory Impact]
    B --> C[Traditional: Linear Growth]
    B --> D[Polycall: Minimal Growth]

    C --> E[+500MB per Process]
    D --> F[+2KB per Goroutine]
```

### Horizontal Scaling

Cost Implications:

```python
# Traditional Python Scaling
memory_per_instance = 500 * num_processes  # MB

# Polycall Scaling
memory_per_instance = 100 + (0.002 * num_concurrent_requests)  # MB
```

## 4. Production Implementation

### Best Practices

1. Resource Configuration:

```go
// Optimal settings based on our experiment
type Config struct {
    BatchSize      int     // 32 for balanced throughput
    NumWorkers     int     // CPU core count
    QueueSize      int     // BatchSize * NumWorkers * 2
    MaxConcurrent  int     // Based on available memory
}
```

2. Monitoring Metrics:

-  Processing time per batch
-  Memory usage patterns
-  Queue depths
-  Error rates

3. Error Handling:

```go
// Implement robust error handling
func processBatch(batch []string) error {
    // Set timeouts
    // Handle partial failures
    // Implement retries
    // Monitor resource usage
}
```

## 5. Cost-Benefit Analysis

### Infrastructure Savings

```mermaid
graph TD
    A[Cost Factors] --> B[Memory Usage]
    A --> C[Processing Efficiency]
    A --> D[Resource Utilization]

    B --> E[80% Reduction]
    C --> F[Comparable Speed]
    D --> G[Better Scaling]
```

### Development Benefits

1. Language Flexibility

   -  Use Go for performance
   -  Use Python for ML
   -  Clean separation of concerns

2. Maintenance
   -  Single model instance
   -  Simplified deployment
   -  Easier monitoring

# Conclusions and Future Considerations

## Key Findings

```mermaid
graph TD
    A[Experiment Results] --> B[Performance]
    A --> C[Memory Usage]
    A --> D[Development Experience]

    B --> B1[Comparable to Single Thread]
    B --> B2[Better Than Multiprocessing]

    C --> C1[80% Memory Reduction]
    C --> C2[Better Resource Utilization]

    D --> D1[Easy Integration]
    D --> D2[Best of Both Languages]
```

## Performance Summary

### Processing Metrics

```markdown
| Metric              | Python Single | Python Multi | Go + Polycall |
|---------------------|---------------|--------------|---------------|
| Processing Time (s) | 38           | 220+         | 42            |
| Memory Usage (MB)   | 500          | 4000         | 110           |
| Requests/second     | 26.3         | 4.5          | 23.8          |
| Max Concurrency     | 1            | 8            | 30            |
| ------------------- | ------------- | ------------ | ------------- |
| Processing Time (s) | 38            | 220+         | 40            |
| Memory Usage (MB)   | 500           | 4000         | 94            |
| Scalability         | Limited       | Poor         | Excellent     |
| Resource Efficiency | Moderate      | Poor         | Excellent     |
```

## Advantages and Trade-offs

```mermaid
graph TB
    subgraph "Advantages"
        A1[Memory Efficiency]
        A2[Better Scaling]
        A3[Resource Sharing]
    end

    subgraph "Trade-offs"
        B1[Setup Complexity]
        B2[Learning Curve]
        B3[Initial Configuration]
    end
```

## Future Improvements

1. Performance Optimizations:

```go
// Potential Enhancements
- Dynamic batch sizing
- Adaptive worker pool
- Memory usage optimization
- Smarter queue management
```

2. Feature Additions:

```python
# Possible Extensions
- GPU support
- Multiple model handling
- Auto-scaling capabilities
- Enhanced monitoring
```

## Best Practices Learned

1. Configuration:

```mermaid
graph LR
    A[Optimal Settings] --> B[Batch Size: 32]
    A --> C[Workers: CPU Count]
    A --> D[Queue Size: Dynamic]
```

2. Resource Management:

-  Monitor memory usage
-  Track batch processing times
-  Handle errors gracefully
-  Implement proper shutdown

## Migration Considerations

```mermaid
graph TD
    A[Migration Steps] --> B[Install Polycall]
    B --> C[Port ML Code]
    C --> D[Implement Go Service]
    D --> E[Optimize Settings]

    F[Considerations] --> G[Team Skills]
    F --> H[Infrastructure]
    F --> I[Monitoring]
```

## Final Recommendations

1. Use Case Fit:

-  Large-scale ML processing
-  Resource-constrained environments
-  Microservice architectures
-  Edge computing applications

2. Implementation Strategy:

```markdown
1. Start with small batches
2. Monitor and adjust
3. Gradually scale up
4. Implement proper error handling
5. Set up comprehensive monitoring
```

3. Development Workflow:

```mermaid
graph LR
    A[Development] --> B[Test Small Scale]
    B --> C[Monitor Performance]
    C --> D[Optimize]
    D --> E[Scale Up]
```

## Looking Forward

The experiment demonstrates that Polycall provides:

1. Significant memory savings
2. Comparable performance
3. Better resource utilization
4. Scalability advantages

This approach is particularly valuable for:

-  Production ML deployments
-  Resource-optimized services
-  Cost-effective scaling
-  Modern microservices

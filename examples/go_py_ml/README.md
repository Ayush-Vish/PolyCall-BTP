
# Sentiment Analysis Benchmark: IMDB Dataset

## 📌 Overview

This project benchmarks sentiment analysis on **1000 IMDB movie reviews** (`IMDB.csv`, `review` column) using the model
`distilbert-base-uncased-finetuned-sst-2-english` across three implementations:

* **Python Single-Threaded** → Sequential execution
* **Python Multi-Threaded (Multiprocessing)** → Parallelized with 8 workers
* **Go + Polycall** → Go calling Python ML with Polycall

---

## ⚙️ Settings

* **Dataset**: `IMDB.csv` (1000 reviews)
* **Model**: `distilbert-base-uncased-finetuned-sst-2-english`

  * CPU inference
  * `max_length=512`, `truncation=True`, `padding=True`
  * Python uses `float16` for efficiency
* **Preprocessing**: Remove HTML, URLs, quotes, normalize whitespace, truncate to 512 chars
* **Batch Size**: 16
* **Workers**:

  * Single-thread: 1
  * Multi-thread: 8
  * Go: 8

---

## 📊 Performance Results

| Approach      | Total Time (s) | Processing Rate (reviews/s) | Avg Memory (MB) | Memory Increase (MB) | Sentiment (NEG/POS) | Success Rate |
| ------------- | -------------- | --------------------------- | --------------- | -------------------- | ------------------- | ------------ |
| Python Single | **221.94**     | \~4.5                       | 901.50          | +309.14              | 536 / 464           | 100%         |
| Python Multi  | **629.08**     | \~1.6                       | 706.09          | +69.70               | 523 / 477           | 100%         |
| Go + Polycall | **60.82**      | \~16.4                      | 77.21           | +12.50               | 589 / 411           | 100%         |

✅ **Observations**:

* Multi-threaded Python **slowed down** due to overhead, though memory use was lower.
* Go + Polycall achieved the **best throughput and lowest memory usage**.
* Single-threaded Python performed reasonably well but used more memory.

---

## 🚀 Setup

1. Place `IMDB.csv` in the project root.
2. Install dependencies (Python side):

   ```bash
   pip install transformers torch pandas psutil tqdm
   ```
3. Run benchmarks:

   ```bash
   # Python single-thread
   python textClassifier.py

   # Python multi-thread (multiprocessing)
   python multithreadedTextClassifier.py

   # Go + Polycall
   go run main.go
   ```



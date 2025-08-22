



# Sentiment Analysis Benchmark: IMDB Dataset

## Overview
This project compares sentiment analysis on 1000 IMDB movie reviews (`IMDB.csv`, `review` column) using `distilbert-base-uncased-finetuned-sst-2-english` across three implementations:
- **Python Single-Threaded**: Sequential processing.
- **Python Multiprocessing**: Parallelized with 8 processes.
- **Go + Polycall**: Go with Python ML via Polycall.

## Settings
- **Dataset**: IMDB.csv (1000 reviews)
- **Model**: `distilbert-base-uncased-finetuned-sst-2-english` (CPU, `max_length=512`, `truncation=True`, `padding=True`, `float16` for Python)
- **Preprocessing**: Remove HTML, URLs, quotes; normalize whitespace; truncate to 512 chars
- **Batch Size**: 16
- **Workers**: Go: 8, Multi: 8, Single: 1

## Performance
| Approach            | Time (s) | Avg Memory (MB) | Sentiment (NEG/POS) | Success Rate |
|---------------------|----------|-----------------|---------------------|--------------|
| Python Single       | 55.37    | 891.03          | ~500/500            | 100%         |
| Python Multi        | 629.08   | 706.09          | 523/477             | 100%         |
| Go + Polycall       | 60.82    | 77.21           | 589/411             | 100%         |


## Setup
1. Place `IMDB.csv` in project root.
2. Install dependencies:
   ```bash
   pip install transformers torch pandas psutil tqdm

   ```
3. Run:
   ```bash
   python textClassifier.py
   python multithreadedTextClassifier.py
   go run main.go
   ```


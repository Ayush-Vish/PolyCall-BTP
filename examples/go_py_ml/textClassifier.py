from transformers import pipeline
import pandas as pd
import os
from tqdm import tqdm
import time
import json
from datetime import datetime
import psutil

print(os.getcwd())

# -------------------------------
# Load dataset
# -------------------------------
dataFile = pd.read_csv("./IMDB.csv")
data = dataFile['review'].tolist()   # IMDB dataset usually has 'review' column
reviews = data[:1000]  # only first 1000 reviews
print("First review:", reviews[0])
# -------------------------------
# Load model
# -------------------------------
print("Model is loading...")
classifier = pipeline(
    'text-classification',
    model="distilbert-base-uncased-finetuned-sst-2-english",
    device=-1  # -1 for CPU
)
print("Model loaded!")

# -------------------------------
# Classification functions
# -------------------------------
def classify_text(text: str | list[str]) -> dict | list[dict]:
    print("Processing in the model")
    result = classifier(text, truncation=True, max_length=512)

    print("Result obtained")
    return result[0] if isinstance(text, str) else result

def classifyTextsBatches(text: list[str], batchSize: int = 8) -> list[dict]:
    results = []
    total_records = len(text)
    num_batches = (total_records + batchSize - 1) // batchSize
    
    for i in tqdm(
        range(0, len(text), batchSize),
        desc=f"Processing {total_records} records in {num_batches} batches"
    ):
        batch = text[i:i + batchSize]
        batch_results = classifier(
                            batch,
                            truncation=True,
                            max_length=512
                        )
        results.extend(batch_results)
    return results

# -------------------------------
# Measure time + memory
# -------------------------------
process = psutil.Process(os.getpid())

mem_before = process.memory_info().rss / (1024 * 1024)  # MB
start_time = time.perf_counter()

results = classifyTextsBatches(reviews, batchSize=8)

end_time = time.perf_counter()
mem_after = process.memory_info().rss / (1024 * 1024)  # MB

print(f"\n⏱️ Total processing time: {end_time - start_time:.2f} seconds")
print(f"💾 Memory before classification: {mem_before:.2f} MB")
print(f"💾 Memory after classification: {mem_after:.2f} MB")
print(f"📈 Memory increase: {mem_after - mem_before:.2f} MB")

# -------------------------------
# Analyze results
# -------------------------------
negative_count = sum(1 for r in results if r['label'] == 'NEGATIVE')
positive_count = sum(1 for r in results if r['label'] == 'POSITIVE')

print(f"\nResults Analysis:")
print(f"Total Results: {len(results)}")
print(f"Negative Results: {negative_count}")
print(f"Positive Results: {positive_count}")

# -------------------------------
# Save results
# -------------------------------
if not os.path.exists('results'):
    os.makedirs('results')

results_with_text = []
for i, (text, result) in enumerate(zip(reviews, results)):
    results_with_text.append({
        "index": i,
        "text": text,
        "label": result["label"],
        "score": result["score"]
    })

timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")

detailed_output = {
    'metadata': {
        'total_records': len(results),
        'negative_count': negative_count,
        'positive_count': positive_count,
        'timestamp': timestamp,
        'model': "distilbert-base-uncased-finetuned-sst-2-english"
    },
    'results': results_with_text
}

# Save JSON
json_output_file = f'results/wine_reviews_sentiment_{timestamp}.json'
with open(json_output_file, 'w', encoding='utf-8') as f:
    json.dump(detailed_output, f, indent=2)
print(f"\nDetailed results saved to: {json_output_file}")

# Save CSV
df_results = pd.DataFrame(results_with_text)
csv_output_file = f'results/wine_reviews_sentiment_{timestamp}.csv'
df_results.to_csv(csv_output_file, index=False)
print(f"Results also saved to: {csv_output_file}")

# -------------------------------
# Print model config
# -------------------------------
print("\nModel configuration:")
print(f"Classification labels: {classifier.model.config.id2label}")
""" 

(venv) {0:30}~/Desktop/go-python-ml-example/examples/go_py_ml:main ✗ ➭ python3 textClassifier.py
/home/ayush/Desktop/go-python-ml-example/examples/go_py_ml
/home/ayush/Desktop/go-python-ml-example/examples/go_py_ml/textClassifier.py:15: DtypeWarning: Columns (5,16,17) have mixed types. Specify dtype option on import or set low_memory=False.
  dataFile = pd.read_csv("./wine-reviews.csv")
First description: wine wasn’t that good it needs improvement
Model is loading...


Device set to use cpu
Model loaded!
Processing 1000 records in 125 batches: 100%|████████████████████████████████████████████████████████████████████████████████████████████████████| 125/125 [00:55<00:00,  2.26it/s]

⏱️ Total processing time: 55.37 seconds
💾 Memory before classification: 770.03 MB
💾 Memory after classification: 1012.03 MB
📈 Memory increase: 242.00 MB

Results Analysis:
Total Results: 1000
Negative Results: 46
Positive Results: 954

Detailed results saved to: results/wine_reviews_sentiment_20250823_003306.json
Results also saved to: results/wine_reviews_sentiment_20250823_003306.csv

Model configuration:
Classification labels: {0: 'NEGATIVE', 1: 'POSITIVE'}


"""

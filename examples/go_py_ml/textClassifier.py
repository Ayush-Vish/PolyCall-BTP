from transformers import pipeline
import pandas as pd
import os
from tqdm import tqdm
import time
import json
from datetime import datetime

print(os.getcwd())

dataFile = pd.read_csv("./wine-reviews.csv")
data = dataFile['description'].tolist()
descriptions = data[:1000]
print("First description:", descriptions[0])

print("Model is loading...")
classifier = pipeline('text-classification',
                     model="distilbert-base-uncased-finetuned-sst-2-english",
                     device="cpu")
print("Model loaded!")

def classify_text(text: str | list[str]) -> dict | list[dict]:
    print("Processing in the model")
    result = classifier(text)
    print("Result obtained")
    return result[0] if isinstance(text, str) else result

def classifyTextsBatches(text: list[str], batchSize: int = 8) -> list[dict]:
    results = []
    total_records = len(text)
    num_batches = (total_records + batchSize - 1) // batchSize
    
    for i in tqdm(range(0, len(text), batchSize),
                  desc=f"Processing {total_records} records in {num_batches} batches"):
        batch = text[i:i + batchSize]
        batch_results = classifier(batch)
        results.extend(batch_results)
    return results


results = classifyTextsBatches(descriptions, batchSize=8)

# Count results
negative_count = sum(1 for r in results if r['label'] == 'NEGATIVE')
positive_count = sum(1 for r in results if r['label'] == 'POSITIVE')
print(f"\nResults Analysis:")
print(f"Total Results: {len(results)}")
print(f"Negative Results: {negative_count}")
print(f"Positive Results: {positive_count}")

# Create results directory if it doesn't exist
if not os.path.exists('results'):
    os.makedirs('results')

# Prepare results with text
results_with_text = []
for i, (text, result) in enumerate(zip(descriptions, results)):
    results_with_text.append({
        "index": i,
        "text": text,
        "label": result["label"],
        "score": result["score"]
    })

# Save detailed results to JSON
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

# Save to files
json_output_file = f'results/wine_reviews_sentiment_{timestamp}.json'
with open(json_output_file, 'w', encoding='utf-8') as f:
    json.dump(detailed_output, f, indent=2)
print(f"\nDetailed results saved to: {json_output_file}")

# Save to CSV
df_results = pd.DataFrame(results_with_text)
csv_output_file = f'results/wine_reviews_sentiment_{timestamp}.csv'
df_results.to_csv(csv_output_file, index=False)
print(f"Results also saved to: {csv_output_file}")

# Print model configuration
print("\nModel configuration:")
print(f"Classification threshold: {classifier.model.config.id2label}")
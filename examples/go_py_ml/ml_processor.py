from transformers import pipeline
import pandas as pd
import time 

initialTime = time.time()
print("Initializing model...")
classifier = pipeline('text-classification',
    model="distilbert-base-uncased-finetuned-sst-2-english",
    device="cpu")
print("Model loaded!")

print(f"Time taken to load: {time.time()-initialTime}")

def process_batch(texts: list[str]) -> list[dict]:
    try:
        # print(f"Python processing {len(texts)} texts")  
        results = classifier(texts)
        # print(f"Python results type: {type(results)}")
        # print(f"First result: {results[0]}")
        return results
    except Exception as e:
        print(f"Error processing batch: {str(e)}")
        return []

def get_model_info() -> dict:
    return {
        "model_name": classifier.model.name_or_path,
        "model_type": "text-classification",
        "device": str(classifier.device)
    }

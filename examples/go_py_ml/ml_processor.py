from transformers import pipeline
import pandas as pd
import time
import re

initialTime = time.time()
print("Initializing model...")

classifier = pipeline(
    "text-classification",
    model="distilbert-base-uncased-finetuned-sst-2-english",
    device=-1,  # use CPU (use 0 for GPU if available)
)

print("Model loaded!")
print(f"Time taken to load: {time.time()-initialTime:.2f}s")

def clean_text(text: str) -> str:
    """Standardize text cleaning to match Go implementation."""
    # Remove HTML tags
    text = re.sub(r'<[^>]+>', '', str(text))
    # Replace newlines with spaces
    text = text.replace('\n', ' ')
    # Remove extra quotes
    text = text.replace('"', '')
    # Normalize whitespace
    text = ' '.join(text.split())
    # Truncate to 512 characters to approximate 512 tokens
    if len(text) > 512:
        text = text[:512] + "...[TRUNCATED]"
    return text.strip()

def process_batch(texts: list[str]) -> list[dict]:
    try:
        # Clean IMDB reviews to match Go preprocessing
        cleaned_texts = [clean_text(text) for text in texts if str(text).strip()]

        if not cleaned_texts:
            return []

        # Pass truncation and padding explicitly
        results = classifier(
            cleaned_texts,
            truncation=True,
            max_length=512,
            padding=True
        )

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

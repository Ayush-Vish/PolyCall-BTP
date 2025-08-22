from transformers import pipeline
import pandas as pd
import multiprocessing as mp
import time
import psutil
from tqdm import tqdm
import os
import json
from datetime import datetime
import traceback
import re
import gc
import torch
from concurrent.futures import ProcessPoolExecutor, as_completed
import logging

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

def clean_text(text):
    """Clean and preprocess text efficiently."""
    if not isinstance(text, str):
        return ""
    # More efficient regex compilation and cleaning
    text = re.sub(r'<[^>]+>', '', text)  # Remove HTML tags
    text = re.sub(r'http\S+|www\S+|https\S+', '', text)  # Remove URLs
    text = text.replace('\n', ' ').replace('"', '')
    text = ' '.join(text.split()).strip()
    # Truncate at word boundary for better results
    if len(text) > 512:
        words = text[:512].split()
        text = ' '.join(words[:-1]) + "..."
    return text

def get_process_memory():
    """Get current process memory usage in MB."""
    try:
        process = psutil.Process(os.getpid())
        return process.memory_info().rss / 1024 / 1024
    except:
        return 0.0

def get_cpu_usage():
    """Get current process CPU usage."""
    try:
        return psutil.Process(os.getpid()).cpu_percent()
    except:
        return 0.0

def check_memory_available():
    """Check available system memory in MB."""
    return psutil.virtual_memory().available / 1024 / 1024

def process_batch_optimized(texts, process_id):
    """
    Optimized batch processing with better memory management.
    Each process loads its own model instance to avoid sharing issues.
    """
    metrics = {
        'process_id': process_id,
        'start_time': time.time(),
        'start_memory': get_process_memory(),
        'model_load_time': 0.0,
        'processing_time': 0.0,
        'total_time': 0.0,
        'success_count': 0,
        'error_count': 0,
        'peak_memory': 0.0
    }
    
    results = []
    
    try:
        logger.info(f"Process {process_id}: Starting with {len(texts)} texts")
        
        # Load model with optimized settings
        model_start = time.time()
        
        # Use a smaller, faster model for better performance
        classifier = pipeline(
            'text-classification',
            model="distilbert-base-uncased-finetuned-sst-2-english",
            device=-1,  # Force CPU usage
            truncation=True,
            max_length=256,  # Reduced max length
            padding=True,
            return_all_scores=False,  # Only return top prediction
            use_fast=True  # Use fast tokenizer
        )
        
        metrics['model_load_time'] = time.time() - model_start
        logger.info(f"Process {process_id}: Model loaded in {metrics['model_load_time']:.2f}s")
        
        # Process in smaller batches for better memory management
        batch_size = 8  # Reduced batch size
        process_start = time.time()
        
        # Pre-clean all texts
        cleaned_texts = [clean_text(text) for text in texts]
        cleaned_texts = [text for text in cleaned_texts if text and len(text) > 10]
        
        logger.info(f"Process {process_id}: Processing {len(cleaned_texts)} cleaned texts")
        
        for i in range(0, len(cleaned_texts), batch_size):
            try:
                batch = cleaned_texts[i:i + batch_size]
                if not batch:
                    continue
                
                # Process batch
                batch_results = classifier(batch)
                results.extend(batch_results)
                metrics['success_count'] += len(batch_results)
                
                # Memory management
                current_memory = get_process_memory()
                metrics['peak_memory'] = max(metrics['peak_memory'], current_memory)
                
                # Force garbage collection periodically
                if (i // batch_size) % 10 == 0:
                    gc.collect()
                    if torch.cuda.is_available():
                        torch.cuda.empty_cache()
                
            except Exception as batch_error:
                logger.error(f"Process {process_id}: Batch error at {i}: {str(batch_error)}")
                metrics['error_count'] += len(batch) if 'batch' in locals() else 1
                continue
        
        metrics['processing_time'] = time.time() - process_start
        metrics['total_time'] = time.time() - metrics['start_time']
        
        logger.info(f"Process {process_id}: Completed {metrics['success_count']} items in {metrics['total_time']:.2f}s")
        
    except Exception as e:
        logger.error(f"Process {process_id}: Fatal error: {str(e)}")
        metrics['error'] = {
            'error_type': type(e).__name__,
            'error_message': str(e),
            'traceback': traceback.format_exc()
        }
    
    finally:
        # Cleanup
        if 'classifier' in locals():
            del classifier
        gc.collect()
        if torch.cuda.is_available():
            torch.cuda.empty_cache()
    
    return {'metrics': metrics, 'results': results}

def main():
    """Main execution function with improved error handling and monitoring."""
    os.makedirs('metrics', exist_ok=True)
    
    total_start = time.time()
    initial_memory = get_process_memory()
    
    print("Loading IMDB CSV file...")
    csv_start = time.time()
    
    try:
        # Load data with better error handling
        data_file = pd.read_csv("IMDB.csv")
        
        # Find review column more robustly
        review_column = None
        possible_columns = ['review', 'Review', 'text', 'comment', 'comments', 'Review_Text', 'reviewText']
        
        for col in possible_columns:
            if col in data_file.columns:
                review_column = data_file[col].dropna().tolist()
                print(f"Found review data in column: '{col}'")
                break
        
        if review_column is None:
            print(f"Available columns: {list(data_file.columns)}")
            raise ValueError("No review column found in IMDB.csv")
        
        # Limit to manageable dataset size
        reviews = review_column[:1000]  # Process first 1000 reviews
        csv_time = time.time() - csv_start
        print(f"Loaded {len(reviews)} reviews in {csv_time:.2f} seconds")
        
    except Exception as e:
        print(f"Error loading CSV: {str(e)}")
        return 1
    
    print(f"Initial memory usage: {initial_memory:.2f} MB")
    print(f"Available memory: {check_memory_available():.2f} MB")
    
    # Optimize process count based on available memory
    available_memory = check_memory_available()
    estimated_memory_per_process = 800  # MB per process (conservative estimate)
    
    max_processes_by_memory = int(available_memory // estimated_memory_per_process)
    max_processes_by_cpu = min(4, mp.cpu_count())  # Limit to 4 for stability
    
    num_processes = min(max_processes_by_memory, max_processes_by_cpu, 4)
    
    if num_processes < 1:
        num_processes = 1
        print("Warning: Low memory available, using single process")
    
    print(f"Using {num_processes} processes")
    
    # Split data more evenly
    chunk_size = len(reviews) // num_processes
    remainder = len(reviews) % num_processes
    
    chunks = []
    start_idx = 0
    for i in range(num_processes):
        end_idx = start_idx + chunk_size + (1 if i < remainder else 0)
        chunks.append(reviews[start_idx:end_idx])
        start_idx = end_idx
    
    print(f"Created {len(chunks)} chunks: {[len(chunk) for chunk in chunks]}")
    
    # Process with ProcessPoolExecutor for better management
    all_results = []
    all_metrics = []
    
    print("\nStarting parallel processing...")
    
    with ProcessPoolExecutor(max_workers=num_processes) as executor:
        # Submit all tasks
        future_to_process = {
            executor.submit(process_batch_optimized, chunk, i): i 
            for i, chunk in enumerate(chunks)
        }
        
        # Monitor progress
        with tqdm(total=len(reviews), desc="Processing Reviews") as pbar:
            for future in as_completed(future_to_process):
                process_id = future_to_process[future]
                try:
                    result = future.result(timeout=300)  # 5 minute timeout per process
                    
                    if result['results']:
                        all_results.extend(result['results'])
                        pbar.update(len(result['results']))
                    
                    all_metrics.append(result['metrics'])
                    
                    logger.info(f"Process {process_id} completed: "
                              f"{result['metrics']['success_count']} successful, "
                              f"{result['metrics']['error_count']} errors")
                    
                except Exception as e:
                    logger.error(f"Process {process_id} failed: {str(e)}")
                    # Add empty metrics for failed process
                    all_metrics.append({
                        'process_id': process_id,
                        'error': str(e),
                        'total_time': 0,
                        'success_count': 0
                    })
    
    total_time = time.time() - total_start
    final_memory = get_process_memory()
    
    # Calculate sentiment distribution
    sentiment_counts = {'POSITIVE': 0, 'NEGATIVE': 0}
    confidence_scores = []
    
    for result in all_results:
        label = result['label'].upper()
        if label in sentiment_counts:
            sentiment_counts[label] += 1
        confidence_scores.append(result['score'])
    
    # Calculate summary statistics
    avg_confidence = sum(confidence_scores) / len(confidence_scores) if confidence_scores else 0
    total_processed = len(all_results)
    processing_rate = total_processed / total_time if total_time > 0 else 0
    
    # Compile final metrics
    final_metrics = {
        'execution_summary': {
            'total_execution_time': total_time,
            'csv_load_time': csv_time,
            'total_processed': total_processed,
            'processing_rate': processing_rate,
            'success_rate': (total_processed / len(reviews)) * 100
        },
        'memory_usage': {
            'initial_memory': initial_memory,
            'final_memory': final_memory,
            'memory_increase': final_memory - initial_memory,
            'peak_memory': max([m.get('peak_memory', 0) for m in all_metrics])
        },
        'sentiment_analysis': {
            'distribution': sentiment_counts,
            'average_confidence': avg_confidence,
            'total_reviews': total_processed
        },
        'process_metrics': all_metrics,
        'system_info': {
            'processes_used': num_processes,
            'cpu_count': psutil.cpu_count(),
            'total_memory_mb': psutil.virtual_memory().total / (1024 * 1024),
            'available_memory_mb': check_memory_available(),
            'platform': os.uname().sysname if hasattr(os, 'uname') else 'Unknown'
        },
        'timestamp': datetime.now().isoformat()
    }
    
    # Save metrics
    metrics_file = f'metrics/imdb_optimized_run_{int(time.time())}.json'
    with open(metrics_file, 'w') as f:
        json.dump(final_metrics, f, indent=2)
    
    # Print comprehensive summary
    print("\n" + "="*60)
    print("EXECUTION SUMMARY")
    print("="*60)
    print(f"Total execution time: {total_time:.2f} seconds")
    print(f"Processing rate: {processing_rate:.1f} reviews/second")
    print(f"Success rate: {(total_processed / len(reviews)) * 100:.1f}%")
    print(f"\nMemory Usage:")
    print(f"  Initial: {initial_memory:.2f} MB")
    print(f"  Final: {final_memory:.2f} MB")
    print(f"  Increase: {final_memory - initial_memory:.2f} MB")
    print(f"\nResults:")
    print(f"  Total processed: {total_processed}/{len(reviews)} reviews")
    print(f"  Sentiment distribution: {sentiment_counts}")
    print(f"  Average confidence: {avg_confidence:.3f}")
    print(f"\nMetrics saved to: {metrics_file}")
    print("="*60)
    
    return 0

if __name__ == "__main__":
    exit(main())
"""


===========================================================
EXECUTION SUMMARY
============================================================
Total execution time: 629.08 seconds
Processing rate: 1.6 reviews/second
Success rate: 100.0%

Memory Usage:
  Initial: 662.73 MB
  Final: 732.44 MB
  Increase: 69.70 MB

Results:
  Total processed: 1000/1000 reviews
  Sentiment distribution: {'POSITIVE': 477, 'NEGATIVE': 523}
  Average confidence: 0.974
"""

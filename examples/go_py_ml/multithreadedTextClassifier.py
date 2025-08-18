from transformers import pipeline
import pandas as pd
import multiprocessing
import time
import psutil
from tqdm import tqdm
import os
import json
from datetime import datetime
import traceback

def get_process_memory():
    process = psutil.Process(os.getpid())
    return process.memory_info().rss / 1024 / 1024  

def get_cpu_usage():
    return psutil.Process(os.getpid()).cpu_percent()

def check_memory_available():
    return psutil.virtual_memory().available / 1024 / 1024  

def worker_function(chunk, process_id, result_queue, status_queue):
    try:
        metrics = {
            'process_id': process_id,
            'chunk_size': len(chunk),
            'start_time': time.time()
        }
        
        metrics['start_memory'] = get_process_memory()
        status_queue.put(('start', process_id, metrics['start_memory']))
        
        model_start = time.time()
        classifier = pipeline('text-classification', 
                            model="distilbert-base-uncased-finetuned-sst-2-english",
                            device="cpu")
        metrics['model_load_time'] = time.time() - model_start
        metrics['post_model_memory'] = get_process_memory()
        metrics['model_memory_increase'] = metrics['post_model_memory'] - metrics['start_memory']
        status_queue.put(('model_loaded', process_id, metrics['model_memory_increase']))

        process_start = time.time()
        batch_size = 16  
        results = []
        
        for i in range(0, len(chunk), batch_size):
            batch = chunk[i:i + batch_size]
            try:
                pre_batch_memory = get_process_memory()
                
                batch_results = classifier(batch)
                results.extend(batch_results)
                
                post_batch_memory = get_process_memory()
                batch_memory_increase = post_batch_memory - pre_batch_memory
                
                status_queue.put(('progress', process_id, {
                    'processed': len(batch),
                    'memory_increase': batch_memory_increase,
                    'cpu_usage': get_cpu_usage()
                }))
                
            except Exception as batch_error:
                status_queue.put(('batch_error', process_id, {
                    'batch_index': i,
                    'error': str(batch_error)
                }))
                continue

        metrics.update({
            'processing_time': time.time() - process_start,
            'total_time': time.time() - metrics['start_time'],
            'success_count': len(results),
            'end_memory': get_process_memory(),
            'final_cpu_usage': get_cpu_usage(),
            'memory_peak': max(metrics['post_model_memory'], metrics['end_memory'])
        })
        
        result_queue.put({'metrics': metrics, 'results': results})
        status_queue.put(('complete', process_id, metrics))
        
    except Exception as e:
        error_info = {
            'error_type': type(e).__name__,
            'error_message': str(e),
            'traceback': traceback.format_exc()
        }
        status_queue.put(('error', process_id, error_info))
        metrics['error'] = error_info
        result_queue.put({'metrics': metrics, 'results': None})

if __name__ == "__main__":
    os.makedirs('metrics', exist_ok=True)
    
    total_start = time.time()
    initial_memory = get_process_memory()
    
    print("Loading CSV file...")
    csv_start = time.time()
    data_file = pd.read_csv("./wine-reviews.csv")
    description_column = data_file["description"].tolist()
    shortened_description = description_column[:1000]
    csv_time = time.time() - csv_start
    
    print(f"CSV load time: {csv_time:.2f} seconds")
    print(f"Initial memory usage: {initial_memory:.2f} MB")
    print(f"Available memory: {check_memory_available():.2f} MB")
    
    num_processes = 8
    chunk_size = len(shortened_description) // num_processes
    chunks = [shortened_description[i:i + chunk_size] 
             for i in range(0, len(shortened_description), chunk_size)]
    
    result_queue = multiprocessing.Queue()
    status_queue = multiprocessing.Queue()
    processes = []
    
    print("\nStarting processes with staggered initialization...")
    process_metrics = {}
    
    for i, chunk in enumerate(chunks):
        available_memory = check_memory_available()
        if available_memory < 1000:
            print(f"Warning: Low memory available ({available_memory:.2f} MB)")
            time.sleep(5)
        
        p = multiprocessing.Process(
            target=worker_function,
            args=(chunk, i, result_queue, status_queue)
        )
        processes.append(p)
        p.start()
        print(f"Started process {i}")
        time.sleep(2)  
    
    active_processes = set(range(num_processes))
    total_items = len(shortened_description)
    processed_items = 0
    
    with tqdm(total=total_items, desc="Overall Progress") as pbar:
        while active_processes:
            try:
                status_type, process_id, data = status_queue.get(timeout=60)
                
                if status_type == 'start':
                    print(f"\nProcess {process_id} started with {data:.2f}MB initial memory")
                elif status_type == 'model_loaded':
                    print(f"\nProcess {process_id} loaded model (+{data:.2f}MB)")
                elif status_type == 'progress':
                    processed_items += data['processed']
                    pbar.update(data['processed'])
                    print(f"\nProcess {process_id}: CPU {data['cpu_usage']:.1f}% | "
                          f"Memory +{data['memory_increase']:.2f}MB")
                elif status_type == 'complete':
                    active_processes.remove(process_id)
                    process_metrics[process_id] = data
                    print(f"\nProcess {process_id} completed in {data['total_time']:.2f}s")
                elif status_type == 'error':
                    print(f"\nError in process {process_id}:")
                    print(f"Type: {data['error_type']}")
                    print(f"Message: {data['error_message']}")
                    active_processes.remove(process_id)
                
            except Exception as e:
                print(f"\nError monitoring processes: {str(e)}")
                break
    
    for p in processes:
        p.join(timeout=30)
        if p.is_alive():
            print(f"Warning: Force terminating process {p.pid}")
            p.terminate()
    
    print("\nCollecting results...")
    all_results = []
    all_metrics = []
    
    while not result_queue.empty():
        result = result_queue.get()
        if result['results']:
            all_results.extend(result['results'])
        all_metrics.append(result['metrics'])
    
    total_time = time.time() - total_start
    final_memory = get_process_memory()
    
    final_metrics = {
        'total_execution_time': total_time,
        'csv_load_time': csv_time,
        'initial_memory': initial_memory,
        'final_memory': final_memory,
        'memory_increase': final_memory - initial_memory,
        'total_processed': len(all_results),
        'process_metrics': all_metrics,
        'timestamp': datetime.now().isoformat(),
        'system_info': {
            'cpu_count': psutil.cpu_count(),
            'total_memory': psutil.virtual_memory().total / (1024 * 1024),
            'platform': os.uname().sysname
        }
    }
    
    metrics_file = f'metrics/run_metrics_{int(time.time())}.json'
    with open(metrics_file, 'w') as f:
        json.dump(final_metrics, f, indent=2)
    
    print("\nExecution Summary:")
    print(f"Total time: {total_time:.2f} seconds")
    print(f"Memory usage: {final_memory:.2f} MB (increased by {final_memory - initial_memory:.2f} MB)")
    print(f"Processed {len(all_results)} items successfully")
    print(f"Detailed metrics saved to: {metrics_file}")
function fibonaccipy(n) {
    // Calculate nth Fibonacci number recursively
    if (n <= 0) {
        return 0;
    }
    if (n === 1) {
        return 1;
    }
    return fibonaccipy(n - 1) + fibonaccipy(n - 2);
}

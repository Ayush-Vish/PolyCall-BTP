def fibonaccijs(n):
    """Calculate nth Fibonacci number recursively."""
    if n <= 0:
        return 0
    if n == 1:
        return 1
    return fibonaccijs(n - 1) + fibonaccijs(n - 2)

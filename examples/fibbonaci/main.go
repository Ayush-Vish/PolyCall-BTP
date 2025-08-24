package main

import (
	"log"

	metacall "github.com/metacall/core/source/ports/go_port/source"
)

func main() {
	if err := metacall.LoadFromFile("py", []string{"fib.py"}); err != nil {
		log.Fatalf("Failed to load fib.py: %v", err)
	}
	if err := metacall.LoadFromFile("node", []string{"fib.js"}); err != nil {
		log.Fatalf("Failed to load fib.js: %v", err)
	}
	log.Default().Println("Scripts loaded successfully")
	// Load Python script
	// err = metacall.LoadFromFile("python", []string{"fib.py"})
	// if err != nil {
	// 	log.Fatalf("Failed to load fib.py: %v", err)
	// }

	// // Load JavaScript script
	// err = metacall.LoadFromFile("node", []string{"fib.js"})
	// if err != nil {
	// 	log.Fatalf("Failed to load fib.js: %v", err)
	// }
	// log.Default().Println("Scripts loaded successfully")

	// // Input for Fibonacci calculation
	// n := 10

	// log.Default().Printf("Calculating Fibonacci(%d) using Python and JavaScript\n", n)
	// // Call Python Fibonacci function
	// pyResult, err := metacall.Call("fibonaccipy", n)
	// if err != nil {
	// 	log.Fatalf("Failed to call Python fibonaccipy: %v", err)
	// }
	// pyFib, ok := pyResult.(int64)
	// if !ok {
	// 	log.Fatalf("Python result type mismatch")
	// }

	// // Call JavaScript Fibonacci function
	// jsResult, err := metacall.Call("fibonaccijs", n)
	// if err != nil {
	// 	log.Fatalf("Failed to call JavaScript fibonaccijs: %v", err)
	// }
	// jsFib, ok := jsResult.(float64) // JavaScript numbers are float64 in MetaCall
	// if !ok {
	// 	log.Fatalf("JavaScript result type mismatch")
	// }

	// // Print results
	// fmt.Printf("Fibonacci(%d) from Python: %d\n", n, pyFib)
	// fmt.Printf("Fibonacci(%d) from JavaScript: %d\n", n, int(jsFib))
}

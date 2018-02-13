package main

import (
	"fmt"
	"math"
)

func main() {
	sumOfSquares := 0
	squareOfSums := 0
	for i:=1; i <= 100; i++{
		sumOfSquares += i*i
		squareOfSums += i
	}
	squareOfSums = squareOfSums * squareOfSums
	fmt.Printf("sq of sums = %d\n", squareOfSums)
	fmt.Printf("sum of sqs = %d\n", sumOfSquares)
	diff := int(math.Abs(float64(squareOfSums) - float64(sumOfSquares)))
	fmt.Printf("Diff = %d\n", diff)
}
package main

import "fmt"

func isPrime (n int) bool {
	if n <= 1{
		return false
	} else if n <= 3 {
		return true
	} else if n % 2 == 0 || n % 3 == 0 {
		return false
	}
	i := 5
	for {
		if i * i > n{
			return true
		}
		if n % i == 0 || n % (i+2) == 0 {
			return false
		}
		i += 6
	}
}

func main() {
	s := make([]int, 1)
	for i:=0; len(s) <= 10002; i++{
		if isPrime(i) {
			s = append(s, i)
		}
	}
	fmt.Printf("The 10001st prime number is: %d \n", s[10001])
}
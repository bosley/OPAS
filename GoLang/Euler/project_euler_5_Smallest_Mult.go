package main

import "fmt"

func check(n, k int) bool {
	if n % k == 0 {
		return true
	}
	return false
}

func main() {
    n := 0
	i := 0
	for {
		n++
		i = 1
		for k:= 1; k <=20; k++ {
			if check(n,k) == true{
				i++
			} else {
				k = 20
			}
		}
		if i > 20{
			break
		}
	}
	fmt.Printf("%d is the smallest divisible by all numbers 1->20\n", n)
}
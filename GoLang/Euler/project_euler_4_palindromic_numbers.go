package main

import (
	"fmt"
	"strconv"
)

func rev(s string) string {
	var r string
	for i := len(s)-1; i >=0; i--{
		r += string(s[i])
	}
	return r
}

func main() {
	largest := 0
	for i:= 100; i < 1000; i++{
		for j:= 100; j < 1000; j++{
			n := i * j
			s := strconv.Itoa(n)
			if s == rev(s){
				if n > largest{
					largest = n
				}
			}
		}
	}
	fmt.Printf("The largest palindromic number is: %d\n", largest)
}
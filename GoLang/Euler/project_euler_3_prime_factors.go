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

func main(){
	num := 600851475143
	for i:=0; i <= num; i++ {
		if isPrime(i){
			//fmt.Printf("%d is prime\n", i)
			if num % i == 0 {
				fmt.Printf("%d \tis a prime factor.\n", i)
			}
		}
	}
}
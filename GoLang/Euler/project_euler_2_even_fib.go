package main

import "fmt"

func main(){
	var n int64
	var sum int64
	n = 2
	sum = 0
	store := make([]int64, 2)
	store[0] = 1
	store[1] = 1

	for {
		c := store[n-1] + store[n-2]
		fmt.Println(c)
		store = append(store, c)
		n++
		if c % 2 == 0{
			sum += c
		}

		if c > 4000000 {
			break
		}
	}

	fmt.Printf("sum=%d\n", sum)
	fmt.Println("fin.")
}
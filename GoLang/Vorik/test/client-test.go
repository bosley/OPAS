package main


import (
	"fmt"
	vconn "github.com/joshbosley/Vorik/connector"
)

func main(){
	fmt.Println("Attempting to connect to -> 127.0.0.1:4096")

	vconn.GenerateAndPerformFirstRegistration("/tmp/test_keys/", "A tester", "127.0.0.1:4096")
}
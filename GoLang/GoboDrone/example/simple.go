package main

import (
	"fmt"
	drone "github.com/joshbosley/GoboDrone"
)

func initializeDrone(port string){

	// Create a new device
	dev := drone.New()

	// Set device specific action
	dev.Interact(func(c * drone.Command, respond chan drone.Response) {

			switch c.Instruct {

			case "hello":
				respond <- drone.Response{42, []byte("world")}

			case "ping":
				respond <- drone.Response{0, []byte("pong")}

			case "doSomething":
				fmt.Println("Got a request to do something with parameters: ", c.Params)

				yourReturnCode := 7
				yourResponseToTheAction := []byte("Some response specific to the action")

				respond <- drone.Response{yourReturnCode, yourResponseToTheAction}
	
			default:
				respond <- drone.Response{-1, []byte("Unknown instruction")}
			}
	})

	// Start the drone server
	drone.Serve(dev, port)
}

func main() {

	initializeDrone("4092")

	remote := "127.0.0.1:4092"

	hello := drone.Command{"hello", []string{}}

	ping := drone.Command{"ping", []string{}}

	doSomething := drone.Command{
		Instruct: "doSomething",
		Params: []string{"param0", "param1", "param2"},
	}

	response := drone.SendRequest(remote, hello)
	fmt.Println("Response to hello: ", response.Code)
	fmt.Printf("%q\n", response.Data)


	response = drone.SendRequest(remote, ping)
	fmt.Println("Response to ping: ", response.Code)
	fmt.Printf("%q\n", response.Data)


	response = drone.SendRequest(remote, doSomething)
	fmt.Println("Response to doSomething: ", response.Code)
	fmt.Printf("%q\n", response.Data)
	

}
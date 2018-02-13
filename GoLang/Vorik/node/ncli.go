package node

import(
	"os"
	"fmt"
	"net"
	"bufio"
	"strings"
	"strconv"
)

/*
	Ensure that the user wants to do something
*/
func questionUserChoice(prompt string) bool {

	ri := bufio.NewReader(os.Stdin)
	fmt.Print(prompt," (y/n): ")
	r, _ := ri.ReadString('\n')
	return ("Y\n" == r)
}

/*
	Get some input from a user
*/
func promptUserForInput(prompt string) string{

	ri := bufio.NewReader(os.Stdin)
	fmt.Print("> ")
	r, _ := ri.ReadString('\n')
	r = strings.Replace(r, "\n", "", -1)
	return r
}

/*
	Main start function for node
*/
func StartCli() {

	var v VNode

	// Set operational state
	v.nodestate = "cli"
	
	// Load the node settings
	v.loadNodeSettings()
	
	// Initiate remote procedure call listener
	v.initiateRpc()

	// Our interface to the core routine
	chatchan := make(chan []string)

	// Tell the node core to go do its thing!
	go v.nodeCore(chatchan)

	// Start serving as node
	for {

		// Get something from the user
		r := promptUserForInput("> ")

		// Split the input into different command chunks for interacting with
		// the node functionality
		inpfields := strings.Fields(r)  
		
		// Ignore blankness
		if len(inpfields) == 0{
			continue
		}

		// Check the first item and route the commands
		switch inpfields[0] {
		case "addrem":

			// Ensure all data exists
			if len(inpfields) != 3 {
				fmt.Println("Syntax Error. Usage: addrem ip port")
				continue
			}

			// Ensure valid IP
			if net.ParseIP(inpfields[1]) == nil {
				fmt.Println("Syntax Error. Invalid IP")
				continue
			}

			// Ensure valid port
			if _, err := strconv.Atoi(inpfields[2]); err != nil {
				fmt.Println("Syntax Error. Invalid Port")
				continue
			}

			// Send a command to the node core
			chatchan <- inpfields[1:]

		default:
			fmt.Println("Unknown command...")
		}
	}
}
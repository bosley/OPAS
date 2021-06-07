package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"strconv"
	"strings"
	CIPCS "ObeliskPOC/internal/cipcs"
	NIS "ObeliskPOC/internal/nis"
	RunInf "ObeliskPOC/internal/runinf"
	. "github.com/logrusorgru/aurora"
)

func shutdownObelisk(){
	fmt.Println(BrightBlue("Attempting to shutdown Obelisk..."))

	var client CIPCS.Client

	err := client.Init("127.0.0.1:"+strconv.Itoa(RunInf.CIPCS_PORT))

	if err != nil {
		fmt.Println(Red("FAIL: "), "Unable to init connection. Is an Obelisk running?")
		return
	}

	err = client.ShutdownObelisk()

	if err != nil {
		fmt.Println(Red("FAIL: "), err)
		return
	}

	fmt.Println(Green("COMPLETE: "), "Obelisk should be down")
	client.Close()
}

func nisReport(line string, active bool){
	fmt.Print(line)
	if active {
		fmt.Println(Green("ACTIVE"))
	} else {
		fmt.Println(Red("INACTIVE"))
	}
}

func getNis(remote string){
	fmt.Println(BrightBlue("Attempting to get NIS from :"), remote, BrightBlue("..."))

	var client NIS.Client

	err := client.Init(remote+":"+strconv.Itoa(RunInf.NIS_PORT))

	if err != nil {
		fmt.Println(Red("FAIL: "), "Unable to init connection. Is an Obelisk running?")
		return
	}

	var nsResp NIS.NisData

	nsResp, eerr := client.Execute()
	if eerr != nil {
		fmt.Println(Red("FAIL: "), eerr)
		return
	}

	nisReport("Node Sender Service ... " , nsResp.Nss)
	nisReport("Node Recvr  Service ... " , nsResp.Nrs)
	nisReport("Node Oracle Service ... " , nsResp.Nos)
	nisReport("Node Pillar Service ... " , nsResp.Nps)
}

// -------------------- Interaction Methods --------------------

func interactGetNis(){
	reader := bufio.NewReader(os.Stdin)
	fmt.Print(BrightBlue("Enter Node IP: "))
	text, _ := reader.ReadString('\n')
	text = strings.TrimSuffix(text, "\n")
	getNis(text)
}

func interactHelp(){
	fmt.Println("\nCommands: \n")
	fmt.Println("\tshutdown	- Signal shutdown to local Obelisk")
	fmt.Println("\thelp		- Show help")
	fmt.Println("\tnis		- Send request for NIS")
	fmt.Println("\tquit		- Exit cmdr")
	fmt.Println("")
}

func interactQuit(){
	fmt.Println(BrightBlue("Exiting"))
	os.Exit(0)
}

func interactiveMode(){
	
	commandMap := map[string]interface{} {
		"shutdown": shutdownObelisk,
		"help": interactHelp,
		"nis": interactGetNis,
		"quit": interactQuit}

	reader := bufio.NewReader(os.Stdin)
	for {
		fmt.Print(Cyan("cmdr>> "))
		text, _ := reader.ReadString('\n')
		text = strings.TrimSuffix(text, "\n")

		if text == ""{
			continue
		}

		val := commandMap[text]

		if val == nil {
			fmt.Println(Red("Error: "), "Unknown command. Use 'help' to see commands")
		} else {
			val.(func())()
		}
	}

	fmt.Println(Red("FAIL: "), Yellow("interactive mode not yet implemented!"))
}
// ------------------ End Interaction Methods ------------------

func main(){

	fmt.Println(BrightGreen("ObeliskPOC ["), Yellow("commander"), BrightGreen("]"))

	// Help
	helpPtr := flag.Bool("h", false, "Get some help")

	// Config
	interactivePtr := flag.Bool("i", false, "Start in 'interactive' mode")
	shutdownPtr := flag.Bool("shutdown", false, "Shutdown local Obelisk")
	getNisPtr := flag.Bool("nis", false, "Request NIS from a node.")
	hostPtr := flag.String("host", "127.0.0.1", "Address of node to interact with")

	flag.Parse()

	if *helpPtr {
		flag.PrintDefaults()
		os.Exit(0)
	}

	if *interactivePtr {
		fmt.Println("Starting in ", Yellow("interactive"), " mode. Ignoring any other flags")
		interactiveMode()
		os.Exit(0)
	}

	if *shutdownPtr {
		shutdownObelisk()
	} else if *getNisPtr {
		getNis(*hostPtr)
	} else {
		fmt.Println(Red("Error: "), "Nothing to do. Use -h for help")
	}
}
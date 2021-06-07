//
//	Josh A. Bosley
//	ObeliskPOC cli
//

package main

import (
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"net"
	"os"
    "os/signal"
	"strings"
	"sync"
	"time"
	Keyman "ObeliskPOC/internal/keyman"
	Config "ObeliskPOC/internal/configuration"
	RunInf "ObeliskPOC/internal/runinf"
	CIPCS "ObeliskPOC/internal/cipcs"
	NIS "ObeliskPOC/internal/nis"
	. "github.com/logrusorgru/aurora"
)

var ObeliskKillChan chan os.Signal
var ObeliskConfig Config.LocalConfig
var ObeliskRunInf RunInf.RunInf

// Methods to start services

func startCommandService() {
	ObeliskRunInf.SetStatusCommand(true)
	fmt.Print(BrightBlue("Starting command service..."))

	go CIPCS.CommanderIPCService(&ObeliskRunInf, ObeliskKillChan)
	
	fmt.Println(Green("Started"))
}

func startNodeInformationService(){
	ObeliskRunInf.SetStatusInformation(true)
	fmt.Print(BrightBlue("Starting node information service..."))

	go NIS.NodeInformationService(&ObeliskRunInf)

	fmt.Println(Green("Started"))
}

func startNodeSenderService(){
	ObeliskRunInf.SetStatusSender(true)
	fmt.Println(BrightBlue("Starting node sender service..."))
}

func startNodeReceiverService(){
	ObeliskRunInf.SetStatusReceiver(true)
	fmt.Println(BrightBlue("Starting node receiver service..."))
}

func startNodePillarService(){
	ObeliskRunInf.SetStatusPillar(true)
	fmt.Println(BrightBlue("Starting pillar service..."))
}

func startNodeOracleService(){
	ObeliskRunInf.SetStatusOracle(true)
	fmt.Println(BrightBlue("Starting oracle service..."))
}

// Main loop serve

func serve(){
	fmt.Println(BrightBlue("Serving ObeliskPOC..."))

    var manLoop sync.WaitGroup
	
	manLoop.Add(1)
	
	signal.Notify(ObeliskKillChan, os.Interrupt)

	go func() {
		<-ObeliskKillChan
		
		fmt.Println(BrightBlue("\nShutting down ..."))
		ObeliskRunInf.DisableAllServices()

		time.Sleep(2 * time.Second)
        manLoop.Done()
    }()
	
	fmt.Println(Green("Press Ctrl+C to shutdown"))
	manLoop.Wait()
}

func checkConfig(){
	// Ensure the directory exists
	info, err := os.Stat("./.obelisk.d")
    if os.IsNotExist(err){
		fmt.Println("Obelisk directory not found.. Creating")
		os.Mkdir("./.obelisk.d", 0755)
	} else if (!info.IsDir()) {
		fmt.Println(Red("Error: Obelisk directory path appears to be a file"))
		os.Exit(1)
	}

	// Ensure the file exists
	info, err = os.Stat("./.obelisk.d/config")
    if os.IsNotExist(err){
		fmt.Println("Obelisk local config not found.. Creating")
		
		ObeliskConfig.NewConfig("./.obelisk.d/config", "./.obelisk.d/keys.d/local")

	} else if (info.IsDir()) {
		fmt.Println(Red("Error: Obelisk config file appears to be a directory"))
		os.Exit(1)
	}

	// Ensure keys dir exists
	info, err = os.Stat("./.obelisk.d/keys.d")
    if os.IsNotExist(err){
		fmt.Println("Obelisk keys directory not found.. Creating")
		os.Mkdir("./.obelisk.d/keys.d", 0644)
	} else if (!info.IsDir()) {
		fmt.Println(Red("Error: Obelisk key directory path appears to be a file"))
		os.Exit(1)
	}

	// Ensure local has a set of keys
	info, err = os.Stat("./.obelisk.d/keys.d/local_Private.key")
    if os.IsNotExist(err){
		fmt.Println("Obelisk local keys not found.. Creating")
		
		var keyManager Keyman.Keyman
		keyManager.GenerateKeys("./.obelisk.d/keys.d/local")

	} else if (info.IsDir()) {
		fmt.Println(Red("Error: Obelisk keys file appears to be a directory"))
		os.Exit(1)
	}
}

func confirm(message string) bool{
	var s string

	fmt.Printf("%s (y/N): ", message)
	_, err := fmt.Scan(&s)
	if err != nil {
		panic(err)
	}

	s = strings.TrimSpace(s)
	s = strings.ToLower(s)

	if s == "y" || s == "yes" {
		return true
	}
	return false
}

func scrubData(){
	if !confirm("Doing this will delete all keys and configuration. Are you sure? "){
		fmt.Println("Data will NOT be scrubbed.")
		return
	}

	fmt.Print(Yellow("Scrubbing data... "))

	os.RemoveAll("./.obelisk.d")

	fmt.Println(Green("Complete."))
}

// Entry

func main() {

	fmt.Println(BrightGreen("ObeliskPOC ["), Yellow("obelisk"), BrightGreen("]"))

	// Help
	helpPtr := flag.Bool("h", false, "Get some help")

	// Scrub
	scrubPtr := flag.Bool("scrub", false, "Remove local config (DANGER)")

	// Verbose
	verbPrt := flag.Bool("v", false, "Include some extra output")

	// Logging
	logPrt := flag.Bool("log", false, "Enable logging")

	// Run parameters
	nssPtr := flag.Bool("nss", false, "Node Sender Service")
	nrsPtr := flag.Bool("nrs", false, "Node Receiver Service")
	npsPtr := flag.Bool("nps", false, "Node Pillar Service")
	nosPtr := flag.Bool("nos", false, "Node Oracle Service")

	pubIpPtr := flag.String("pub", "127.0.0.1", "Public Address - Defaults to loopback")

	flag.Parse()

	if !*logPrt {
		log.SetOutput(ioutil.Discard)
	}

	if net.ParseIP(*pubIpPtr) == nil {
		fmt.Println(Red("Invalid public IP address given. Exiting"))
		os.Exit(1)
	}

	ObeliskRunInf.Setup(*pubIpPtr)
	
	if *helpPtr {
		flag.PrintDefaults()
		os.Exit(0)
	}

	if *scrubPtr {
		scrubData()
		os.Exit(0)
	}

	checkConfig()

	ObeliskConfig.Load("./.obelisk.d/config")

	if *verbPrt {
		fmt.Println(Yellow("----------------------------------------"))
		fmt.Print(Yellow("Services Ip : "))
		fmt.Println(ObeliskRunInf.GetPublicAddr())
		fmt.Print(Yellow("Host Id: "))
		fmt.Println(ObeliskConfig.Hostid)
		fmt.Print(Yellow("Key Location: "))
		fmt.Println(ObeliskConfig.KeyLoc)
		fmt.Print(Yellow("Remotes: "))
		if ObeliskConfig.Remotes != nil {
			fmt.Println(len(ObeliskConfig.Remotes))
		} else {
			fmt.Println("nil")
		}
		fmt.Println(Yellow("----------------------------------------"))
	}

	ObeliskKillChan = make(chan os.Signal, 1)
	
	startCommandService()

	startNodeInformationService()

	if *nssPtr {
		startNodeSenderService()
	}

	if *nrsPtr {
		startNodeReceiverService()
	}

	if *npsPtr {
		startNodePillarService()
	}

	if *nosPtr {
		startNodeOracleService()
	}

	serve()
}
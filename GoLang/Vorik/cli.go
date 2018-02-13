package main

import (
	"os"
	"fmt"
	"flag"
	"bufio"
	"strings"
	"github.com/rs/xid"
	vrpc "github.com/joshbosley/Vorik/rpc"
	vnode "github.com/joshbosley/Vorik/node"
	vkey "github.com/joshbosley/Vorik/keyman"
	scribble "github.com/nanobox-io/golang-scribble"
)

func main(){

	initialSetup := flag.Bool("init", false, "Initialize local server/client")
	accessControl := flag.String("netpass", "_", "The network password")
	netServer := flag.String("server", "127.0.0.1", "Primary network server")
	netPort := flag.String("port", "4096", "Primary network port")
	netName := flag.String("nname", "vorik-0", "A friendly name for the network")
	locName := flag.String("lname", "vorik-device", "A friendly name for the local node")
	launchNode := flag.Bool("launch", false, "Start local node")
	doDaemon := flag.Bool("daemon", false, "Start local process as daemon")
	flag.Parse()

	// Run the initial setup
	if *initialSetup{

		// Ensure we have the access control phrase
		if *accessControl == "_" {
			fmt.Println("ERROR: When performing initialization, a netpass is required.")
			os.Exit(1)
		}

		if *launchNode || *doDaemon {
			fmt.Println("Must initialize fully before issuing run or daemonize command.")
			os.Exit(1)
		}

		// Warn the user of potential data wipe
		ri := bufio.NewReader(os.Stdin)
		fmt.Print("Initializing will erase any existing settings. Continue? (Y/n): ")
		r, _ := ri.ReadString('\n')

		if "Y\n" != r {
			fmt.Println(" 'Y' not entered. Exiting!")
			os.Exit(0)
		}

		// Create dirs if needed
		if _, err := os.Stat("./data/"); os.IsNotExist(err) {
			err = os.MkdirAll("./data/", 0755)
			if err != nil {
					panic(err)
				}
		}
		
		// Write the information
		dir := "./data/"
		db, err := scribble.New(dir, nil)
		if err != nil {
			panic(err)
		}
		db.Write("local", "config", vrpc.LocalSettings{*accessControl, *netServer, *netPort, *netName, *locName})

		// Generate Keys
		*locName = strings.Replace(*locName, " ", "-", -1)

		// Create directory for keys if needed
		var k vkey.Keyman
		k.GenerateKeys(("./data/local/" + *locName))
		
		// New uid
		guid := xid.New()

		// Setup a registration structure with the information
		regArgs := vrpc.RegArgs{*locName, guid.String(), k.Public}

		// Write the id information
		db, err = scribble.New(dir, nil)
		if err != nil {
			panic(err)
		}
		db.Write("local", "registration", regArgs)

		// Initialization Complete
		fmt.Println("Done")
		os.Exit(0)
	}

	// Handle a silly user
	if *accessControl != "_"{
		fmt.Println("Netpass must be used in conjunction with -init. Use: -help for more info")
		os.Exit(1)
	}

	// Start a node instance
	if *launchNode {
		if _, err := os.Stat("./data/local/"); os.IsNotExist(err) {
			fmt.Println("Local node not setup yet. Requires -init. Check -h for help.")
			os.Exit(1)
		}

		if *doDaemon {
			fmt.Println("Daemon not yet created")
		} else {
			vnode.StartCli()
		}
		os.Exit(0)
	} 
}
package node

import (
	"os"
	"log"
	"time"
	vrpc "github.com/joshbosley/Vorik/rpc"
	vkey "github.com/joshbosley/Vorik/keyman"
	scribble "github.com/nanobox-io/golang-scribble"
)

type VNode struct {
	keys vkey.Keyman
	config vrpc.LocalSettings
	registration vrpc.RegArgs
	signalintercept chan int
	nodestate string
}

/*
	Load settings from data/local/
*/
func (v * VNode) loadNodeSettings() {

	// Setup scribbler
	dir := "./data/"
	db, err := scribble.New(dir, nil)
	if err != nil {
		panic(err)
	}

	// Load settings 
	if err := db.Read("local", "config", &v.config); err != nil {
		log.Println(err)
	}
	// Load registration
	if err := db.Read("local", "registration", &v.registration); err != nil {
		log.Println(err)
	}
	// Load keys
	v.keys.LoadKeys(("./data/local/"+v.registration.Friendly))

	// Setup a signal intercepter
	v.signalintercept = NodeSignalIntercept()
}

/*
	Start remote procedure calls
*/
func (v * VNode) initiateRpc(){

	// Start rpc serving
	vrpc.ListenerRpc(v.config.Port)
}

/*
	The primary action center of the node
*/
func (v * VNode) nodeCore(localInterface chan []string){

	for{
		select{
		case cig := <-v.signalintercept:
			log.Println("Got a signal!")

			// Kill for now
			os.Exit(cig)

		case lint := <-localInterface:
			log.Println("Got byte data from local interface")
			log.Println(lint)

		default:
			// Say 'NO' to grinding
			time.Sleep(5 * time.Millisecond)
		}
	}
}

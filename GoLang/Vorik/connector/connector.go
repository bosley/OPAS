
package connector

import (
	"os"
	"log"
	"strings"
    "net/rpc/jsonrpc"
	"github.com/rs/xid"
	vrpc "github.com/joshbosley/Vorik/rpc"
	vkey "github.com/joshbosley/Vorik/keyman"
)

/*
	Used to setup new device registration with a server
	(keyLoc, friendly, servloc)
	("/path/to/keys/", ip:port
*/
func GenerateAndPerformFirstRegistration(keyLoc, friendly, servloc string) bool {

	/*
	// Ensure name doesn't contain spaces
	friendly = strings.Replace(friendly, " ", "-", -1)

	// Create directory for keys if needed
	if _, err := os.Stat(keyLoc); os.IsNotExist(err) {
		err = os.MkdirAll(keyLoc, 0755)
		if err != nil {
				log.Println(err)
				return false
			}
	}

	// Set the key location based on path and friendly name
	charCheck := keyLoc[len(keyLoc)-1:]
	if charCheck != "/" {
		keyLoc = keyLoc + "/" + friendly
	} else {
		keyLoc = keyLoc + friendly
	}

	// Make a new set of keys, and a uid
	var k vkey.Keyman
	k.GenerateKeys(keyLoc)
	guid := xid.New()

	// Setup a registration structure with the information
	regArgs := vrpc.RegArgs{friendly, guid.String(), k.Public}


*/



	// Register data
	client, err := jsonrpc.Dial("tcp", servloc)

	if err != nil {
		log.Println("Unable to contact server")
		return false
	}

	var reply int
	err = client.Call("DoRegister.NewRegister", regArgs, &reply)

	if err != nil {
		log.Println(err)
		return false
	}
	return true
}

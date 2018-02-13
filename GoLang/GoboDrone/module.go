package drone

type module struct {

	// Device interaction
	local * device
	
	// Chan for triggering device interaction 
	callchan chan Command

	// Chan for responding 
	retchan chan Response

	// Port to listen on
	port string
}

var mod module

func Serve(d * device, port string) {
	mod.port = port
	mod.local = d
	mod.callchan = make(chan Command)
	mod.retchan = make(chan Response)

	// Start the device goroutine 
	go d.serve()

	// Start the RPC server
	rpcServer(port)
}


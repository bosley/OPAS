package drone

/*
	Input commands coming from the network
	will be setup in a Command struct and
	sent to device.interact for processing
*/
type Command struct {
	Instruct string
	Params []string
}

/*
	Response from a command
*/
type Response struct {
	Code int
	Data []byte
}

// Device Abstraction
type device struct {
	interact func(c * Command, r chan Response)
}

/*
	Interaction function routed via callback 
	for device-specific action
*/
func (d * device) Interact(callback func(c * Command, r chan Response)){
	d.interact = callback
}

/*
	Launched as goroutine, serve will take in Command
	structure from cmdInterface channel and route to 
	interact

*/
func (d * device) serve() {
	for {
		select{
		case cmd := <-mod.callchan:
			// Have interact return a response
			// Place response in output chan
			go d.interact(&cmd, mod.retchan)

		default:
			continue
		}
	}
}

/*
	Create a new device 
*/
func New() * device {
	dev := &device {}
	dev.Interact(func(c * Command, r chan Response) {})
	return dev
}
package vorikrpc

type DoRegister int

func (r * DoRegister) NewRegister(args * RegArgs, reply *int) error {
	// Register id, pubkey, and sig

	*reply = 0

	return nil
}

func (r * DoRegister) UpdateRegistration(args * RegArgs, reply * int) error {
	// Update registration

	*reply = 0

	return nil
}

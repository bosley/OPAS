package vorikrpc

// Local system settings
type LocalSettings struct {
	AccessControl, Server, Port, NetName, LocalName string
}

// Arguments for registration process
type RegArgs struct {
	Friendly, Id string
	Pubkey []byte
}



package server

import (
	"fmt"
	"net"
	"log"
    "net/rpc"
	"net/rpc/jsonrpc"
	
	vrpc "github.com/joshbosley/Vorik/rpc"
)

func checkError(err error){
	if err != nil {
		panic(err)
	}
}

func Start(port string) {
	vreg := new(vrpc.DoRegister) 

	rpc.Register(vreg)

	tcpAdd, err := net.ResolveTCPAddr("tcp", (":" + port))
	checkError(err)

	heylisten, err := net.ListenTCP("tcp", tcpAdd)
	checkError(err)

	log.Println("Serving on port : " + port)
	
	// Serve the RPC server
	go func(){
		for {
			conn, err := heylisten.Accept()
			if err != nil{
				continue
			}
			jsonrpc.ServeConn(conn)
		}
	}()

	fmt.Println("Use ^C to exit")
	for{}
}

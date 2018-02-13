package drone

import (
	"net"
	"log"
    "net/rpc"
	"net/rpc/jsonrpc"
)

type DroneCall Response

func (d * DroneCall) Request(cmd * Command, reply * Response) error {

	// Send the command over the channel
	c := Command{cmd.Instruct, cmd.Params}
	mod.callchan <- c

	// Set a reply from the return channel
	*reply = <-mod.retchan
	return nil
}

func checkError(err error){
	if err != nil {
		panic(err)
	}
}

func rpcServer(port string){

	caller := new(DroneCall)

	rpc.Register(caller)
	tcpAdd, err := net.ResolveTCPAddr("tcp", (":" + port))
	checkError(err)

	heylisten, err := net.ListenTCP("tcp", tcpAdd)
	checkError(err)

	log.Println("Serving on port : " + port)
	
	go func(){
		for {
			conn, err := heylisten.Accept()
			if err != nil{
				continue
			}
			jsonrpc.ServeConn(conn)
		}
	}()
}

func SendRequest(serverLocation string, request Command) Response {

	// Register data
	client, err := jsonrpc.Dial("tcp", serverLocation)

	if err != nil {
		log.Println("Unable to contact server")
		return Response{0, []byte{}}
	}

	var reply Response
	err = client.Call("DroneCall.Request", request, &reply)

	if err != nil {
		log.Println(err)
		return Response{0, []byte{}}
	}

	client.Close()
	return reply
}

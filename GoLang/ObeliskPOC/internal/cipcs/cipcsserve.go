package cipcs

import (
	"log"
	"net"
	"net/rpc"
	"net/rpc/jsonrpc"
	"os"
	RunInf "ObeliskPOC/internal/runinf"
)

var CoreHandlerObject CipsHandler

type Server struct {
	ConnInfo ConnectInfo
	jsonListener net.Listener
}

func (s *Server) Close() (err error) {
	if s.jsonListener != nil {
		err = s.jsonListener.Close()
	}
	return
}

func (s *Server) Start(obeliskRunInfo *RunInf.RunInf, killChan chan os.Signal) (err error) {
	
	var conn net.Conn

	CoreHandlerObject.ObeliskRunInfo  = obeliskRunInfo
	CoreHandlerObject.ObelisKKillChan = killChan

	rpc.Register(&CoreHandlerObject)
	
	s.jsonListener, err = net.Listen("tcp", s.ConnInfo.JspnRpc)
	if err != nil {
		log.Println(err)
		return err
	}

	for {
		conn, err = s.jsonListener.Accept()

		if err != nil {
			log.Println(err)
			return
		}

		jsonrpc.ServeConn(conn)
	}
}
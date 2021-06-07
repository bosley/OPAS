package nis


import (
	"fmt"
	"log"
	"net"
	"net/http"
	"net/rpc"
	"net/rpc/jsonrpc"
	RunInf "ObeliskPOC/internal/runinf"
)

var NISCoreHandlerObject NISCoreHandler

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

func (s *Server) startJsonRpc() (err error) {

	var conn net.Conn

	for {
		conn, err = s.jsonListener.Accept()

		if err != nil {
			log.Println(err)
			return
		}

		jsonrpc.ServeConn(conn)
	}
}

func handleHttp(w http.ResponseWriter, r *http.Request) {
	
	var res Response
	var req Request

	NISCoreHandlerObject.Handle(req, &res)

	if res.Ok {
		fmt.Fprintf(w, "%s", res.Data)
	}
}

func (s *Server) Start(obeliskRunInfo *RunInf.RunInf) (err error) {

	NISCoreHandlerObject.ObeliskRunInfo = obeliskRunInfo

	rpc.Register(&NISCoreHandlerObject)

	s.jsonListener, err = net.Listen("tcp", s.ConnInfo.JspnRpc)
	if err != nil {
		log.Println(err)
		return err
	}
	go s.startJsonRpc()

	http.HandleFunc("/", handleHttp)
	http.ListenAndServe(s.ConnInfo.Http, nil)

	return nil
}
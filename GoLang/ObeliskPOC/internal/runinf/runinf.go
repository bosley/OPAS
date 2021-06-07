package runinf

import (
	"log"
	. "github.com/logrusorgru/aurora"
)

const (
	LOCAL_ADDR    = "127.0.0.1"
	CIPCS_PORT    = 18200
	NIS_PORT      = 18201
	NIS_HTTP_PORT = 18206
	NSS_PORT      = 18202
	NRS_PORT      = 18203
	NPS_PORT      = 18204
	NOS_PORT      = 18205
)

type RunInf struct {
	obServices Services
	publicAddr string
}

type Services struct {
	Command bool
	Information bool
	Sender bool 
	Receiver bool
	Oracle bool
	Pillar bool
}

func(r * RunInf) Setup(publicAddr string){
	r.publicAddr = publicAddr
	r.DisableAllServices()
}

func(r * RunInf) DisableAllServices(){
	r.SetStatusCommand(false)
	r.SetStatusInformation(false)
	r.SetStatusSender(false)
	r.SetStatusReceiver(false)
	r.SetStatusOracle(false)
	r.SetStatusPillar(false)
}

func(r * RunInf) SetStatusCommand(state bool) {
	log.Println("RunInf::SetStatusCommand(", Yellow(state), ")")
	r.obServices.Command = state
}

func(r * RunInf) SetStatusInformation(state bool) {
	log.Println("RunInf::SetStatusInformation(", Yellow(state), ")")
	r.obServices.Information = state
}

func(r * RunInf) SetStatusSender(state bool) {
	log.Println("RunInf::SetStatusSender(", Yellow(state), ")")
	r.obServices.Sender = state
}

func(r * RunInf) SetStatusReceiver(state bool) {
	log.Println("RunInf::SetStatusReceiver(", Yellow(state), ")")
	r.obServices.Receiver = state
}

func(r * RunInf) SetStatusOracle(state bool) {
	log.Println("RunInf::SetStatusOracle(", Yellow(state), ")")
	r.obServices.Oracle = state
}

func(r * RunInf) SetStatusPillar(state bool) {
	log.Println("RunInf::SetStatusPillar(", Yellow(state), ")")
	r.obServices.Pillar = state
}

func(r * RunInf) GetStatusCommand() bool {
	return r.obServices.Command
}

func(r RunInf) GetStatusInformation() bool {
	return r.obServices.Information
}

func(r RunInf) GetStatusSender() bool {
	return r.obServices.Sender
}

func(r RunInf) GetStatusReceiver() bool {
	return r.obServices.Receiver
}

func(r RunInf) GetStatusOracle() bool {
	return r.obServices.Oracle
}

func(r RunInf) GetStatusPillar() bool {
	return r.obServices.Pillar
}

func(r RunInf) GetPublicAddr() string {
	return r.publicAddr
}
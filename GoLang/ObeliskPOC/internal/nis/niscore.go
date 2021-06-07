package nis

import (
	"encoding/json"
	RunInf "ObeliskPOC/internal/runinf"
)

type ConnectInfo struct {
	Http	string
	JspnRpc	string
}

type Response struct {
	Data    string
	Ok      bool
}

type Request struct {
	Data string
}

type NISCoreHandler struct {
	ObeliskRunInfo *RunInf.RunInf
}

type NisData struct {
	Nss bool `json:"nss"`
	Nrs bool `json:"nrs"`
	Nos bool `json:"nos"`
	Nps bool `json:"nps"`
}

func (n NisData) NisDataToString() (string, error){
	b, err := json.Marshal(&n)
    if err != nil {
        return "", err
	}
	data := string(b)
	return data, nil
}

func (n *NisData) NisDataFromString(data string) (error){
	
	return json.Unmarshal([]byte(data), &n)
}

func (c *NISCoreHandler) Handle(req Request, res *Response) (error){

	var ns NisData
	ns.Nss = c.ObeliskRunInfo.GetStatusSender()
	ns.Nrs = c.ObeliskRunInfo.GetStatusReceiver()
	ns.Nos = c.ObeliskRunInfo.GetStatusOracle()
	ns.Nps = c.ObeliskRunInfo.GetStatusPillar()

	var err error
	res.Data, err = ns.NisDataToString()

	if err != nil {
		res.Ok = false
	} else {
		res.Ok = true
	}
	return nil
}
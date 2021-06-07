package nis

import (
	"strconv"
	RunInf "ObeliskPOC/internal/runinf"
)

//
//	Start the Node Information Service
//
func NodeInformationService(obeliskRunInfo *RunInf.RunInf) {

	var NISServer Server
	NISServer.ConnInfo.JspnRpc = obeliskRunInfo.GetPublicAddr() + ":" + strconv.Itoa(RunInf.NIS_PORT)
	NISServer.ConnInfo.Http    = obeliskRunInfo.GetPublicAddr() + ":" + strconv.Itoa(RunInf.NIS_HTTP_PORT)
	NISServer.Start(obeliskRunInfo)
	return
}

package cipcs

import (
	"os"
	"strconv"
	RunInf "ObeliskPOC/internal/runinf"
)

//
//	Start the Commander IPC service
//
func CommanderIPCService(obeliskRunInfo *RunInf.RunInf, killChan chan os.Signal) {
	
	var CIPCSServer Server
	CIPCSServer.ConnInfo.JspnRpc = obeliskRunInfo.GetPublicAddr() + ":" + strconv.Itoa(RunInf.CIPCS_PORT)
	CIPCSServer.Start(obeliskRunInfo, killChan)
	return
}
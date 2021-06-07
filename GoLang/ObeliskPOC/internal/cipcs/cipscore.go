package cipcs

import (
	"os"
	RunInf "ObeliskPOC/internal/runinf"
)

type ConnectInfo struct {
	JspnRpc	string
}

type Response struct {
	Data string
	Ok   bool
}

type Request struct {
	Command string
}

const (
	CommandShutdown = "CipsHandler.Shutdown"
)

type CipsHandler struct {
	ObelisKKillChan chan os.Signal
	ObeliskRunInfo *RunInf.RunInf
}

func (c *CipsHandler) Shutdown(req Request, res *Response) (error) {

	c.ObelisKKillChan <- os.Interrupt

	res.Data = "complete"
	res.Ok   = true
	return nil
}

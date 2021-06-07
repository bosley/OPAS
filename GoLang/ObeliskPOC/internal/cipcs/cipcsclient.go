package cipcs

import (
	"errors"
	"log"
	"net/rpc"
	"net/rpc/jsonrpc"
)

type Client struct {
	client  *rpc.Client
	ok      bool
}

func (c *Client) Init(connectionInfo string) (error) {

	var err error
	c.ok = false
	c.client, err = jsonrpc.Dial("tcp", connectionInfo)

	if err != nil {
		log.Println(err)
		return err
	}
	c.ok = true
	return nil
}

func (c *Client) Close() (error) {
	if c.client != nil {
		return c.client.Close()
	}

	return nil
}

//
//	Command param should be from const command in cipscore
//
func (c *Client) sendCommand(command string) (bool, string, error) {

	if !c.ok {
		return false, "", errors.New("Client !ok . Perhaps not connected.")
	}

	var request  Request
	var response Response

	request.Command = command

	err := c.client.Call(command, &request, &response)

	if err != nil {
		log.Println(err)
		return false, "", err
	}

	return response.Ok, response.Data, nil
}

func (c *Client) ShutdownObelisk() (error) {

	if !c.ok {
		return errors.New("Client !ok . Perhaps not connected.")
	}

	ok, resp, err := c.sendCommand(CommandShutdown)

	if err != nil {
		log.Println("Unable to send shutdown command")
		return err
	}

	if !ok {
		log.Println("Shutdown sent, but not completed. Reason : ", resp)
		return errors.New("Shutdown sent, but not completed")
	}

	return nil
}
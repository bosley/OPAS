package nis

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

func (c *Client) Execute() (NisData, error) {

	var ns NisData
	
	if !c.ok {
		return ns, errors.New("Client !ok . Perhaps an error connecting?")
	}

	var request  Request
	var response Response

	err := c.client.Call("NISCoreHandler.Handle", &request, &response)
	if err != nil {
		log.Println(err)
		return ns, err
	}

	ns.NisDataFromString(response.Data)

	return ns, nil
}
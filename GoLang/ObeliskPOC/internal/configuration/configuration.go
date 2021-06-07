package configuration

import (
    "crypto/rand"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"os"
	"time"
)

type LocalConfig struct {
	Hostid string `json:"hostid"`
	KeyLoc string `json:"keyloc"`
	Remotes []Remote `json:"pillars"`
}

type Remote struct {
	Id string `json:"id"`
	Ip string `json:"ip"`
	LastSeen time.Time `json:"lastSeen"`
	Functions Functionality `json:"functions"`
}

type Functionality struct {
	Sender bool `json:"sender"`
	Receiver bool `json:"receiver"`
	Oracle bool `json:"oracle"`
	Pillar bool `json:"pillar"`
}

func(l * LocalConfig) NewConfig(filePath string, keyloc string){

	// Generate struct 

	// - Make Host id
	b := make([]byte, 8)
    _, err := rand.Read(b)
    if err != nil {
        fmt.Println("Error generating id: ", err)
        os.Exit(1)
    }

	l.Hostid = fmt.Sprintf("%X-%X-%X-%X", b[0:2], b[2:4], b[4:6], b[6:8])
	l.KeyLoc = keyloc
	l.Remotes = nil

	// Write struct to file
	file, _ := json.MarshalIndent(&l, "", " ")
	_ = ioutil.WriteFile(filePath, file, 0644)
}

func(l * LocalConfig) Load(filePath string){

	file, _ := ioutil.ReadFile(filePath)

	json.Unmarshal([]byte(file), &l)
}
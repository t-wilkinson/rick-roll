package main

import (
	// "github.com/t-wilkinson/rick-roll-detector/rickroll"
	"github.com/t-wilkinson/rick-roll-detector/webserver"
)

func main() {
	rpcAddress := ":8081"
	webServerAddress := ":8080"
	// rickroll.Start(rpcAddress)
	server.Start(webServerAddress, rpcAddress)
}

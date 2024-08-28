package rickroll

import (
	"errors"
	"io"
	// "log"
	// "net"
	"net/http"
	// "net/rpc"
	"regexp"
)

type SafetyRating byte

const (
	Empty   SafetyRating = '\x00'
	Unknown              = '0'
	Danger               = '1'
	Warning              = '2'
	Safe                 = '3'
)

func isRickRoll(body []byte) SafetyRating {
	re := regexp.MustCompile("(?i)rick roll|never gonna give you up|rick astley")
	if re.Match(body) {
		return Danger
	} else {
		return Safe
	}
}

func SearchURL(query string) SafetyRating {
	resp, err := http.Get(query)
	if err != nil {
		return Empty
	}
	defer resp.Body.Close()

	body, err := io.ReadAll(resp.Body)
	return isRickRoll(body)
}

func (t *SafetyRating) SearchURL(query *string, reply *SafetyRating) error {
	resp, err := http.Get(*query)
	if err != nil {
		*reply = Unknown
		return errors.New("Query must be a valid URL")
	}
	defer resp.Body.Close()

	body, err := io.ReadAll(resp.Body)
	*reply = isRickRoll(body)
	return nil
}

// func Start(address string) {
// 	safetyRating := new(SafetyRating)
// 	rpc.Register(safetyRating)
// 	rpc.HandleHTTP()
// 	l, e := net.Listen("tcp", address)
// 	if e != nil {
// 		log.Fatal("listen error:", e)
// 	}
// 	log.Println(l.Addr().String())
// 	go rpc.Accept(l)
// 	go http.Serve(l, nil)
// }

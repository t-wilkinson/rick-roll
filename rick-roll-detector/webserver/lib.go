package server

import (
	"fmt"
    "os"
	"github.com/t-wilkinson/rick-roll-detector/rickroll"
	"log"
	"net"
	"net/http"
	// "net/rpc"
)

type Client struct {
	// client *rpc.Client
}

func (c *Client) homeHandler(w http.ResponseWriter, r *http.Request) {
	http.ServeFile(w, r, "index.html")
}

func (c *Client) searchHandler(w http.ResponseWriter, r *http.Request) {
	r.ParseForm()
	userQuery := r.Form.Get("search")
	var rating rickroll.SafetyRating = rickroll.Empty

	// send request to new database connection and return the response
	sendDb := func(req string) rickroll.SafetyRating {
		var rating rickroll.SafetyRating = rickroll.Empty

		// connect to database
        portno := os.Getenv("DATABASE_PORT")
        if portno == "" {
            portno = "8082"
        }

        db, err := net.Dial("tcp", "localhost:" + portno)
		if err != nil {
			fmt.Println("can't connect to db")
			return rickroll.Empty
		}
		defer db.Close()

		// write request
		_, err = db.Write([]byte(req))
		if err != nil {
			return rickroll.Empty
		}

		// get response
		buf := make([]byte, 512)
		db.Read(buf[:])
		rating = rickroll.SafetyRating(buf[0])

		return rating
	}

	// check if response is saved in database
	rating = sendDb(userQuery)

	// return rating if it is not empty
	if rating != rickroll.Empty {
		fmt.Fprintln(w, rating)
		return
	}

	// get the rating from rickroll service
	rating = rickroll.SearchURL(userQuery)
	// err := c.client.Call("SafetyRating.SearchURL", userQuery, &rating)
	// if err != nil {
	// 	fmt.Println("safetyRating call error")
	// 	http.Error(w, err.Error(), http.StatusBadRequest)
	// 	return
	// }

	// save the rating to the database
	go sendDb(fmt.Sprintf("%s %c", userQuery, rating))

	fmt.Fprintln(w, rating)
	return
}

func Start(webServerAddress string, rpcAddress string) {
	// client, err := rpc.DialHTTP("tcp", rpcAddress)
	// if err != nil {
	// 	log.Fatal("dialing:", err)
	// }

	c := &Client{}

	http.HandleFunc("/", c.homeHandler)
	http.HandleFunc("/search", c.searchHandler)

	fmt.Printf("Web server running on port %s\n", webServerAddress)
	log.Fatal(http.ListenAndServe(webServerAddress, nil))
}

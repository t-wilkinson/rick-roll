#ifndef _server_h_
#define _server_h_

#include "db.h"
#include <netinet/in.h>
#define URL_LENGTH 512

typedef struct Server {
    int port;
    int sockfd;
    struct sockaddr_in servaddr;
    db_t *db;
} server_t;

typedef struct Request {
    char url[URL_LENGTH];
    safety_rating_t safety_rating;
} request_t;

typedef struct Response {
    safety_rating_t safety_rating;
} response_t;


server_t *create_server(int);
int connect_server(server_t *);

/*
 * server runs in a loop which accepts requests, parses them,
 *  creates a thread to handle the request, prepares the response,
 *  then send the reply.
 */
int run_server(server_t *);

int accept_connection(server_t *);
int close_server(server_t *);
request_t *to_request(int);
char *from_response(response_t *);

/*
 * takes a request, matches request with sub function, then returns a response
 */
response_t *handle_request(request_t *);

#endif

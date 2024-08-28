#include "db.h"
#include "server.h"
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define SA struct sockaddr

server_t *create_server(int portno) {
    int sockfd, connfd, len;
    struct sockaddr_in servaddr;

    server_t *serv = malloc(sizeof(server_t));
    serv->port = portno;

    /* socket create and verification */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));

    /* assign IP address and port */
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(serv->port);

    /* populate serv with relevant values */
    serv->servaddr = servaddr;
    serv->sockfd = sockfd;
    return serv;
}

int connect_server(server_t *serv) {
    /* binding newly created socket to given IP and verification */
    int one = 1;
    setsockopt(serv->sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    if ((bind(serv->sockfd,
                    (struct sockaddr*)&(serv->servaddr),
                    sizeof(serv->servaddr))) != 0) {
        printf("Socket bind failed...\n");
        exit(0);
    }

    /* now server is ready to listen and verification */
    if ((listen(serv->sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }

    printf("Server running on port %i...\n", serv->port);
}

int accept_connection(server_t *serv) {
    struct sockaddr_in cli;
    int connfd, len;
    len = sizeof(cli);

    /* accept the data packet from client and verification */
    connfd = accept(serv->sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        exit(0);
    }

    return connfd;
}

request_t *to_request(int connfd) {
    request_t *req = malloc(sizeof(request_t));
    req->safety_rating = Empty;

    /* read the message from client and copy it in buffer */
    char buff[URL_LENGTH];
    bzero(buff, URL_LENGTH);
    read(connfd, buff, sizeof(buff));

    /*
     * create request by first finding the url
     * also add safety_rating if it is found after the url
     */
    for (int i = 0; i < URL_LENGTH; i++) {
        /* if request ends exit loop */
        if (buff[i] == '\0' || buff[i] == '\n') {
            break;
        }

        /* if request has a space, interpret the next byte as the safety_rating and break */
        if (buff[i] == ' ') {
            if (i + 1 < URL_LENGTH) {
                req->safety_rating = (safety_rating_t) buff[i+1];
            }
            break;
        }

        req->url[i] = buff[i];
    }

    return req;
}

response_t *func(server_t *serv, request_t *req) {
    response_t *res = malloc(sizeof(response_t));
    db_item_t item;

    switch(req->safety_rating) {
        case Empty:
            item = get_item(serv->db, req->url);
            res->safety_rating = item.safety_rating;
            break;
        default:
            item.safety_rating = req->safety_rating;
            set_item(serv->db, req->url, &item);
            save_db(serv->db);
            res->safety_rating = Empty;
            break;
    }

    return res;
}

char *from_response(response_t *res) {
    char *buff = malloc(sizeof(char) * URL_LENGTH);
    bzero(buff, URL_LENGTH);

    buff[0] = res->safety_rating;
    return buff;
}

int close_server(server_t *serv) {
    close(serv->sockfd);
    free(serv->db);
    free(serv);
}

int run_server(server_t *serv) {
    connect_server(serv);

    while (1) {
        int connfd = accept_connection(serv);
        request_t *req = to_request(connfd);
        response_t *res = func(serv, req);
        char *buff = from_response(res);
        write(connfd, buff, sizeof(char) * URL_LENGTH);

        /* cleanup */
        free(req);
        free(res);
        free(buff);
        close(connfd);
    }

    close_server(serv);
}

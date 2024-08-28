# syntax=docker/dockerfile:1

FROM golang:buster AS build

WORKDIR /app

COPY go.mod ./
RUN go mod download

ADD db ./db
ADD rickroll ./rickroll
ADD webserver ./webserver

COPY main.go ./

RUN go build -o /app/server
RUN make -C db compile

FROM ubuntu:latest

WORKDIR /
ADD scripts/start-docker.sh ./
ADD index.html ./
COPY --from=build /app/server ./server
COPY --from=build /app/db/database ./database

CMD [ "/bin/bash", "./start-docker.sh" ]

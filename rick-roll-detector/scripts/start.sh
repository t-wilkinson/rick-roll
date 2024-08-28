#!/usr/bin/env sh

# kill child processes with one SIGINT
trap 'trap " " TERM; kill 0; wait' INT TERM

go build -o ./rick-roll-server
./rick-roll-server &

make -C db compile
cd db && ./database &

wait

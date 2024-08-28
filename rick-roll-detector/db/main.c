#include "db.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

void *save_db_loop(void *vargp) {
    db_t *db = (db_t *)vargp;
    while (1) {
        sleep(60000);
        save_db(db);
    }
    return 0;
}

int main() {
    db_t *db = init_db("backup.db");
    char *portno_env = getenv("DATABASE_PORT");
    int portno = 8082;

    if (portno_env) {
        portno = atoi(portno_env);
    }

    server_t *serv = create_server(portno);
    serv->db = db;

    /* run thread that periodically writes database to file */
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, save_db_loop, (void *)db);

    run_server(serv);

    pthread_cancel(thread_id);
    pthread_join(thread_id, NULL);

    return 0;
}

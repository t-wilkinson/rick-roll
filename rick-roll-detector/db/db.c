#include "db.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

db_t *init_db(char *filename) {
    db_t *db = load_db(filename);
    if (db == NULL) {
        printf("Could not find database file, creating one.\n");
        db = create_db(filename);
    }
    return db;
}

db_t *create_db(char *filename) {
    db_t *db = malloc(sizeof(db_t));
    db_item_t *item = malloc(sizeof(db_item_t));
    if (db) {
        db->number_of_elements = 0;
        for (int i = 0; i < MAXELEMENTS; i++) {
            db->elements[i] = *item;
        }

        /* copy filename into db */
        db->filename = malloc(sizeof(filename));
        strcpy(db->filename, filename);

        save_db(db);

        return db;
    } else {
        printf("Unable to create database.\n");
        return NULL;
    }
}

db_t *load_db(char *filename) {
    FILE *infile;
    db_t *db;

    infile = fopen(filename, "rb");
    if (infile == NULL) {
        return NULL;
    }

    /* load db into struct */
    db = malloc(sizeof(db_t));
    fread(db, sizeof(db_t), 1, infile);
    fclose(infile);

    /* overwrite filename */
    db->filename = malloc(sizeof(filename));
    strcpy(db->filename, filename);

    return db;
}

int save_db(db_t *db) {
    FILE *outfile;

    outfile = fopen(db->filename, "wb");
    if (outfile == NULL) {
        fprintf(stderr, "Error writing file.\n");
        return 1;
    }
    printf("Saving db to %s.\n", db->filename);

    /* write database to file */
    fwrite(db, sizeof(db_t), 1, outfile);
    fclose(outfile);

    return 0;
}

/**
 *  sdbm hash algorithm
 *  stole this from the internet
 */
int key_hash(char key[]) {
    unsigned int hash_code = 0;

    /* iterates over string at each character */
    for (int counter = 0; key[counter] != '\0'; counter++) {
        /* actual computing of the hash code */
        hash_code =
            key[counter] + (hash_code << 6) + (hash_code << 16) - hash_code;
    }

    /* fit index in array */
    return hash_code % MAXELEMENTS;
}

int set_item(db_t *db, char key[], db_item_t *item) {
    int index = key_hash(key);

    if (db->elements[index].safety_rating == Empty) {
        db->number_of_elements++;
    }
    db->elements[index] = *item;
    return 0;
}

db_item_t get_item(db_t *db, char key[]) {
    int index = key_hash(key);

    /* fetch item from database returning null if it wasn't found */
    db_item_t item = db->elements[index];
    return item;
}

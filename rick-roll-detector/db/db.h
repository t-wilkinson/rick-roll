#ifndef _db_h_
#define _db_h_

#define MAXELEMENTS 10
#define FILENAME_LENGTH 100

typedef enum SafetyRating { Empty='\0', Unknown='0', Danger='1', Warning='2', Safe='3' } safety_rating_t;

typedef struct DatabaseItem {
    safety_rating_t safety_rating;
} db_item_t;

typedef struct Database {
    int number_of_elements;
    db_item_t elements[MAXELEMENTS];
    char *filename;
} db_t;

/** load database from file if exists, otherwise call create_db */
db_t *init_db(char *);

/** creates new database from scratch and saves it to filesystem */
db_t *create_db(char *);

/** load database from previously saved database file */
db_t *load_db(char *);

/** save the database to disk to load it up accross sessions */
int save_db(db_t *);

int key_hash(char []);
int set_item(db_t *, char [], db_item_t *);
db_item_t get_item(db_t *, char []);

#endif

CC = gcc

# The CFLAGS variable sets compile flags for gcc:
#  -g        compile with debug information
#  -Wall     give verbose compiler warnings
#  -O0       do not optimize generated code
CFLAGS = -g -O0
SOURCES = db.c server.c
OBJECTS = $(SOURCES:.c=.o)

compile:
	$(CC) $(CFLAGS) $(SOURCES) main.c -o database -lpthread

run: compile
	./database

test:
	cd db; $(CC) $(CFLAGS) $(SOURCES) test.c -o test
	./test

.PHONY: clean

clean:
	rm db/database db/test db/database.db

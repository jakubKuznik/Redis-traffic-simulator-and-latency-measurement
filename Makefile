# gcc redis-generate-values.c -lhiredis -o out
# gcc redis-write-keys.c -lhiredis -o out
CC = gcc
LD = gcc

CFLAGS = -g -std=gnu99 -Wall -Wextra -lpthread -lhiredis  
#CFLAGS = -g -std=c99 -pedantic -Wall -Wextra -Werror -lpthread   

all: redis-generate-values redis-write-keys
FILES = *

prog: redis-generate-values
	$(CC) $(CFLAGS) $< -o $@
prog: redis-write-keys 
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm *.o redis-generate-values redis-write-keys


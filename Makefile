# gcc redis-generate-values.c -lhiredis -o out
CC = gcc
LD = gcc

CFLAGS = -g -std=gnu99 -Wall -Wextra -lpthread -lhiredis  
#CFLAGS = -g -std=c99 -pedantic -Wall -Wextra -Werror -lpthread   

all: redis-generate-values
FILES = *

prog: redis-generate-values
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm *.o redis-generate-values


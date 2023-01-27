CC = g++
LD = g++

CPPFLAGS = -g -std=c++11 -lhiredis -lpthread -Wall -Wextra -pedantic

all: redis-latency 
FILES = *

prog: redis-latency
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o redis-latency


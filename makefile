CC=gcc
FLAGS=-Wall -g -pthread -o

all: st_reactor.so react_server

st_reactor.so: reactor.c
	$(CC) -shared -fPIC -o st_reactor.so reactor.c

react_server: st_reactor.so server.c headers.h hashmap.h
	$(CC) $(FLAGS) react_server server.c hashmap.c ./st_reactor.so
clean:
	rm react_server *.so
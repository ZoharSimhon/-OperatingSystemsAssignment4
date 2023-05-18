CC=gcc
FLAGS=-Wall -g -pthread -o

all: libreactor.so server

libreactor.so: reactor.c
	$(CC) -shared -fPIC -o libreactor.so reactor.c

server: libreactor.so server.c headers.h hashmap.h
	$(CC) $(FLAGS) server server.c hashmap.c ./libreactor.so
clean:
	rm server *.so
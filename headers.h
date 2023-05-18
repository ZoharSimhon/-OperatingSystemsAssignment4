#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/time.h>
#include <time.h>
#include <openssl/md5.h>
#include <sys/stat.h>

#define PORT 4000
#define MESSAGE_LENGTH 1025

// definition for handler_t
typedef int (*handler_t)(int);

// server
int createServerSocket();

// reactor
void *createReactor();
void stopReactor(void *thisReactor);
void startReactor(void *thisReactor);
void addFd(void *thisReactor, int fd, handler_t handler);
void waitFor(void *thisReactor);

typedef struct _reactor
{
    struct pollfd *pfds;
    int fd_count;
    int fd_size;
    // hashMap
    int keepListening;
} reactor,* preactor;

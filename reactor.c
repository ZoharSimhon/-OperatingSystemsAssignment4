#include "headers.h"

void *createReactor()
{
    preactor newReactor = (preactor)malloc(sizeof(reactor));
    if (!newReactor)
    {
        perror("malloc newReactor");
        return NULL;
    }
    newReactor->keepListening = 1;
    newReactor->fd_count = 0;
    newReactor->fd_size = 4;
    newReactor->pfds = malloc(sizeof(struct pollfd) * 4);
    if (!newReactor->pfds)
    {
        perror("malloc pfds");
        free(newReactor);
        return NULL;
    }
    // hashmap

    return newReactor;
}
void stopReactor(void *thisReactor)
{
    preactor pReactor = (preactor)thisReactor;
    pReactor->keepListening = 0;
}

void startReactor(void *thisReactor)
{
    preactor pReactor = (preactor)thisReactor;
    pReactor->keepListening = 1;
}

void addFd(void *thisReactor, int fd, handler_t handler)
{
    preactor pReactor = (preactor)thisReactor;
    // add fd to pollfd
    // If we don't have room, add more space in the pfds array
    if (pReactor->fd_count == pReactor->fd_size)
    {
        pReactor->fd_size *= 2; // Double it

        pReactor->pfds = realloc(pReactor->pfds, sizeof(struct pollfd) * (pReactor->fd_size));

        if (!pReactor->pfds)
        {
            perror("realloc pfds");
            free(pReactor);
            return NULL;
        }
    }
    pReactor->pfds[pReactor->fd_count].fd = fd;
    pReactor->pfds[pReactor->fd_count].events = POLLIN;
    pReactor->fd_count++;

    // add fd to hashmap
}

void waitFor(void *thisReactor)
{
    
}
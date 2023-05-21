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
    newReactor->thread = (pthread_t *)calloc(1, sizeof(pthread_t));
    newReactor->pfds = (struct pollfd *)malloc(sizeof(struct pollfd) * 4);
    if (!newReactor->pfds)
    {
        perror("malloc pfds");
        free(newReactor);
        return NULL;
    }
    // hashmap
    newReactor->FDtoFunction = hashmap_create();
    if (!newReactor->FDtoFunction)
    {
        free(newReactor->pfds);
        free(newReactor);
        printf("hashmap_create error\n");
    }

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

    // start thread
    if (pthread_create(pReactor->thread, NULL, clientListener, thisReactor) != 0)
    {
        perror("pthread_create");
    }
}

void addFd(void *thisReactor, int fd, handler_t handler)
{
    preactor pReactor = (preactor)thisReactor;
    int isInserted = 0;
    int freeIndex = pReactor->fd_count;
    // add fd to pollfd
    // first, find a "free" place
    for (size_t i = 0; i < pReactor->fd_count; i++)
    {
        if (pReactor->pfds[i].fd == -1)
        {
            freeIndex = i;
            isInserted = 1;
            break;
        }
    }
    // If we don't have room, add more space in the pfds array
    if (pReactor->fd_count == pReactor->fd_size && !isInserted)
    {
        pReactor->fd_size *= 2; // Double it

        pReactor->pfds = realloc(pReactor->pfds, sizeof(struct pollfd) * (pReactor->fd_size));

        if (!pReactor->pfds)
        {
            perror("realloc pfds");
            free(pReactor);
            return;
        }
    }
    pReactor->pfds[freeIndex].fd = fd;
    pReactor->pfds[freeIndex].events = POLLIN;
    if (!isInserted)
        pReactor->fd_count++;
    int *fdcpy = (int *)malloc(sizeof(int));
    *fdcpy = fd;
    // add fd to hashmap
    hashmap_set(pReactor->FDtoFunction, fdcpy, sizeof(int), (uintptr_t)handler);
}

void waitFor(void *thisReactor)
{
    preactor pReactor = (preactor)thisReactor;
    pthread_join(*pReactor->thread, NULL);
}


void *clientListener(void *thisReactor)
{
    preactor pReactor = (preactor)thisReactor;

    while (pReactor->keepListening)
    {
        if (poll(pReactor->pfds, pReactor->fd_count, 1000) == -1)
        {
            perror("poll");
            return NULL;
        }
        for (size_t i = 0; i < pReactor->fd_count; i++)
        {
            uintptr_t function;
            if (pReactor->pfds[i].revents & POLLIN)
            {
                int fileDescriptor = pReactor->pfds[i].fd;
                // calling hashmap function
                if (hashmap_get(pReactor->FDtoFunction,
                                &fileDescriptor, sizeof(int), &function) == false)
                {

                    printf("hashmap_get failed\n");
                    continue;
                }
                handler_t clientFunction = (handler_t)function;
                int clientAns = clientFunction(fileDescriptor);
                if (clientAns == -1)
                {
                    printf("client's function failed\n");
                }
                else if (clientAns == 1)
                {
                    pReactor->pfds[i].fd = -1;
                    hashmap_remove(pReactor->FDtoFunction, &fileDescriptor, sizeof(int));
                }

            } // end if
        }     // end for
    }         // end while
    return NULL;
}

void freeReactor(void *thisReactor)
{
    preactor pReactor = (preactor)thisReactor;

    stopReactor(pReactor);

    // free the hashmap
    hashmap_iterate(pReactor->FDtoFunction, free_entry, NULL);
    hashmap_free(pReactor->FDtoFunction);

    // free pfds
    free(pReactor->pfds);

    // free reactor
    free(pReactor);
}

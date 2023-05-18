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
            return;
        }
    }
    pReactor->pfds[pReactor->fd_count].fd = fd;
    pReactor->pfds[pReactor->fd_count].events = POLLIN;
    pReactor->fd_count++;
    int * p = (int*)malloc(sizeof(int));
    *p = fd;
    // add fd to hashmap
    hashmap_set(pReactor->FDtoFunction,p, sizeof(int), (uintptr_t)handler);
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
        printf("count: %d\n", pReactor->fd_count);
        if (poll(pReactor->pfds, pReactor->fd_count, 1000) == -1)
        {
            perror("poll");
            return NULL;
        }
        hashmap_iterate(pReactor->FDtoFunction, print_entry, NULL);
        for (size_t i = 0; i < pReactor->fd_count; i++)
        {
            uintptr_t function;
            if (pReactor->pfds[i].revents & POLLIN)
            {
                printf("%d\n",pReactor->pfds[i].fd);
                // calling hashmap function
                if (hashmap_get(pReactor->FDtoFunction,
                                &pReactor->pfds[i].fd, sizeof(int), &function) == false)
                {

                    printf("hashmap_get failed\n");
                    continue;
                }
                printf("OK\n");
                handler_t a = (handler_t)function;
                if (a(pReactor->pfds[i].fd) == -1)
                {
                    printf("client's function failed\n");
                }
            } // end if
        }     // end for
    }         // end while
    printf("OK\n");
    return NULL;
}

#include <pthread.h>

#include "epoll.h"

#define HASH_SIZE 1023

typedef struct EpollData EpollData;

struct EpollData{
    EpollData *prev;
    EpollData *next;
    int fd;
    int events;
    void *user_data;
    EPCallback callback;
};

struct Epoll{
    int epfd;
    int fds_num;
    EpollData *data_head[HASH_SIZE];
    int eb_size;
    struct epoll_event *eb;
    pthread_mutex_t lock;
};

static inline void epoll_lock(Epoll *ep)
{
    pthread_mutex_lock(&ep->lock);
}

static inline void epoll_unlock(Epoll *ep)
{
    pthread_mutex_unlock(&ep->lock);
}

Epoll *epoll_new(int size)
{
    assert(size <= 0);
    int epfd = epoll_create(size);
    if (epfd < 0){
        return NULL;
    }
    Epoll *ep = (Epoll *)calloc(1, sizeof(Epoll));
    if (!ep){
        goto FAILED;
    }
    ep->eb = (struct epoll_event *)calloc(size, sizeof(struct epoll_event));
    if (!ep->eb){
        goto FAILED;
    }
    ep->eb_size = size;
    ep->epfd = epfd;
    ep->fds_num = 0;
    pthread_mutex_init(&ep->lock, NULL);

    return ep;
FAILED:
    close(epfd);
    if (ep){
        if (ep->eb){
            free(ep->eb);
        }
        free(ep);
    }
    return NULL;
}

static int epoll_add_callback(Epoll *ep, int fd, int events, void *user_data)
{
    
}

static int epoll_remove_callback(Epoll *ep, int fd, int events, void *user_data)
{
    
}

int epoll_add_event(Epoll *ep, int fd, int events, EPCallback callback, void *user_data)
{

}

int epoll_remove_event(Epoll *ep, int fd, int events)
{

}

int epoll_run(Epoll *ep)
{

}

void epoll_free(Epoll *ep)
{

}


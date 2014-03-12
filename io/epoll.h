#ifndef __EPOLL_H
#define __EPOLL_H
#include <sys/epoll.h>

typedef enum{
    EPOLL_IN    = (1 << 0),
    EPOLL_OUT   = (1 << 1),
    EPOLL_URG   = (1 << 2),
    EPOLL_ERR   = (1 << 3),
    EPOLL_ALL   = (0xff)
}EpollEvent;

typedef struct{
    int fd;
    EpollEvent events;
    void *user_data;
}EpollEventData;

typedef struct Epoll Epoll;

typedef int (*EPCallback)(Epoll *ep, EpollEventData *edata);

Epoll *epoll_new(int size);

int epoll_add_event(Epoll *ep, int fd, EpollEvent events, void *user_data, EPCallback callback);

int epoll_remove_event(Epoll *ep, int fd, EpollEvent events);

int epoll_run(Epoll *ep);

void epoll_free(Epoll *ep);

#endif //!__EPOLL_H

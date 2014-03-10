#ifndef __EPOLL_H
#define __EPOLL_H
#include <sys/epoll.h>

typedef struct Epoll Epoll;

typedef int (*EPCallback)(Epoll *ep, int fd, int event, void *user_data);

Epoll *epoll_new(void);

int epoll_add_event(Epoll *ep, int fd, int events, EPCallback callback, void *user_data);

int epoll_remove_event(Epoll *ep, int fd, int events);

int epoll_run(Epoll *ep);

void epoll_free(Epoll *ep);

#endif //!__EPOLL_H

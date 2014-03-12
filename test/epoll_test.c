#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#include "epoll.h"

static int g_fd[2];

void *thread_proc(void *data)
{
    Epoll *ep = (Epoll *)data;
    char *p = "hello world!";
    int len = strlen(p);
    int count = 0;
    while(1){
        sleep(1);
        write(g_fd[1], p, len);
        if (++count >= 5){
            epoll_free(ep);
            break;
        }
    }

    return NULL;
}

static int ep_callback(Epoll *ep, EpollEventData *edata)
{
    char buf[1024] = {0};
    if (edata->events & EPOLL_IN){
        read(edata->fd, buf, 1023);
        printf("%s\n", buf);
    }

    return 0;
}

int main(void)
{
    Epoll *ep = epoll_new(1024);
    pipe(g_fd);
    epoll_add_event(ep, g_fd[0], EPOLL_IN, NULL, ep_callback);

    pthread_t tid;
    pthread_create(&tid, NULL, thread_proc, (void *)ep);

    epoll_run(ep);

    return 0;
}

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "epoll.h"

static int g_fd[2];
static int g_fd2[2];

void *thread_proc(void *data)
{
    Epoll *ep = (Epoll *)data;
    char *p = "hello world!";
    int len = strlen(p);
    int count = 0;
    while(1){
        usleep(250000);
        count++;
        if (count <= 10){
            write(g_fd[1], p, len);
        }else{
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
        int size = read(edata->fd, buf, 1023);
        printf("epcbk: %s\n", buf);
        write(g_fd2[1], buf, size);
    }else{
        printf("epcbk: remove g_fd2[0]\n");
    }

    return 0;
}

static int ep_callback2(Epoll *ep, EpollEventData *edata)
{
    char buf[1024] = {0};
    if (edata->events & EPOLL_IN){
        int size = read(edata->fd, buf, 1023);
        printf("epcbk2: %s\n", buf);
    }

    return 0;
}

int main(void)
{
    Epoll *ep = epoll_new(1024);
    pipe(g_fd);
    pipe(g_fd2);
    epoll_add_event(ep, g_fd[0], EPOLL_IN, NULL, ep_callback);
    epoll_add_event(ep, g_fd2[0], EPOLL_IN, NULL, ep_callback2);

    pthread_t tid;
    pthread_create(&tid, NULL, thread_proc, (void *)ep);

    epoll_run(ep);

    return 0;
}

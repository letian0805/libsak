#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "epool.h"

static int g_fd[2];
static int g_fd2[2];

void *thread_proc(void *data)
{
    EPool *ep = (EPool *)data;
    char *p = "hello world!";
    int len = strlen(p);
    int count = 0;
    while(1){
        usleep(250000);
        count++;
        if (count <= 10){
            write(g_fd[1], p, len);
        }else{
            printf("------------epool_free-------------\n");
            epool_free(ep);
            break;
        }
    }

    return NULL;
}

static void test1(void)
{
    EPool *ep = epool_current();
//    epool_stop(ep);
}

static int ep_callback(EPool *ep, EPEventData *edata)
{
    char buf[1024] = {0};
    if (edata->events & EP_IN){
        int size = read(edata->fd, buf, 1023);
        printf("epcbk: %s\n", buf);
        write(g_fd2[1], buf, size);
        test1();
    }else{
        printf("epcbk: remove g_fd2[0]\n");
    }

    return 0;
}

static int ep_callback2(EPool *ep, EPEventData *edata)
{
    char buf[1024] = {0};
    if (edata->events & EP_IN){
        int size = read(edata->fd, buf, 1023);
        if (size > 0){
            printf("epcbk2: %s\n", buf);
        }
    }

    return 0;
}

int main(void)
{
    EPool *ep = epool_new(1024);
    EPool *ep1 = epool_new(1024);
    if (ep1 == ep){
        printf("------same ep-----\n");
    }
    pipe(g_fd);
    pipe(g_fd2);
    epool_add_event(ep, g_fd[0], EP_IN, NULL, ep_callback);
    epool_add_event(ep, g_fd2[0], EP_IN, NULL, ep_callback2);

    pthread_t tid;
    pthread_create(&tid, NULL, thread_proc, (void *)ep);

    epool_run(ep);
    epool_free(ep);
    printf("--------STOPED-------------\n");

    return 0;
}

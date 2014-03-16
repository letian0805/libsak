#define _GNU_SOURCE
#include <sys/epoll.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>
#include <assert.h>

#include "log.h"
#include "epool.h"

#define HASH_SIZE 1023

typedef struct EPData EPData;

struct EPData{
    EPData *prev;
    EPData *next;
    EPEventData edata;
    EPCallback callback;
};

typedef struct{
    int fd;
    EPEvent events;
    void *user_data;
    EPCallback callback;
}EPAddEvent;

typedef struct{
    int fd;
    EPEvent events;
}EPDelEvent;

struct EPool{
    int epfd;
    volatile bool running;
    int fds_num;
    EPData *data_head[HASH_SIZE];
    int eb_size;
    struct epoll_event *eb;
    int stop_trigger[2];
    int add_trigger[2];
    int del_trigger[2];
    pthread_mutex_t lock;
    pthread_t tid;
};

static EPEvent epool_events_translate_from(uint32_t events);
static int epool_events_translate_to(EPEvent events);
static int epool_add_callback(EPool *ep, EPEventData *edata);
static int epool_del_callback(EPool *ep, EPEventData *edata);
static int epool_stop_callback(EPool *ep, EPEventData *edata);
static int epool_init_trigger(EPool *ep, int fds[2], EPCallback callback);

static inline void epool_lock(EPool *ep)
{
    pthread_mutex_lock(&ep->lock);
}

static inline void epool_unlock(EPool *ep)
{
    pthread_mutex_unlock(&ep->lock);
}

static EPEvent epool_events_translate_from(uint32_t events)
{
    EPEvent evts = 0;
    if ( (events & (EPOLLIN | EPOLLHUP)) ){
        evts |= EP_IN;
    }
    if ( (events & EPOLLOUT) ){
        evts |= EP_OUT;
    }
    if ( (events & EPOLLPRI) ){
        evts |= EP_URG;
    }
    if ( (events & EPOLLERR) ){
        evts |= EP_ERR;
    }

    return evts;
}

static int epool_events_translate_to(EPEvent events)
{
    int evts = 0;
    if ( (events & EP_IN) ){
        evts |= EPOLLIN;
    }
    if ( (events & EP_OUT) ){
        evts |= EPOLLOUT;
    }
    if ( (events & EP_URG) ){
        evts |= EPOLLPRI;
    }

    return evts;
}

static inline int epool_fd_nonblock(int fd)
{
    int flags = 0;
    int ret = -1;

    flags = fcntl (fd, F_GETFL);
    if (flags != -1){
        ret = fcntl (fd, F_SETFL, flags | O_NONBLOCK);
    }
    return ret;
}

EPool *epool_new(int size)
{
    assert(size > 0);
    int epfd = epoll_create(size);
    if (epfd < 0){
        return NULL;
    }
    EPool *ep = (EPool *)calloc(1, sizeof(EPool));
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
    ep->running = false;
    epool_init_trigger(ep, ep->add_trigger, epool_add_callback);
    epool_init_trigger(ep, ep->del_trigger, epool_del_callback);
    epool_init_trigger(ep, ep->stop_trigger, epool_stop_callback);
    pthread_mutex_init(&ep->lock, NULL);
    ep->tid = pthread_self();

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

static int nonblock_read(int fd, char *buf, int size)
{
    int result = 0;
    int ret = 0;
    while(size){
        errno = 0;
        if ( (ret = read(fd, buf, size)) <= 0){
            if (ret == 0){
                return -1;
            }
            if (errno == EAGAIN){
                break;
            }
            if (errno == EINTR){
                continue;
            }
        }
        size -= ret;
        result += ret;
    }

    return result;
}

static inline int epool_get_hash_id(int fd)
{
    return (fd % HASH_SIZE);
}

static inline int epool_insert_edata(EPool *ep, EPData *edata)
{
    int hash_id = epool_get_hash_id(edata->edata.fd);
    EPData *head = ep->data_head[hash_id];
    if (head == NULL){
        ep->data_head[hash_id] = edata;
    }else{
        edata->next = head;
        head->prev = edata;
        ep->data_head[hash_id] = edata;
    }
}

static int epool_add_event_internal(EPool *ep, EPAddEvent *add)
{
    EPData *edata = (EPData *)calloc(1, sizeof(EPData));
    edata->callback = add->callback;
    edata->edata.fd = add->fd;
    edata->edata.user_data = add->user_data;
    edata->edata.events = add->events;

    epool_fd_nonblock(add->fd);

    struct epoll_event epevt = {
        .events = epool_events_translate_to(add->events),
        .data = {.ptr = edata},
    };
    epoll_ctl(ep->epfd, EPOLL_CTL_ADD, add->fd, &epevt);
    epool_insert_edata(ep, edata);

    return 0;
}

static int epool_add_callback(EPool *ep, EPEventData *edata)
{
    EPAddEvent add;
    while(nonblock_read(edata->fd, (char *)&add, sizeof(add)) == sizeof(add)){
        epool_add_event_internal(ep, &add);
    }
    
    return 0;
}

static int epool_del_event_internal(EPool *ep, EPDelEvent *del)
{
    int idx = epool_get_hash_id(del->fd);
    EPData *edata_head = ep->data_head[idx];
    EPData *edata = edata_head;
    while(edata && edata->edata.fd != del->fd){
        edata = edata->next;
    }
    if (!edata){
        return -1;
    }
    int events = (del->events & edata->edata.events);
    if ( events == edata->edata.events){
        epoll_ctl(ep->epfd, EPOLL_CTL_DEL, del->fd, NULL);
        if (edata == edata_head){
            ep->data_head[idx] = edata_head->next;
            ep->data_head[idx]->prev = NULL;
        }else{
            edata->prev->next = edata->next;
            edata->next->prev = edata->prev;
        }
        free(edata);
    }else if (events > 0){
        struct epoll_event epevt = {
            .events = epool_events_translate_to(events),
            .data = {.ptr = edata},
        };
        epoll_ctl(ep->epfd, EPOLL_CTL_MOD, del->fd, &epevt);
    }

    return 0;
}

static int epool_del_callback(EPool *ep, EPEventData *edata)
{
    EPDelEvent del;
    while(nonblock_read(edata->fd, (char *)&del, sizeof(del)) == sizeof(del)){
        epool_del_event_internal(ep, &del);
    }

    return 0;
}

static int epool_stop_callback(EPool *ep, EPEventData *edata)
{
    char c;
    while(nonblock_read(edata->fd, &c, 1) == 1);

    ep->running = false;

    return 0;
}

static int epool_init_trigger(EPool *ep, int fds[2], EPCallback callback)
{
    if (pipe(fds) != 0){
        return -1;
    }
    epool_fd_nonblock(fds[0]);
    epool_fd_nonblock(fds[1]);

    EPAddEvent add = {
        .fd = fds[0],
        .events = EP_IN,
        .callback = callback,
    };
    return epool_add_event_internal(ep, &add);
}

int epool_add_event(EPool *ep, int fd, EPEvent events, void *user_data, EPCallback callback)
{
    EPAddEvent add = {
        .events = events,
        .fd = fd,
        .user_data = user_data,
        .callback = callback,
    };
    if (pthread_equal(ep->tid, pthread_self())){
        epool_add_event_internal(ep, &add);
    }else{
        write(ep->add_trigger[1], &add, sizeof(add));
    }

    return 0;
}

int epool_del_event(EPool *ep, int fd, EPEvent events)
{
    EPDelEvent del = {
        .fd = fd,
        .events = events,
    };
    if (pthread_equal(ep->tid, pthread_self())){
        epool_del_event_internal(ep, &del);
    }else{
        write(ep->del_trigger[1], &del, sizeof(del));
    }

    return 0;
}

int epool_time_wait(EPool *ep, int time_ms)
{
    int evt_rets = epoll_wait(ep->epfd, ep->eb, ep->eb_size, time_ms);
    int i;
    for (i = 0; i < evt_rets && ep->running; i++){
        EPData *ed = (EPData *)ep->eb[i].data.ptr;
        EPEvent events = epool_events_translate_from(ep->eb[i].events);
        EPEventData edata = {
            .fd = ed->edata.fd,
            .events = events,
            .user_data = ed->edata.user_data
        };
        ed->callback(ep, &edata);
    }
    
    return evt_rets;
}

int epool_run(EPool *ep)
{
    int evt_rets = 0;
    epool_lock(ep);
    ep->tid = pthread_self();
    ep->running = true;
    epool_unlock(ep);
    while(ep->running){
        epool_time_wait(ep, -1);
    }

    return 0;
}

int epool_stop(EPool *ep)
{
    write(ep->stop_trigger[1], "1", 1);

    return 0;
}

void epool_free(EPool *ep)
{
    if (!pthread_equal(ep->tid, pthread_self())){
        epool_stop(ep);
    }
    while(ep->running){
        usleep(100);
    }
    close(ep->add_trigger[0]);
    close(ep->add_trigger[1]);
    close(ep->del_trigger[0]);
    close(ep->del_trigger[1]);
    close(ep->stop_trigger[0]);
    close(ep->stop_trigger[1]);
    close(ep->epfd);
    free(ep->eb);
    free(ep);
}


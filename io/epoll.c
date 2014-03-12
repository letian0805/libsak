#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>
#include <assert.h>

#include "epoll.h"

#define HASH_SIZE 1023

typedef struct EpollData EpollData;

struct EpollData{
    EpollData *prev;
    EpollData *next;
    EpollEventData edata;
    EPCallback callback;
};

typedef struct{
    int fd;
    EpollEvent events;
    void *user_data;
    EPCallback callback;
}EpollAddEvent;

typedef struct{
    int fd;
    EpollEvent events;
}EpollDelEvent;

struct Epoll{
    int epfd;
    volatile bool running;
    int fds_num;
    EpollData *data_head[HASH_SIZE];
    int eb_size;
    struct epoll_event *eb;
    int stop_trigger[2];
    int add_trigger[2];
    int rm_trigger[2];
    pthread_mutex_t lock;
    pthread_t tid;
};

static EpollEvent epoll_events_translate_from(uint32_t events);
static int epoll_events_translate_to(EpollEvent events);
static int epoll_add_callback(Epoll *ep, EpollEventData *edata);
static int epoll_remove_callback(Epoll *ep, EpollEventData *edata);
static int epoll_stop_callback(Epoll *ep, EpollEventData *edata);
static int epoll_init_trigger(Epoll *ep, int fds[2], EPCallback callback);

static inline void epoll_lock(Epoll *ep)
{
    pthread_mutex_lock(&ep->lock);
}

static inline void epoll_unlock(Epoll *ep)
{
    pthread_mutex_unlock(&ep->lock);
}

static inline int epoll_fd_nonblock(int fd)
{
    int flags = 0;
    int ret = -1;

    flags = fcntl (fd, F_GETFL);
    if (flags != -1){
        ret = fcntl (fd, F_SETFL, flags | O_NONBLOCK);
    }
    return ret;
}

Epoll *epoll_new(int size)
{
    assert(size > 0);
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
    ep->running = false;
    epoll_init_trigger(ep, ep->add_trigger, epoll_add_callback);
    epoll_init_trigger(ep, ep->rm_trigger, epoll_remove_callback);
    epoll_init_trigger(ep, ep->stop_trigger, epoll_stop_callback);
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

static int nonblock_read(int fd, char *buf, int size)
{
    int result = 0;
    int ret = 0;
    while(size){
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

static inline int epoll_get_hash_id(int fd)
{
    return (fd % HASH_SIZE);
}

static inline int epoll_insert_edata(Epoll *ep, EpollData *edata)
{
    int hash_id = epoll_get_hash_id(edata->edata.fd);
    EpollData *head = ep->data_head[hash_id];
    if (head == NULL){
        ep->data_head[hash_id] = edata;
    }else{
        edata->next = head;
        head->prev = edata;
        ep->data_head[hash_id] = edata;
    }
}

static int epoll_add_event_internal(Epoll *ep, EpollAddEvent *add)
{
    EpollData *edata = (EpollData *)calloc(1, sizeof(EpollData));
    edata->callback = add->callback;
    edata->edata.fd = add->fd;
    edata->edata.user_data = add->user_data;
    edata->edata.events = add->events;

    epoll_fd_nonblock(add->fd);

    struct epoll_event epevt = {
        .events = epoll_events_translate_to(add->events),
        .data = {.ptr = edata},
    };
    epoll_ctl(ep->epfd, EPOLL_CTL_ADD, add->fd, &epevt);
    epoll_insert_edata(ep, edata);

    return 0;
}

static int epoll_add_callback(Epoll *ep, EpollEventData *edata)
{
    EpollAddEvent add;
    if (nonblock_read(edata->fd, (char *)&add, sizeof(add)) == sizeof(add)){
        return epoll_add_event_internal(ep, &add);
    }

    return -1;
}

static int epoll_rm_event_internal(Epoll *ep, EpollDelEvent *del)
{
    int idx = epoll_get_hash_id(del->fd);
    EpollData *edata_head = ep->data_head[idx];
    EpollData *edata = edata_head;
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
            .events = epoll_events_translate_to(events),
            .data = {.ptr = edata},
        };
        epoll_ctl(ep->epfd, EPOLL_CTL_MOD, del->fd, &epevt);
    }

    return 0;
}

static int epoll_remove_callback(Epoll *ep, EpollEventData *edata)
{
    EpollDelEvent del;
    if (nonblock_read(edata->fd, (char *)&del, sizeof(del)) == sizeof(del)){
        return epoll_rm_event_internal(ep, &del);
    }

    return -1;
}

static int epoll_stop_callback(Epoll *ep, EpollEventData *edata)
{
    char c;
    while(nonblock_read(edata->fd, &c, 1) == 1);

    ep->running = false;

    return 0;
}

static int epoll_init_trigger(Epoll *ep, int fds[2], EPCallback callback)
{
    if (pipe2(fds, O_NONBLOCK) != 0){
        return -1;
    }
    epoll_fd_nonblock(fds[0]);
    epoll_fd_nonblock(fds[1]);

    EpollAddEvent add = {
        .fd = fds[0],
        .events = EPOLL_IN,
        .callback = callback,
    };
    return epoll_add_event_internal(ep, &add);
}

static EpollEvent epoll_events_translate_from(uint32_t events)
{
    EpollEvent evts = 0;
    if ( (events & (EPOLLIN | EPOLLHUP)) ){
        evts |= EPOLL_IN;
    }
    if ( (events & EPOLLOUT) ){
        evts |= EPOLL_OUT;
    }
    if ( (events & EPOLLPRI) ){
        evts |= EPOLL_URG;
    }
    if ( (events & EPOLLERR) ){
        evts |= EPOLL_ERR;
    }

    return evts;
}

static int epoll_events_translate_to(EpollEvent events)
{
    int evts = 0;
    if ( (events & EPOLL_IN) ){
        evts |= EPOLLIN;
    }
    if ( (events & EPOLL_OUT) ){
        evts |= EPOLLOUT;
    }
    if ( (events & EPOLL_URG) ){
        evts |= EPOLLPRI;
    }

    return evts;
}


int epoll_add_event(Epoll *ep, int fd, EpollEvent events, void *user_data, EPCallback callback)
{
    EpollAddEvent add = {
        .events = events,
        .fd = fd,
        .user_data = user_data,
        .callback = callback,
    };
    write(ep->add_trigger[1], &add, sizeof(add));

    return 0;
}

int epoll_remove_event(Epoll *ep, int fd, EpollEvent events)
{
    EpollDelEvent del = {
        .fd = fd,
        .events = events,
    };
    write(ep->rm_trigger[1], &del, sizeof(del));

    return 0;
}

int epoll_run(Epoll *ep)
{
    int evt_rets = 0;
    ep->running = true;
    while(ep->running){
        evt_rets = epoll_wait(ep->epfd, ep->eb, ep->eb_size, -1);
        int i;
        for (i = 0; i < evt_rets && ep->running; i++){
            EpollData *ed = (EpollData *)ep->eb[i].data.ptr;
            EpollEvent events = epoll_events_translate_from(ep->eb[i].events);
            EpollEventData edata = {
                .fd = ed->edata.fd,
                .events = events,
                .user_data = ed->edata.user_data
            };
            ed->callback(ep, &edata);
        }
    }

    return 0;
}

int epoll_stop(Epoll *ep)
{
    write(ep->stop_trigger[1], "1", 1);

    return 0;
}

void epoll_free(Epoll *ep)
{
    if (!pthread_equal(ep->tid, pthread_self())){
        epoll_stop(ep);
        return;
    }
    while(ep->running){
        usleep(1000);
    }

    free(ep);
}


#ifndef __EPOOL_H
#define __EPOOL_H
typedef enum{
    EP_IN    = (1 << 0),
    EP_OUT   = (1 << 1),
    EP_URG   = (1 << 2),
    EP_ERR   = (1 << 3),
    EP_ALL   = (0xff)
}EPEvent;

typedef struct{
    int fd;
    EPEvent events;
    void *user_data;
}EPEventData;

typedef struct EPool EPool;

typedef int (*EPCallback)(EPool *ep, EPEventData *edata);

EPool *epool_new(int size);

int epool_add_event(EPool *ep, int fd, EPEvent events, void *user_data, EPCallback callback);

int epool_del_event(EPool *ep, int fd, EPEvent events);

int epool_time_wait(EPool *ep, int time_ms);

int epool_run(EPool *ep);

int epool_stop(EPool *ep);

void epool_free(EPool *ep);

#endif //!__EPOOL_H

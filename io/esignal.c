#include <stdint.h>

#include "esignal.h"

typedef struct ESignal ESignal;
struct ESignal{
    uint64_t hash;
    int sigfd[2];
    void *owner;
    char *name;
    ESCallback cbk;
    EPool *ep;
};

int esignal_add(void *owner, char *sig_name, ESCallback callback,  EPool *ep)
{

    return 0;
}

int esignal_emit(void *owner, char *sig_name, void *data)
{

    return 0;
}

int esignal_remove(void *owner, char *sig_name)
{

    return 0;
}


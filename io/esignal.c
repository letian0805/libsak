#include <stdint.h>
#include <stdlib.h>

#include "esignal.h"

#define ES_SLOT_SIZE 65536

typedef struct ESignal ESignal;
struct ESignal{
    uint64_t hash;
    int sigfd[2];
    void *owner;
    char *name;
    ESCallback cbk;
    EPool *ep;
};

static ESignal *esignal_slot[ES_SLOT_SIZE];

static int esignal_insert(ESignal *es)
{

    return 0;
}

static ESignal *esignal_new(void *owner, char *sig_name, ESCallback callback)
{
    ESignal *es = (ESignal *)calloc(1, sizeof(ESignal));

    

    return es;
}

int esignal_add(void *owner, char *sig_name, ESCallback callback)
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


#ifndef __ESIGNAL_H
#define __ESIGNAL_H
#include "epool.h"

typedef int (*ESCallback)(void *owner, void *data);

int esignal_add(void *owner, char *sig_name, ESCallback callback, EPool *ep);

int esignal_emit(void *owner, char *sig_name, void *data);

int esignal_remove(void *owner, char *sig_name);

#endif //!__EVENT_SIGNAL_H

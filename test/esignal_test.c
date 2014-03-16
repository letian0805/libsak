#include "esignal.h"
#include "log.h"

static char *g_owner = "estest";

int esignal_test_callback(void *owner, void *data)
{
    DEBUG("-------signal callback-----");
    esignal_remove(g_owner, "esignal test");
    return 0;
}

int main(void)
{
    EPool *ep = epool_new(1024);
    esignal_add(g_owner, "esignal test", esignal_test_callback, ep);
    esignal_emit(g_owner, "esignal test", "hello world");

    epool_run(ep);

    return 0;
}

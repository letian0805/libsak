#include "sak.h"
#include "esignal.h"

static char *g_owner = "estest";

int esignal_test_callback(void *owner, void *data)
{
    SAK_DEBUG("-------signal callback-----");
    esignal_add(g_owner, "esignal test2", esignal_test_callback);
    esignal_remove(g_owner, "esignal test");
    esignal_emit(g_owner, "esignal test2", "hello world");
    return 0;
}

int esignal_test2_callback(void *owner, void *data)
{
    SAK_DEBUG("-------signal callback-----");

    return 0;
}

int main(void)
{
    EPool *ep = epool_new(1024);
    esignal_add(g_owner, "esignal test", esignal_test_callback);
    esignal_emit(g_owner, "esignal test", "hello world");

    epool_run(ep);

    return 0;
}

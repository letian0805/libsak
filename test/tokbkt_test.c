#include "token_bucket.h"
#include "log.h"

int main(void)
{
    TokenBucketManager *tbman = token_bucket_manager_create(6, 10);
    TokenBucket *tb = token_bucket_create(1024*8, 1024*1024);
    token_bucket_set_manager(tb, tbman);
    while(1){
        int ret = token_bucket_get(tb, 1024*8);
        DEBUG("------get token: %d-----\n", ret);
    }

    return 0;
}

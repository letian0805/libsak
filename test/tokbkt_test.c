#include <time.h>
#include <sys/time.h>
#include "token_bucket.h"
#include "log.h"

int main(void)
{
    TokenBucketManager *tbman = token_bucket_manager_create(6, 10);
    TokenBucket *tb = token_bucket_create(1024*1024);
    token_bucket_set_manager(tb, tbman);
    struct timeval tm1;
    struct timeval tm2;
    int datasize = 1024*4;
    int times = 3000;
    int sum = 0;
    gettimeofday(&tm1, NULL);
    int i;
    for(i = 0; i < times; i++){
        int ret = token_bucket_get(tb, datasize);
        sum += ret;
//        DEBUG("------get token: %d-----\n", ret);
    }
    gettimeofday(&tm2, NULL);
    double time = tm2.tv_sec - tm1.tv_sec + (tm2.tv_usec - tm1.tv_usec)/1000000.0;
    double speed = (double)sum/time;
    DEBUG("--------time: %lfS, sum: %d\n", time, sum);

    if (speed > 1024*1024){
        DEBUG("------speed is: %lf Mb/s\n", speed/(1024*1024));
    }else{
        DEBUG("------speed is: %lf Kb/s\n", speed/1024);
    }

    return 0;
}

#include <time.h>
#include <sys/time.h>
#include "token_bucket.h"
#include "log.h"

int main(void)
{
    TokenBucketManager *tbman = token_bucket_manager_create(6, 1);
    int bitrate = 1024*1024;
    TokenBucket *tb = token_bucket_create(bitrate);
    token_bucket_set_manager(tb, tbman);
    struct timeval tm1;
    struct timeval tm2;
    srand(time(NULL));
    int datasize = bitrate/128;
    int sumsize = 4*bitrate;
    int sum = 0;
    gettimeofday(&tm1, NULL);
    int i;
    do{
        int size = datasize - (((rand()*1000) % datasize) - datasize/2);
        int ret = token_bucket_get(tb, size);
        sum += ret;
    }while(sum < sumsize);
    gettimeofday(&tm2, NULL);
    double time = tm2.tv_sec - tm1.tv_sec + (tm2.tv_usec - tm1.tv_usec)/1000000.0;
    double speed = (double)sum/time;
    DEBUG("--------time: %lfS, sum: %d\n", time, sum);

    if (speed >= 1024*1024){
        DEBUG("------speed is: %lf Mb/s\n", speed/(1024*1024));
    }else if (speed >= 1024){
        DEBUG("------speed is: %lf Kb/s\n", speed/1024);
    }else{
        DEBUG("------speed is: %lf b/s\n", speed);
    }

    return 0;
}

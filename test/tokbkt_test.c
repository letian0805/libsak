#include <time.h>
#include <stdio.h>
#include <sys/time.h>

#include "token_bucket.h"

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
    int sumsize = 2*bitrate;
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
    printf("--------time: %lfS, sum: %d\n", time, sum);
    token_bucket_destroy(tb);

    if (speed >= 1024*1024){
        printf("------speed is: %lf Mb/s\n", speed/(1024*1024));
    }else if (speed >= 1024){
        printf("------speed is: %lf Kb/s\n", speed/1024);
    }else{
        printf("------speed is: %lf b/s\n", speed);
    }
    int deviation = speed - bitrate;
    if (deviation >= 1024){
        printf("----speed deviation: %.2f%%, %.2fKb/s\n", (100.0 * deviation) / bitrate, deviation / 1024.0);
    }else{
        printf("----speed deviation: %.2f%%, %db/s\n", (100.0 * deviation) / bitrate, deviation);
    }

    return 0;
}

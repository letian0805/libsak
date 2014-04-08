#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "log.h"
#include "token_bucket.h"

#define MIN_BITRATE (1024*4)

struct TokenBucket{
    int size;
    int limit;
    volatile int tokens;
    int bitrate;
    int id;
    pthread_mutex_t wait_lock;
    pthread_spinlock_t lock;
    TokenBucketManager *tbman;
    TBStateCallback callback;
    void *user_data;
};

struct TokenBucketManager{
    int maxtbs;
    int interval_ms;
    struct timeval start_time;
    volatile int64_t run_time;
    volatile int running;
    int ntbs;
    TokenBucket **tbs;
    pthread_spinlock_t lock;
    pthread_t tid;
};

static inline void tbmanager_lock(TokenBucketManager *tbman)
{
    pthread_spin_lock(&tbman->lock);
}

static inline void tbmanager_unlock(TokenBucketManager *tbman)
{
    pthread_spin_unlock(&tbman->lock);
}

static inline int64_t tbmanager_get_runtime(TokenBucketManager *tbman)
{
    struct timeval tm;
    gettimeofday(&tm, NULL);
    int64_t tm_us = (tm.tv_sec - tbman->start_time.tv_sec) * 1000000;
    tm_us += tm.tv_usec - tbman->start_time.tv_usec;
    return tm_us;
}

static inline void tbmanager_sleep(TokenBucketManager *tbman)
{
    tbmanager_lock(tbman);
    int64_t tm_us = tbmanager_get_runtime(tbman) - tbman->run_time * 1000;
    int64_t ust = tbman->interval_ms * 1000 - tm_us;
    tbman->run_time += tbman->interval_ms;
    if (ust < 0){
        ust = 0;
    }
    tbmanager_unlock(tbman);
    usleep(ust);
}

static inline void tb_lock(TokenBucket *tb)
{
    pthread_spin_lock(&tb->lock);
}

static inline void tb_unlock(TokenBucket *tb)
{
    pthread_spin_unlock(&tb->lock);
}

static inline int tbmanager_add_token_bucket(TokenBucketManager *tbman, TokenBucket *tb)
{
    int id = -1;
    tbmanager_lock(tbman);
    if (tbman->ntbs < tbman->maxtbs){
        tbman->tbs[tbman->ntbs] = tb;
        id = tbman->ntbs;
        tbman->ntbs++;
    }
    tbmanager_unlock(tbman);

    return id;
}

static inline void tbmanager_del_token_bucket(TokenBucketManager *tbman, TokenBucket *tb)
{
    if (!tbman || !tb){
        return;
    }
    tbmanager_lock(tbman);
    tb_lock(tb);
    int id = tb->id;
    tb->tbman = NULL;
    tb->id = -1;
    tb_unlock(tb);
    if (id >= 0 && id < tbman->ntbs && tbman->tbs[id] == tb){
        tbman->ntbs--;
        TokenBucket *bkt = tbman->tbs[tbman->ntbs];
        if (bkt != tb){
            tb_lock(bkt);
            bkt->id = id;
            tb_unlock(bkt);
            tbman->tbs[id] = bkt;
        }
        tbman->tbs[tbman->ntbs] = NULL;
    }
    tbmanager_unlock(tbman);
}

static inline void tbmanager_put_token(TokenBucketManager *tbman)
{
    int i;
    int tokens = 0;
    TokenBucket *tb = NULL;
    for (i = 0; i < tbman->ntbs; i++){
        tb = tbman->tbs[i];
        tokens = (tb->bitrate * tbman->interval_ms) / 1000;
        token_bucket_put(tb, tokens);
    }
}

static void *tbmanager_thread(void *data)
{
    TokenBucketManager *tbman = (TokenBucketManager *)data;
    tbmanager_lock(tbman);
    tbman->running = 1;
    tbman->run_time = 0;
    tbmanager_unlock(tbman);
    gettimeofday(&tbman->start_time, NULL);
    while(1){
        tbmanager_lock(tbman);
        if(!tbman->running){
            tbmanager_unlock(tbman);
            break;
        }
        tbmanager_put_token(tbman);
        tbmanager_unlock(tbman);
        tbmanager_sleep(tbman);
    }

    return 0;
}

TokenBucketManager *token_bucket_manager_create(int maxtbs, int interval_ms)
{
    assert(maxtbs > 0);
    assert(interval_ms > 0);

    TokenBucketManager *tbman = (TokenBucketManager *)calloc(1, sizeof(TokenBucketManager));
    if (!tbman){
        return NULL;
    }
    tbman->maxtbs = maxtbs;
    tbman->interval_ms = interval_ms;
    tbman->ntbs = 0;
    tbman->running = 0;
    tbman->tbs = (TokenBucket **)calloc(maxtbs, sizeof(TokenBucket *));
    pthread_spin_init(&tbman->lock, 0);
    pthread_create(&tbman->tid, NULL, tbmanager_thread, tbman);

    return tbman;
}

void token_bucket_manager_destroy(TokenBucketManager *tbman)
{
    void *ret = NULL;
    tbmanager_lock(tbman);
    tbman->running = 0;
    tbmanager_unlock(tbman);
    pthread_join(tbman->tid, &ret);
    free(tbman->tbs);
    pthread_spin_destroy(&tbman->lock);
    free(tbman);
}

TokenBucket *token_bucket_create(int bitrate)
{
    assert(bitrate >= MIN_BITRATE);

    TokenBucket *tb = (TokenBucket *)calloc(1, sizeof(TokenBucket));
    pthread_mutex_init(&tb->wait_lock, NULL);
    pthread_spin_init(&tb->lock, 0);
    token_bucket_set_bitrate(tb, bitrate);
    tb->tokens = 0;
    tb->tbman = NULL;
    tb->callback = NULL;
    tb->user_data = NULL;
    tb->id = -1;

    return tb;
}

int token_bucket_set_manager(TokenBucket *tb, TokenBucketManager *tbman)
{
    assert(tb && tbman);

    int ret = -1;
    tb_lock(tb);
    tb->id = tbmanager_add_token_bucket(tbman, tb);
    if (tb->id >= 0){
        tb->tbman = tbman;
        ret = 0;
    }
    tb_unlock(tb);

    return ret;
}

int token_bucket_set_state_callback(TokenBucket *tb, TBStateCallback callback, void *user_data)
{
    tb_lock(tb);
    tb->callback = callback;
    tb->user_data = user_data;
    tb_unlock(tb);
    return 0;
}

int token_bucket_put(TokenBucket *tb, int tokens)
{
    assert(tb && tokens > 0);

    int oldtok = 0;
    int ret = 0;
    tb_lock(tb);
    oldtok = tb->tokens;
    tb->tokens += tokens;
    if (tb->tokens > tb->size){
        tb->tokens = tb->size;
    }
    if (oldtok <= tb->limit && tb->tokens > tb->limit){
        if (tb->callback){
            tb->callback(tb, TB_STATE_UNLOCK, tb->user_data);
        }
        pthread_mutex_unlock(&tb->wait_lock);
    }
    ret = tb->tokens - oldtok;
    tb_unlock(tb);

    return ret;
}

int token_bucket_get(TokenBucket *tb, int tokens, bool nonblock)
{
    assert(tb && tokens > 0);

    int oldtok = 0;
    int ret = 0;
    if (nonblock){
        if (pthread_mutex_trylock(&tb->wait_lock)){
            return -1;
        }
    }else{
        pthread_mutex_lock(&tb->wait_lock);
    }
    tb_lock(tb);
    oldtok = tb->tokens;
    tb->tokens -= tokens;
    if (tb->tokens > tb->limit){
        pthread_mutex_unlock(&tb->wait_lock);
    }else if (tb->callback){
        tb->callback(tb, TB_STATE_LOCK, tb->user_data);
    }
    ret = oldtok - tb->tokens;
    tb_unlock(tb);

    return ret;
}

int token_bucket_set_bitrate(TokenBucket *tb, int bitrate)
{
    assert(tb && bitrate >= MIN_BITRATE);

    tb_lock(tb);
    tb->bitrate = bitrate;
    tb->size = bitrate/4;
    tb->limit = bitrate/1024;
    if (tb->tokens > tb->size){
        tb->tokens = tb->size;
    }
    tb_unlock(tb);

    return 0;
}

void token_bucket_destroy(TokenBucket *tb)
{
    tbmanager_del_token_bucket(tb->tbman, tb);
    pthread_mutex_destroy(&tb->wait_lock);
    pthread_spin_destroy(&tb->lock);
    free(tb);
}

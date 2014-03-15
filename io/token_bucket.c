#include <pthread.h>
#include <stdlib.h>

#include "log.h"
#include "token_bucket.h"

struct TokenBucket{
    int size;
    int limit;
    volatile int tokens;
    int bitrate;
    int id;
    pthread_mutex_t wait_lock;
    pthread_spinlock_t lock;
    TokenBucketManager *tbman;
};

struct TokenBucketManager{
    int maxbkts;
    int interval_ms;
    int nbkts;
    volatile int running;
    TokenBucket **bkts;
    pthread_spinlock_t lock;
    pthread_mutex_t thread_lock;
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
    if (tbman->nbkts < tbman->maxbkts){
        tbman->bkts[tbman->nbkts] = tb;
        id = tbman->nbkts;
        tbman->nbkts++;
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
    if (tb->id >= 0 && tb->id < tbman->nbkts && tbman->bkts[tb->id] == tb){
        tbman->nbkts--;
        tbman->bkts[tb->id] = tbman->bkts[tbman->nbkts];
        tbman->bkts[tbman->nbkts] = NULL;
        tb->tbman = NULL;
        tb->id = -1;
    }
    tb_unlock(tb);
    tbmanager_unlock(tbman);
}

static inline void tbmanager_put_token(TokenBucketManager *tbman)
{
    int i;
    int tokens = 0;
    TokenBucket *bkt = NULL;
    for (i = 0; i < tbman->nbkts; i++){
        bkt = tbman->bkts[i];
        tokens = (bkt->bitrate * tbman->interval_ms) / 1000;
        token_bucket_put(bkt, tokens);
    }
}

static void *tbmanager_thread(void *data)
{
    TokenBucketManager *tbman = (TokenBucketManager *)data;
    tbmanager_lock(tbman);
    tbman->running = 1;
    pthread_mutex_lock(&tbman->thread_lock);
    tbmanager_unlock(tbman);
    while(1){
        tbmanager_lock(tbman);
        if(!tbman->running){
            tbmanager_unlock(tbman);
            break;
        }
        tbmanager_put_token(tbman);
        tbmanager_unlock(tbman);
        usleep(tbman->interval_ms*1000);
    }

    pthread_mutex_unlock(&tbman->thread_lock);
    return 0;
}

TokenBucketManager *token_bucket_manager_create(int maxbkts, int interval_ms)
{
    TokenBucketManager *tbman = (TokenBucketManager *)calloc(1, sizeof(TokenBucketManager));
    if (!tbman){
        return NULL;
    }
    tbman->maxbkts = maxbkts;
    tbman->interval_ms = interval_ms;
    tbman->nbkts = 0;
    tbman->running = 0;
    tbman->bkts = (TokenBucket **)calloc(maxbkts, sizeof(TokenBucket *));
    pthread_spin_init(&tbman->lock, 0);
    pthread_create(&tbman->tid, NULL, tbmanager_thread, tbman);

    return tbman;
}

void token_bucket_manager_destroy(TokenBucketManager *tbman)
{

}

TokenBucket *token_bucket_create(int bitrate)
{
    TokenBucket *tb = (TokenBucket *)calloc(1, sizeof(TokenBucket));
    tb->bitrate = bitrate;
    tb->size = bitrate/8;
    tb->tokens = tb->size;
    tb->limit = (tb->size/64 > 512) ? 512 : tb->size/64;
    tb->tbman = NULL;
    tb->id = -1;
    pthread_mutex_init(&tb->wait_lock, NULL);
    pthread_spin_init(&tb->lock, 0);

    return tb;
}

int token_bucket_set_manager(TokenBucket *bkt, TokenBucketManager *tbman)
{
    int ret = -1;
    tb_lock(bkt);
    bkt->id = tbmanager_add_token_bucket(tbman, bkt);
    if (bkt->id >= 0){
        bkt->tbman = tbman;
        ret = 0;
    }
    tb_unlock(bkt);

    return ret;
}

int token_bucket_put(TokenBucket *bkt, int tokens)
{
    if (!bkt || tokens <= 0){
        return 0;
    }
    int oldtok = 0;
    int ret = 0;
    tb_lock(bkt);
    oldtok = bkt->tokens;
    bkt->tokens += tokens;
    if (bkt->tokens > bkt->size){
        bkt->tokens = bkt->size;
    }
    if (oldtok <= bkt->limit && bkt->tokens > bkt->limit){
        pthread_mutex_unlock(&bkt->wait_lock);
    }
    ret = bkt->tokens - oldtok;
    tb_unlock(bkt);

    return ret;
}

int token_bucket_get(TokenBucket *bkt, int tokens)
{
    if (!bkt || tokens <= 0){
        ERROR("---wrong args----");
        return 0;
    }
    int oldtok = 0;
    int ret = 0;
    pthread_mutex_lock(&bkt->wait_lock);
    tb_lock(bkt);
    oldtok = bkt->tokens;
    bkt->tokens -= tokens;
    if (bkt->tokens > bkt->limit){
        pthread_mutex_unlock(&bkt->wait_lock);
    }
    ret = oldtok - bkt->tokens;
    tb_unlock(bkt);

    return ret;
}

int token_bucket_set_bitrate(TokenBucket *bkt, int bitrate)
{
    if (!bkt || bitrate < 1024){
        return -1;
    }
    tb_lock(bkt);
    bkt->bitrate = bitrate;
    tb_unlock(bkt);

    return 0;
}

int token_bucket_set_size(TokenBucket *bkt, int size)
{
    if (!bkt || size <= 4096){
        return -1;
    }
    tb_lock(bkt);
    bkt->size = size;
    if (bkt->tokens > bkt->size){
        bkt->tokens = bkt->size;
    }
    tb_unlock(bkt);

    return 0;
}

void token_bucket_destroy(TokenBucket *bkt)
{
    tbmanager_del_token_bucket(bkt->tbman, bkt);
    pthread_mutex_destroy(&bkt->wait_lock);
    pthread_spin_destroy(&bkt->lock);
    free(bkt);
}

void token_bucket_term(void)
{

}

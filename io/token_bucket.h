#ifndef __TOKEN_BUCKET_H
#define __TOKEN_BUCKET_H

typedef struct TokenBucket TokenBucket;
typedef struct TokenBucketManager TokenBucketManager;

#if 0
struct TokenBucket{
    int size;
    int tokens;
    int bitrate;
    pthread_mutex_t wait_lock;
    pthread_spinlock_t lock;
};
#endif

TokenBucketManager *token_bucket_manager_create(int maxtbs, int interval_ms);

void token_bucket_manager_destroy(TokenBucketManager *tbman);

TokenBucket *token_bucket_create(int bitrate);

int token_bucket_set_manager(TokenBucket *tb, TokenBucketManager *tbman);

int token_bucket_put(TokenBucket *tb, int tokens);

int token_bucket_get(TokenBucket *tb, int tokens);

int token_bucket_set_bitrate(TokenBucket *tb, int bitrate);

int token_bucket_set_size(TokenBucket *tb, int size);

void token_bucket_destroy(TokenBucket *tb);

#endif //!__TOKEN_BUCKET_H

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

TokenBucketManager *token_bucket_manager_create(int maxbkts, int interval_ms);

void token_bucket_manager_destroy(TokenBucketManager *tbman);

TokenBucket *token_bucket_create(int bitrate);

int token_bucket_set_manager(TokenBucket *bkt, TokenBucketManager *tbman);

int token_bucket_put(TokenBucket *bkt, int tokens);

int token_bucket_get(TokenBucket *bkt, int tokens);

int token_bucket_set_bitrate(TokenBucket *bkt, int bitrate);

int token_bucket_set_size(TokenBucket *bkt, int size);

void token_bucket_destroy(TokenBucket *bkt);

#endif //!__TOKEN_BUCKET_H

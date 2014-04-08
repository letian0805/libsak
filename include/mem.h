#ifndef __MEM_H
#define __MEM_H

#ifdef __cplusplus
extern "C" {
#endif

int mem_add_pool(int blksize);

void mem_del_pool(int blksize);

void mem_del_pool_by_id(int mpid);

void *mem_malloc(int size);

void *mem_map(int size);

void *mem_shmget(int size);

void *mem_poolget(int size);

int mem_size(void *addr);

void mem_free(void *addr);

#ifdef __cplusplus
}
#endif

#endif //!__MEM_H

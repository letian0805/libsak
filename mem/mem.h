#ifndef __MEM_H
#define __MEM_H

int mem_add_pool(int blksize);

void mem_del_pool(int blksize);

void mem_del_pool_by_id(int mpid);

void *mem_malloc(int size);

void *mem_map(int size);

void *mem_shmget(int size);

void *mem_poolget(int size);

int mem_size(void *addr);

void mem_free(void *addr);

#endif //!__MEM_H

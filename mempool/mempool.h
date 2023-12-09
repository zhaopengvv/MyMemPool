#ifndef MEM_POOL_h
#define MEM_POOL_H
#include <cstdlib>

#define MEM_BLOCK_INIT_SIZE (8 * 1024)
#define MEM_BLOCK_MAX_SIZE  (8 * 1024 * 1024)
#define MEM_FREELISTS_NUM 11
#define MEM_MINBITS 3 /* smallest chunk size is 8 bytes */
#define MEM_SIZE_ALIGN 8 /* smallest chunk size is 8 bytes */
#define MEM_CHUNK_LIMIT (1 << (MEM_FREELISTS_NUM - 1 + MEM_MINBITS))

#define MEM_BLOCK_HEAD_SIZE sizeof(MemBlockHead)
#define MEM_CHUNK_HEAD_SIZE sizeof(MemChunkHead)

struct MemBlockHead {
    MemBlockHead *prev;
    MemBlockHead *next;
    char *freePtr;  /* start of free sapce in this block */
    char *endptr;   /* end of space in this block */
};

struct MemChunkHead {
    size_t size;
    union {
        void *free_list_link;
        void *ptr;
    };
};

#endif
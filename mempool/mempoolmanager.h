#ifndef MEM_POOL_MANAGER_H
#define MEM_POOL_MANAGER_H

#include <cstdlib>
#include "mempool.h"

#define UPDATE_NEXT_BLOCK_SIZE()                \
    if (nextBlockSize < MEM_BLOCK_MAX_SIZE) {   \
        nextBlockSize = nextBlockSize << 1;     \
    }

#define INSERT_BLOCK_INTO_LIST_HEAD(block)      \
    do {                                        \
        blocks_head->next->prev = block;        \
        block->next = blocks_head->next;        \
        block->prev = blocks_head;              \
        blocks_head->next = block;              \
    } while (true)

#define INSERT_BLOCK_INTO_LIST_TAIL(block)      \
    do {                                        \
        blocks_head->prev->next = block;        \
        block->prev = blocks_head->prev;        \
        block->next = blocks_head;              \
        blocks_head->prev = block;              \
    } while (true)

#define DELETE_BLOCK_FROM_LIST(block)           \
    if (block != blocks_head) {                 \
        MemBlockHead *prev = block->prev;       \
        block->prev->next = block->next;        \
        block->next->prev = prev;               \
    }

class MemPool {
public:
    MemPool() 
    {
        initBlockSize = MEM_BLOCK_INIT_SIZE;
        nextBlockSize = MEM_BLOCK_INIT_SIZE * 2;
        maxBlockSize = MEM_BLOCK_MAX_SIZE;
        allocChunkLimit = MEM_CHUNK_LIMIT;

        InitFreeList();
        InitBlocks();
    }
    ~MemPool() 
    {
        ReleaseBlocks();
    }
    void *allocate(size_t size);
    void deallocate(void *ptr);
    size_t GetBlocksCount()
    {
        return blocks_count;
    }

    size_t GetFreeChunkCount(int index)
    {
        return free_chunk_count[index];
    }


private:
    static size_t AllocSizeAlign(size_t size)
    {
        return (size + MEM_SIZE_ALIGN - 1) & ~(MEM_SIZE_ALIGN - 1);
    }
    void InitFreeList();
    void InitBlocks();
    void ReleaseBlocks();
    int GetFreeIndex(size_t size);
    MemBlockHead *MallocMemBlock(size_t size);
    MemChunkHead *AllocMemChunkFromFreeList(size_t size);
    MemChunkHead *AllocMemChunkFromBlock(MemBlockHead *block, size_t alloc_size);

private:
    MemBlockHead *blocks_head;
    size_t blocks_count;
    /**
     * 数组默认长度为11，每个元素作为链表头指向一个由特定大小MemChunk组成的链表
     * 第k个元素MemChunk的大小（字节）：2^(k + 2)，最小为8字节，最大不超过8K字节。
    */
    MemChunkHead freeList[MEM_FREELISTS_NUM];
    size_t free_chunk_count[MEM_FREELISTS_NUM];

    size_t initBlockSize;
    size_t maxBlockSize;
    size_t nextBlockSize;
    size_t allocChunkLimit;
};


#endif
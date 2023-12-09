#include <malloc.h>
#include<cstring>
#include "mempoolmanager.h"

void MemPool::InitFreeList()
{
    for (int i = 0; i < MEM_FREELISTS_NUM; i++) {
        freeList[i].ptr = nullptr;
        freeList[i].size = 1 << (i + MEM_MINBITS);
    }
}

void MemPool::InitBlocks()
{
    blocks_head = MallocMemBlock(0);
    MemBlockHead *block = MallocMemBlock(initBlockSize);
    blocks_head->next = block;
    blocks_head->prev = block;
    block->next = blocks_head;
    block->prev = blocks_head;
}

void MemPool::ReleaseBlocks()
{
    if (blocks_head == nullptr) {
        return;
    }

    MemBlockHead *block = nullptr;
    while (blocks_head->next != blocks_head) {
        block = blocks_head->next;
        DELETE_BLOCK_FROM_LIST(block);
        block->endptr = nullptr;
        block->freePtr = nullptr;
        block->next = nullptr;
        block->prev = nullptr;

        free(reinterpret_cast<void *>(block));
    }

    blocks_head->prev = nullptr;
    blocks_head->next = nullptr;
    free(reinterpret_cast<void *>(blocks_head));
}

/**
 *  idx     0   1   2   3   4   5 ...     
 *  size    8   16  32  64  128 .....
 *  eg. size:18
 *  (18 + 7) / 8 - 1 = 3 -1 = 2
*/
int MemPool::GetFreeIndex(size_t size)
{
    return (size + MEM_SIZE_ALIGN -1) / MEM_SIZE_ALIGN - 1; 
}

MemBlockHead *MemPool::MallocMemBlock(size_t size)
{
    size_t malloc_size = MEM_BLOCK_HEAD_SIZE + size;
    char *data = (char *)malloc(malloc_size);
    MemBlockHead *block = reinterpret_cast<MemBlockHead *>(data);
    if (size > 0) {
        block->freePtr = data + MEM_BLOCK_HEAD_SIZE;
        block->endptr = block->freePtr + size - 1;
    } else {
        block->freePtr = nullptr;
        block->endptr = nullptr;
    }
    block->next = NULL;
    block->prev = NULL;

    return block;
}

MemChunkHead *MemPool::AllocMemChunkFromFreeList(size_t size)
{
    int index = GetFreeIndex(size);
    MemChunkHead &list_header = freeList[index];
    if (list_header.ptr != nullptr) {
        char *chunk_body =  reinterpret_cast<char *>(list_header.ptr);
        MemChunkHead *chunk = reinterpret_cast<MemChunkHead *>(chunk_body - MEM_CHUNK_HEAD_SIZE);
        list_header.ptr = chunk->ptr;
        chunk->ptr = chunk_body;
        return chunk;
    }
    return nullptr;
}

MemChunkHead *MemPool::AllocMemChunkFromBlock(MemBlockHead *block, size_t alloc_size)
{
    size_t available_szie = block->endptr - block->freePtr + 1;
    if (available_szie >= alloc_size) {
        MemChunkHead *chunk = reinterpret_cast<MemChunkHead *>(block->freePtr);
        chunk->ptr = block->freePtr + MEM_CHUNK_HEAD_SIZE;
        chunk->size = alloc_size;
        block->freePtr = block->freePtr + MEM_CHUNK_HEAD_SIZE + alloc_size;
        return chunk;
    }
    return nullptr;
}

void *MemPool::allocate(size_t size)
{
    if (size > MEM_CHUNK_LIMIT) {
        // 申请一个独立的内存块
        size_t alloc_size = AllocSizeAlign(size);
        MemBlockHead *block = MallocMemBlock(alloc_size);
        INSERT_BLOCK_INTO_LIST_TAIL(block);
        MemChunkHead *ret = AllocMemChunkFromBlock(block, alloc_size);
        return ret->ptr;
    }

    // 查找空闲空间链表
    MemChunkHead *chunk = AllocMemChunkFromFreeList(size);
    if (chunk != nullptr) {
        return chunk->ptr;
    }

    // 查找合适的内存块，分配内存片
    MemBlockHead *block = blocks_head->next;
    size_t alloc_size = AllocSizeAlign(size);
    while (block != nullptr) {
        MemChunkHead *ret = AllocMemChunkFromBlock(block, alloc_size);
        if (ret != nullptr) {
            return ret->ptr;
        }
        block = block->next;
    }

    // 申请新的内存块
    block = MallocMemBlock(nextBlockSize);
    INSERT_BLOCK_INTO_LIST_HEAD(block);
    UPDATE_NEXT_BLOCK_SIZE();
    // 从内存块分配内存片
    MemChunkHead *ret = AllocMemChunkFromBlock(block, alloc_size);
    return ret->ptr;
}

void MemPool::deallocate(void *ptr)
{
    // 1. 获取memchunk
    char *chunk_body = reinterpret_cast<char *>(ptr);
    MemChunkHead *chunk = reinterpret_cast<MemChunkHead *>(chunk_body - MEM_CHUNK_HEAD_SIZE);
    size_t chunk_size = chunk->size;

    // 2. 释放独立内存块
    if (chunk_size > MEM_CHUNK_LIMIT) {
        MemBlockHead *block = reinterpret_cast<MemBlockHead *>(chunk_body - MEM_CHUNK_HEAD_SIZE - MEM_BLOCK_HEAD_SIZE);
        DELETE_BLOCK_FROM_LIST(block);
        block->endptr = nullptr;
        block->freePtr = nullptr;
        block->next = nullptr;
        block->prev = nullptr;
        free(reinterpret_cast<void *>(block));
        return;
    }
    
    // 2. 清除内存数据
    (void *)memset(ptr, 0, chunk_size);

    // 3. 插入freeList
    int index = GetFreeIndex(chunk_size);
    MemChunkHead &list_header = freeList[index];
    char *first_chunk_body = reinterpret_cast<char *>(list_header.ptr);
    MemChunkHead *first_chunk = reinterpret_cast<MemChunkHead *>(first_chunk_body - MEM_CHUNK_HEAD_SIZE);
    list_header.ptr = chunk->ptr;
    chunk->ptr = first_chunk->ptr;
}
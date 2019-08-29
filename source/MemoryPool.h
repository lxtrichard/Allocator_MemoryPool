#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H
#include <iostream>
using namespace std;
struct block
{
    struct block *prev = nullptr;
    struct block *next = nullptr;
};
class MemoryPool {
public:
    MemoryPool();
    MemoryPool(size_t num,size_t size);
    ~MemoryPool();
    void* allocate(size_t size);
    MemoryPool* deallocate(MemoryPool* cursor,MemoryPool* end,void* ptr,size_t size);
    block* free_Header = nullptr;
    block* alloc_Header = nullptr;
    void* Pool_Header = nullptr;
    size_t block_size;//每个块的大小
    size_t block_num;//块的数目
    size_t pool_size;//(block的大小 + 维护block结构体大小) * block 数目
    int id = 0;
    MemoryPool* Next;
    MemoryPool* Prev;
};

class MemoryPool_List 
{
public:
	MemoryPool* free_pool = nullptr;
	MemoryPool* first_pool = nullptr;
	MemoryPool_List();
	void* allocate(size_t size);
	void deallocate(void* ptr, size_t size);
};
#endif //MEMORYPOOL_H

#include "MemoryPool.h"
#define k 1024//内存分配单元的大小
#define BlockNum 64//默认内存分配单元个数
#define BlockSize 4 * k//默认的内存分配单元大小
MemoryPool::MemoryPool(size_t blocknumber, size_t blocksize)//根据输入的参数创建memorypool
{
    if(blocknumber < BlockNum) block_num = BlockNum;
    else block_num = blocknumber;//最小为默认分配单元个数
    if(blocksize < BlockSize )block_size = BlockSize;
    else block_size = blocksize;//最小为默认大小
    pool_size = block_num * (block_size + sizeof(block));//memorypool的大小还要包含维持结构体所需空间
    Pool_Header = operator new(pool_size);//pool_header指针指向当前memorypool
    Next = nullptr;  // 创建成功
    for (int i = 0; i < block_num; i++) 
	{
		block *b = reinterpret_cast<block*>(static_cast<char*>(Pool_Header) + i * (block_size + sizeof(block)));
        b->prev = nullptr;
        b->next = free_Header;
        if (free_Header != nullptr)  
		 free_Header->prev = b;
		free_Header = b;//free_header指向未分配的第一个内存块
     }//由大到小降序生成一系列memory pool并等待分配
}
MemoryPool::MemoryPool()
 { MemoryPool(BlockNum, BlockSize); }//构造函数，创建一个默认大小的memorypool
void* MemoryPool::allocate(size_t sz) 
{
    if( sz > block_size)
        return ::operator new(sz);// 大于blocksize，直接从系统分配
    else
	{
        block* b = free_Header;
        free_Header = b->next;
        if (free_Header != nullptr)
            free_Header->prev = nullptr;//free_header移向下一个内存block组
        b->next = alloc_Header;//分配空间
        if (alloc_Header != nullptr)
            alloc_Header->prev = b;
        alloc_Header = b;//指明allock_header
        return static_cast<void *>(reinterpret_cast<char*>(b) + sizeof(block));//返回创建后空间的头指针
    }
}
MemoryPool* MemoryPool::deallocate(MemoryPool* firstpool,MemoryPool* freepool,void* ptr,size_t sz) 
{
    if (firstpool != nullptr)//指针在内存池中
	{
        void *Pool_Header = firstpool->Pool_Header;
        if (Pool_Header < ptr && ptr < (void *) ((char *) Pool_Header + firstpool->pool_size))//指针在内存池中
		{
            block* curr = reinterpret_cast<block*>(static_cast<char *>(ptr) - sizeof(block));
            if (curr != firstpool->alloc_Header) 
			{
                curr->prev->next = curr->next;
                if (curr->next != nullptr)
					curr->next->prev = curr->prev;
            }
			else 
			{
                firstpool->alloc_Header = curr->next;
               if (curr->next != nullptr)
				   curr->next->prev = nullptr;
            }
            curr->next = firstpool->free_Header;// 加到空块链表中
            if (firstpool->free_Header != nullptr)
                firstpool->free_Header->prev = curr;
            firstpool->free_Header = curr;
            return firstpool; //已经移除指针,返回当前内存池的首地址
        }
		else if( firstpool != freepool && firstpool->Next!= nullptr) //还有下一块,查找下一块
            return deallocate(firstpool->Next, freepool, ptr, sz);
        else //没有空的内存池了
            return nullptr;
    }
    return nullptr;
}
MemoryPool::~MemoryPool() 
{
    if (Pool_Header != nullptr)
        ::operator delete(Pool_Header);
}
MemoryPool_List::MemoryPool_List()
{
	MemoryPool* p = new MemoryPool();
	free_pool = p;
	first_pool = p;//新建一个memory_pool
}
void* MemoryPool_List::allocate(size_t size)
{
	if (free_pool->free_Header == nullptr)//当前内存池满了
	{
		if (free_pool->Next == nullptr)//下一个内存池还没分配好
		{
			free_pool->Next = new MemoryPool();
			free_pool->Next->id = free_pool->id + 1;
		}
		MemoryPool* mp = free_pool;//下一个内存池已经分配好了，移除空闲内存池链表头
		free_pool = free_pool->Next;
		free_pool->Prev = mp;
	}
	return free_pool->allocate(size);
}
void MemoryPool_List::deallocate(void* p, size_t size)
{
	MemoryPool *cursor = first_pool->deallocate(first_pool, free_pool, p, size);//从第一个pool中开始查找
	if (cursor == nullptr)//不在内存池中
	{
		operator delete(p);
		return;
	}
	else if (cursor->alloc_Header == nullptr) 
	{
		if (cursor != free_pool) 
		{
			if (cursor != first_pool)//把当前的空间挤出
			{
				cursor->Prev->Next = cursor->Next;
				cursor->Next->Prev = cursor->Prev;
			}
			else 
			{
				first_pool = first_pool->Next;
				first_pool->Prev = nullptr;
			}
			cursor->Next = free_pool->Next;
			if (free_pool->Next != nullptr) 
				free_pool->Next->Prev = cursor;
			cursor->Prev = free_pool;
			free_pool->Next = cursor;
		}
	}
}

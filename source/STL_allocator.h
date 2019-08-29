#include <iostream>
#include <limits>
#include "MemoryPool.h"
template <typename T>
class Allocator {
public:
	typedef T value_type;
	typedef  value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	Allocator() {};
	~Allocator() {};
	Allocator(Allocator const&) {};
	pointer allocate(size_type tmp);
	void deallocate(pointer p, size_type n);
	pointer address(reference r) { return &r; };//��ַ��Ϣ
	const_pointer address(const_reference r) { return &r; };
	size_type max_size();//���ش�С
	void construct(pointer p, const T& t);
	void destroy(pointer p);
	bool operator==(Allocator const&);
	bool operator!=(Allocator const& a);
	template<typename U>
	Allocator(Allocator<U> const&){};//�ڴ����
	template<typename U>//convert an allocator<T> to allocator<U>
	struct rebind { typedef Allocator<U> other; };
private:
	static MemoryPool_List Mempool;
};   
template <typename T>
MemoryPool_List Allocator<T>::Mempool;//���ش�С
template<typename T>
size_t Allocator<T>::max_size()
{
	return numeric_limits<size_type>::max() / sizeof(T);
};
template <typename T>
T* Allocator<T>::allocate(size_type tmp)//�ڴ����
{
	return reinterpret_cast<pointer>(Mempool.allocate(tmp * sizeof(T)));
}
template <typename T>
void Allocator<T>::deallocate(pointer p, size_type n)
{
	Mempool.deallocate(p, n);
}
template <typename T>
void Allocator<T>::construct(pointer p, const T &t)//���캯��
{
	new (p) T(t);
}
template <typename T>
void Allocator<T>::destroy(pointer p)
{
	p->~T();
}
template <typename T>
bool Allocator<T>::operator==(Allocator const &)
{
	return true;
}
template <typename T>
bool Allocator<T>::operator!=(Allocator const &a)
{
	return !operator==(a);
}
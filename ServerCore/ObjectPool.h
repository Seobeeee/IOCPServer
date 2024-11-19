#pragma once

#include "Types.h"
#include "MemoryPool.h"

// 추후에는 클래스 별로 원하는 객체를 메모리풀로 사용한다거나 할 수 있게 구현하면 좋을 듯.
template<typename Type>
class ObjectPool
{
public:
    template<typename... Args>
    static Type* Pop(Args&&... args)
    {
#ifdef _STOMP
        MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(s_allocSize));
        Type * memory = static_cast<Type*>(MemoryHeader::AttachHeader(ptr, s_allocSize));
#else
        Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(s_pool.Pop(), s_allocSize));
#endif
        new(memory)Type(forward<Args>(args)...);
        return memory;
    }

    static void Push(Type* obj)
    {
        obj->~Type();
#ifdef _STOMP
        StompAllocator::Release(MemoryHeader::DetachHeader(obj));
#else
        s_pool.Push(MemoryHeader::DetachHeader(obj));
#endif
    }

    static shared_ptr<Type> MakeShared()
    {
        shared_ptr<Type> ptr = { Pop(), Push };
        return ptr;
    }

private:
    static int32        s_allocSize;
    static MemoryPool   s_pool;
};

template<typename Type>
int32 ObjectPool<Type>::s_allocSize = sizeof(Type) + sizeof(MemoryHeader);

template<typename Type>
MemoryPool ObjectPool<Type>::s_pool{ s_allocSize };

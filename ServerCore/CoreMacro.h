#pragma once

#define OUT

#define USE_MANY_LOCK(count)	Lock _locks[count];
#define USE_LOCK				USE_MANY_LOCK(1)
#define READ_LOCK_IDX(idx)		ReadLockGuard readLockGuard_##idx(_locks[idx], typeid(this).name());
#define READ_LOCK				READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx)		WriteLockGuard writeLockGuard_##idx(_locks[idx], typeid(this).name());
#define WRITE_LOCK				WRITE_LOCK_IDX(0)


#ifdef _DEBUG
#define newAlloc(size)			BaseAllocator::Alloc(size)
#define newRelease(ptr)			BaseAllocator::Release(ptr)
#else
#define newAlloc(size)			BaseAllocator::Alloc(size)
#define newRelease(ptr)			BaseAllocator::Release(ptr)
#endif


// 인위적인 크래시를 내고 싶을 떄 사용할 매크로.
#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

// 특정한 조건을 확인하여 크래시를 내고싶을 떄 사용
#define ASSERT_CRASH(expr)					\
{											\
	if(!(expr))								\
	{										\
		CRASH("ASSERT_CRASH");				\
		__analysis_assume(expr);			\
	}										\
}											\

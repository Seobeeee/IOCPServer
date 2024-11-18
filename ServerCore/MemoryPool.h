#pragma once

struct MemoryHeader
{
    // [MemoryHeader][Data]
    MemoryHeader(int32 size) : allocSize(size) {}

    static void* AttachHeader(MemoryHeader* header, int32 size)
    {
        // placement New를 사용하여 원하는 위치에 동적할당 진행.
        new(header)MemoryHeader(size);
        return reinterpret_cast<void*>(++header);
    }

    static MemoryHeader* DetachHeader(void* ptr)
    {
        MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
        return header;
    }

    int32 allocSize;
};

class MemoryPool
{
public:
    MemoryPool(int32 allocSize) : _allocSize(allocSize) {}
    ~MemoryPool();

    void            Push(MemoryHeader* ptr);
    MemoryHeader*   Pop();

protected:

private:
    int32                   _allocSize = 0;
    atomic<int32>           _allocCount = 0;

    USE_LOCK;
    queue<MemoryHeader*>    _queue;
};


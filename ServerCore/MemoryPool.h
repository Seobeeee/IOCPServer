#pragma once

enum 
{
    SLIST_ALIGNMENT = 16
};

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
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

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
    MemoryPool(int32 allocSize);
    ~MemoryPool();

    void            Push(MemoryHeader* ptr);
    MemoryHeader*   Pop();

protected:

private:
    SLIST_HEADER            _header;
    int32                   _allocSize = 0;
    atomic<int32>           _allocCount = 0;
};


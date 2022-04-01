#include <types.h>

// This is terrible, mod was rushed lol

u8 heap[0x80];
u8 * nextFree = heap;

void * malloc(std::size_t size)
{
    void * ret = (void *) nextFree;
    nextFree += size;
    return ret;
}

void * operator new(std::size_t size)
{
    return malloc(size);
}

void * operator new[](std::size_t size)
{
    return malloc(size);
}



#include "mempoolmanager.h"

int main()
{
    MemPoolManager mem;
    void *zone = mem.MemAlloc(100);
    return 0;
}


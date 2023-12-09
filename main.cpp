#include "mempoolmanager.h"

int main()
{
    MemPool mem;
    void *zone = mem.allocate(100);
    return 0;
}


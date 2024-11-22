#ifndef IALLOC_H
#define IALLOC_H

#include "interface/Common.h"

#include <iostream>
#include <memory>

// 用来维护每个内存池的分配
class IAlloc;
typedef std::shared_ptr<IAlloc> IAllocPtr;

class IAlloc
{

public:

    virtual void free(void* const pPointer, size_t size) = 0;

    virtual void* malloc(size_t size) = 0;

    static IAllocPtr instance(void);

};

#endif
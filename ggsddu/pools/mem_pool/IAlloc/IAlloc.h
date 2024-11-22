#ifndef IALLOC_H
#define IALLOC_H

#include <iostream>
#include <memory>

// 内存池管理模块接口       单例
/*
    example:

    int* a = static_cast<int*>(IAlloc::instance()->malloc(sizeof(int)));
    *a = 42;



 */

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
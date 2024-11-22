#include "interface/IObject.h"
#include "interface/IAlloc.h"

#include <iostream>

void* IObject::operator new(size_t size)
{
    return (IAlloc::instance()->malloc(size));
}

void IObject::operator delete(void* p, size_t size)
{
    IAlloc::instance()->free(p, size);
}

IObject::IObject(void)
:std::enable_shared_from_this<IObject>()
{
}

IObject::~IObject(void)
{
}
#include "IAlloc.h"
#include "CAlloc.h"

#include <memory>


IAllocPtr IAlloc::instance() 
{
    return std::dynamic_pointer_cast<IAlloc>(CAlloc::instance());
}
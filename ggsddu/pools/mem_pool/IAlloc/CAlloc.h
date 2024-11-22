
#ifndef CALLOC_H__
#define CALLOC_H__ 

#include "IAlloc.h"

#include <map>
#include <boost/pool/pool.hpp>
#include <boost/pool/poolfwd.hpp>
#include <memory>
#include <iostream>
#include <mutex>


// 用来维护每个内存池的分配

class CAlloc;
typedef std::shared_ptr<CAlloc> CAllocPtr;

class CAlloc:
    virtual public IAlloc
{

public:

    virtual ~CAlloc(void);

    virtual void free(void* const pPointer, size_t size);

    virtual void* malloc(size_t size);

    static const CAllocPtr& instance(void);

private:

    CAlloc(void);

    std::map<size_t, boost::pool<>*> m_memPools;
    
    std::map<void*, size_t> m_ptr2size;

    std::mutex m_mutex;
};


#endif
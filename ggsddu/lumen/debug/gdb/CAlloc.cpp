

#include "CAlloc.h"

#include <iostream>


CAlloc::CAlloc(void)
{

}

CAlloc::~CAlloc(void)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    boost::pool<>* p = NULL;
    for(std::map<size_t, boost::pool<>*>::iterator pIter = m_memPools.begin(); pIter != m_memPools.end(); ++pIter)
    {
        p = pIter->second;

        if (p != NULL)
        {
            void(p->purge_memory());
            delete p;
        }
    }
    
    m_memPools.clear();
}

void* CAlloc::malloc(size_t size)
{
    printf("start \n");
    void* result = NULL;

    if (size != 0U)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        std::map<size_t, boost::pool<>*>::iterator pPool = m_memPools.find(size);

        if (pPool == m_memPools.end())
        {
            boost::pool<>* const p = new boost::pool<>(size);

            if (p != NULL)
            {
                void(m_memPools.insert(std::make_pair(size, p)));
                void* const pPointer = p->malloc();

                if (pPointer == NULL)
                {
                    std::cout << __FUNCTION__ << "failure malloc object size = " << size << std::endl;
                }

                result = pPointer;
            }
            else
            {
                std::cout << __FUNCTION__ << "faulure new boost::pool<> size = " << size << std::endl;
                result = NULL;
            }
        }
        else
        {
            boost::pool<>* const p = pPool->second;
            void* const pPointer = p->malloc();

            if (pPointer == NULL)
            {
                std::cout << __FUNCTION__ << "failure malloc object size = " << size << std::endl;
            }
            
            result = pPointer;
        }
    }
    else
    {
        std::cout << __FUNCTION__ << "CAlloc::malloc() can not malloc object size = 0 !!" << std::endl;
        result = NULL;
    }

    return result;
}

void CAlloc::free(void* const pPointer, size_t size)
{
    if ((pPointer != NULL) && (size != 0))
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        std::map<size_t, boost::pool<>*>::iterator pPool = m_memPools.find(size);

        if (pPool != m_memPools.end())
        {
            boost::pool<>* const p = pPool->second;
            p->free(pPointer);
        }
        else
        {
            std::cout << "delete object = " << pPointer << ", size = " << size << " failure" << std::endl;
        }
    }
    else
    {
        std::cout << __FUNCTION__ << "CAlloc::malloc() can not free object size = 0 or pPointer = NULL !!" << std::endl;
    }
}

const CAllocPtr& CAlloc::instance(void)
{
    const static CAllocPtr s_Alloc(new CAlloc);
    return (s_Alloc);
}
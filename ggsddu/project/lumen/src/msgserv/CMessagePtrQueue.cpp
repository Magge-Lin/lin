
#include "interface/CMessagePtrQueue.h"
#include "interface/CMessage.h"


CMessagePtrQueue::CMessagePtrQueue(void)
: IObject()
{
}

CMessagePtrQueue::~CMessagePtrQueue(void)
{
}

bool CMessagePtrQueue::postMessage(const CMessagePtr& msg)
{
    bool result = false;

    if (msg)
    {
        std::lock_guard<std::recursive_mutex> lock(m_rMutex);
        msg->reset();
        m_messagePtrQueues.push_back(msg);

        result = true;
    }

    return result;
}

bool CMessagePtrQueue::getMessage(CMessagePtr& msg)
{
    bool bRet = false;
    std::lock_guard<std::recursive_mutex> lock(m_rMutex);

    if (m_messagePtrQueues.empty() == false)
    {
        msg = m_messagePtrQueues.front();
        m_messagePtrQueues.pop_front();
        bRet = true;
    }

    return (bRet);
}

void CMessagePtrQueue::reset(void)
{
    std::lock_guard<std::recursive_mutex> lock(m_rMutex);
    m_messagePtrQueues.clear();
}

int32_t CMessagePtrQueue::getSize(void) const
{
    return (m_messagePtrQueues.size());
}

bool CMessagePtrQueue::empty(void) const
{
    return (m_messagePtrQueues.empty());
}

CMessagePtrQueue& CMessagePtrQueue::operator= (const CMessagePtrQueue& right)
{
    if (this != &right)
    {
        m_messagePtrQueues = right.m_messagePtrQueues;
    }

    return *this;
}
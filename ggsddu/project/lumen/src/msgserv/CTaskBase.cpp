#include "interface/CTaskBase.h"


#include <thread>

CTaskBase::CTaskBase()
: IObject()
, m_queue()
, m_isRunning(false)
, m_stopRequested(false)
{
    m_queue.reset();
}

CTaskBase::~CTaskBase()
{
    stop();
}

int32_t CTaskBase::start()
{
    int32_t nRet = 0;

    m_isRunning.store(true);

    // 开启线程
    m_workerThread = std::thread(&CTaskBase::taskFunc, this);

    return nRet;
}

void CTaskBase::stop()
{
    m_stopRequested.store(true);

    if (m_workerThread.joinable()) 
    {
        m_workerThread.join();
    }

    m_isRunning.store(false);
}

void CTaskBase::taskFunc(CTaskBase* const pThis)
{
    if (pThis != NULL)
    {
        (void)pThis->run();
    }
}

bool CTaskBase::postMessage(const CMessagePtr& msg)
{
    return m_queue.postMessage(msg);
}

bool CTaskBase::getMessage(CMessagePtr& msg)
{
    return m_queue.getMessage(msg);
}

int32_t CTaskBase::testCancel(void)  const
{
    return m_isRunning.load();
}

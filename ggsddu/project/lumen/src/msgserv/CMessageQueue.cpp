#include "interface/CMessageQueue.h"
#include "interface/IAlloc.h"


#include <iostream>
#include <thread>
#include <chrono>
#include <boost/lexical_cast.hpp>
#include <arpa/inet.h>

//class CMessageQueue
CMessageQueue::CMessageQueue()
: IObject()
, m_maxMessageSize(8096)
, m_readPtr(0)
, m_writePtr(0)
, m_mallocMode(SYSTEM_MALLOC)
{
    m_MessageBuffer = new int8_t[m_maxMessageSize];
}

CMessageQueue::CMessageQueue(const int32_t maxMessageSize, const int32_t mallocMode)
: IObject()
, m_maxMessageSize(maxMessageSize)
, m_readPtr(0)
, m_writePtr(0)
, m_mallocMode(mallocMode)
{
    if (m_maxMessageSize > 0)
    {
        if (m_mallocMode == SYSTEM_MALLOC)
        {
            m_MessageBuffer = new int8_t[m_maxMessageSize];
        }
        else
        {
            m_MessageBuffer = static_cast<int8_t*>(IAlloc::instance()->malloc(m_maxMessageSize));
        }
    }
    else
    {
        m_MessageBuffer = NULL;
    }
}

CMessageQueue::~CMessageQueue(void)
{
    if (m_MessageBuffer != NULL)
    {
        if (m_mallocMode == SYSTEM_MALLOC)
        {
            delete[] m_MessageBuffer;
        }
        else
        {
            IAlloc::instance()->free(m_MessageBuffer, m_maxMessageSize);
        }
    }
}

bool CMessageQueue::postMessage(const CMessagePtr& msg)
{
    bool bRet = true;

    if (m_MessageBuffer == NULL)
    {
        bRet = false;
    }
    else
    {
        const int32_t   len = static_cast<int32_t>(msg->getLength());
        int32_t nextWritePtr = m_writePtr;

        for (int32_t i = 0; i < len; i++)
        {
            nextWritePtr ++;
            nextWritePtr = nextWritePtr % m_maxMessageSize;

            if (m_readPtr == nextWritePtr)  //
            {
                int32_t count = 0;

                for (count = 0; count < 10; count++)        //waiting  read
                {
                    if (m_readPtr == nextWritePtr)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));     //10ms
                    }
                    else
                    {
                        break;      // may wirte
                    }
                }

                if (count >= 10)    //can not write
                {
                    bRet = false;
                    break;
                }
            }
        }

        if ((bRet == true) && (len > MESSAGE_MAX_SIZE))
        {
            std::cout << __FUNCTION__ << " message length is too ods_long32: " << len << std::endl;
            bRet = false;
        }

        //put msg buf
        if (bRet == true)
        {
            const int8_t* const buf = msg->getBuffer();

            for (int32_t i = 0; i < len; i++)
            {
                m_MessageBuffer[m_writePtr] = buf[i];
                m_writePtr ++;
                m_writePtr = m_writePtr % m_maxMessageSize;
            }
        }
    }

    return bRet;
}

bool CMessageQueue::getMessage(const CMessagePtr& msg)
{
    bool bRet = true;

    if ((m_MessageBuffer == NULL) || (m_readPtr == m_writePtr))
    {
        bRet = false;
    }
    else
    {
        bRet = resolveMessage(msg);
    }

    return bRet;
}

bool CMessageQueue::postBuffer(const int8_t* const buf, const size_t len)
{
    bool bRet = true;

    if (m_MessageBuffer == NULL)
    {
        bRet = false;
    }
    else
    {
        int32_t nextWritePtr = m_writePtr;
        int32_t count = 0;

        for (size_t i = 0U; i <= len; i++)
        {
            nextWritePtr ++;
            nextWritePtr = nextWritePtr % m_maxMessageSize;

            if (m_readPtr == nextWritePtr)  //
            {
                for (count = 0; count < 100; count++)//waiting  read
                {
                    if (m_readPtr == nextWritePtr)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));     //10ms
                    }
                    else
                    {
                        break;      // may wirte
                    }
                }

                if (count >= 100)       //can not write
                {
                    std::cout <<  __FUNCTION__<< ":put messge into message queue failure. " << len << std::endl;
                    bRet = false;
                    break;
                }
            }
        }

        if (bRet == true)
        {
            //put msg buf
            for (size_t i = 0U; i < len; i++)
            {
                m_MessageBuffer[m_writePtr] = buf[i];
                m_writePtr ++;
                m_writePtr = m_writePtr % m_maxMessageSize;
            }
        }
    }

    return bRet;
}

void CMessageQueue::reset(void)
{
    m_readPtr = 0;
    m_writePtr = 0;
}

int32_t CMessageQueue::getReadPtr(void) const
{
    return m_readPtr;
}

int32_t CMessageQueue::getWritePtr(void) const
{
    return m_writePtr;
}

bool CMessageQueue::getBuffer(int8_t* buf, size_t& len)
{
    bool bRet = true;

    if ((m_MessageBuffer == NULL) || (m_readPtr == m_writePtr))
    {
        bRet = false;
    }
    else
    {
        int32_t tempReadPtr = m_readPtr;
        int32_t realLen = 0;
        const int32_t inputLen = boost::lexical_cast<int32_t>(len);

        for (int32_t i = 0; i < inputLen; i++)
        {
            buf[i] = m_MessageBuffer[tempReadPtr];
            tempReadPtr ++;
            tempReadPtr = tempReadPtr % m_maxMessageSize;
            realLen++;

            if (tempReadPtr == m_writePtr)
            {
                m_readPtr = tempReadPtr;
                len = realLen;
                break;
            }
        }
    }

    return bRet;
}

bool CMessageQueue :: resolveMessage(const CMessagePtr& msg)
{
    if (m_MessageBuffer == NULL)
    {
        return false;
    }

    int32_t tempReadPtr1 = m_readPtr;
    int32_t tempReadPtr2 = tempReadPtr1;
    int32_t pos[4] = {0, 0, 0, 0};

    for (int32_t i = 0; i < 4 ; i++)
    {
        if (tempReadPtr1 == m_writePtr)
        {
            return false;
        }

        pos[i] = tempReadPtr1;
        tempReadPtr1 ++;
        tempReadPtr1 = tempReadPtr1 % m_maxMessageSize;
    }

    //search sync head
    while (!((m_MessageBuffer[pos[0]] == static_cast<int8_t>(0xeb)) && (m_MessageBuffer[pos[1]] == static_cast<int8_t>(0x90)) && \
             (m_MessageBuffer[pos[2]] == static_cast<int8_t>(0xeb)) && (m_MessageBuffer[pos[3]] == static_cast<int8_t>(0x90))))
    {
        tempReadPtr2 ++;
        tempReadPtr1 = tempReadPtr2 % m_maxMessageSize;
        tempReadPtr2 = tempReadPtr1;

        if (tempReadPtr1 == m_writePtr)
        {
            m_readPtr = tempReadPtr1;
            return false;
        }

        for (int32_t i = 0; i < 4 ; i++)
        {
            if (tempReadPtr1 == m_writePtr)
            {
                return false;
            }

            pos[i] = tempReadPtr1;
            tempReadPtr1 ++;
            tempReadPtr1 = tempReadPtr1 % m_maxMessageSize;
        }
    }

    //tempReadPtr2  is current head pos
    //tempReadPtr1  is length
    //find sync head
    int8_t szTemp[sizeof(uint16_t)];
    memset(szTemp, 0, sizeof(uint16_t));

    for (int32_t i = 0; i < sizeof(uint16_t); i++)          //len;
    {
        if (tempReadPtr1 == m_writePtr)
        {
            return false;
        }

        szTemp[i] = m_MessageBuffer[tempReadPtr1];
        tempReadPtr1 ++;
        tempReadPtr1 = tempReadPtr1 % m_maxMessageSize;
    }

    uint16_t msgLength = 0U;
    (void)memcpy(&msgLength, &(szTemp[0]), sizeof(uint16_t));

    if (msg->isNet() == true)
    {
        msgLength = ntohs(msgLength);
    }

    msgLength = msgLength - sizeof(uint16_t);           //sub self len
    //messageid

    for (int32_t i = 0; i < sizeof(uint16_t); i++)      //id;
    {
        if (tempReadPtr1 == m_writePtr)
        {
            return false;
        }

        szTemp[i] = m_MessageBuffer[tempReadPtr1];
        tempReadPtr1 ++;
        tempReadPtr1 = tempReadPtr1 % m_maxMessageSize;
    }

    uint16_t msgId = 0U;
    (void)memcpy(&msgId, &(szTemp[0]), sizeof(uint16_t));

    if (msg->isNet() == true)
    {
        msgId = ntohs(msgId);
    }

    msgLength = msgLength - sizeof(uint16_t);//sub id len
    //nodeid

    for (int32_t i = 0; i < sizeof(uint16_t); i++)//id;
    {
        if (tempReadPtr1 == m_writePtr)
        {
            return false;
        }

        szTemp[i] = m_MessageBuffer[tempReadPtr1];
        tempReadPtr1 ++;
        tempReadPtr1 = tempReadPtr1 % m_maxMessageSize;
    }

    uint16_t nodeId = 0U;
    (void)memcpy(&nodeId, &(szTemp[0]), sizeof(uint16_t));

    if (msg->isNet() == true)
    {
        nodeId = ntohs(nodeId);
    }

    msgLength = msgLength - sizeof(uint16_t);//sub id len
    //tempReadPtr1 is data
    int32_t dataPtr = tempReadPtr1;
    msg->initMessage(msgId, nodeId);

    for (uint16_t i = 0U; i < msgLength; i++)
    {
        if (dataPtr == m_writePtr)
        {
            return false;
        }

        *msg << static_cast<uint8_t>(m_MessageBuffer[dataPtr]);
        dataPtr ++;
        dataPtr = dataPtr % m_maxMessageSize;
    }

    (void)msg->getBuffer();
    msg->reset();
    m_readPtr = dataPtr;
    return true;
}

int32_t CMessageQueue::getSize(void) const
{
    int32_t count = 0;

    if (m_MessageBuffer != NULL)
    {
        int32_t tempReadPtr = m_readPtr;
        const int32_t tempWritePtr = m_writePtr;

        while (tempReadPtr != tempWritePtr)
        {
            tempReadPtr ++;
            tempReadPtr = tempReadPtr % m_maxMessageSize;
            count++;
        }
    }

    return count;
}

const int8_t* CMessageQueue::getMessageBuffer(void) const
{
    return m_MessageBuffer;
}

void CMessageQueue::resetReadPtr(void)
{
    m_readPtr = 0;
}

//class CDynamicMessageQueue
CDynamicMessageQueue::CDynamicMessageQueue(void)
: m_queueSize(8096)
, m_mallocMode(CMessageQueue::SYSTEM_MALLOC)
{
}
CDynamicMessageQueue::CDynamicMessageQueue(const int32_t queueSize, const int32_t mallocMode)
: m_queueSize(queueSize)
, m_mallocMode(mallocMode)
{
}

CDynamicMessageQueue::~CDynamicMessageQueue()
{
}

bool CDynamicMessageQueue::postMessage(const CMessagePtr& msg)
{
    bool bRet = true;

    if (m_msgQueues.empty())
    {
        const CMessageQueuePtr pMsgQueue(new CMessageQueue(m_queueSize, m_mallocMode));

        if (pMsgQueue == NULL)
        {
            bRet = false;
        }

        if (bRet == true)
        {
            bRet = pMsgQueue->postMessage(msg);
        }

        if (bRet == true)
        {
            m_msgQueues.push_back(pMsgQueue);
            m_msgBakQueues.push_back(pMsgQueue);
        }
    }
    else
    {
        const CMessageQueuePtr pMsgQueue = m_msgQueues.back();

        if (pMsgQueue == NULL)
        {
            bRet = false;
        }

        if (bRet == true)
        {
            if (pMsgQueue->postMessage(msg) == false)
            {
                const CMessageQueuePtr pMsgQueue2(new CMessageQueue(m_queueSize, m_mallocMode));

                if (pMsgQueue2->postMessage(msg) == false)
                {
                    bRet = false;
                }

                if (bRet == true)
                {
                    m_msgQueues.push_back(pMsgQueue2);
                    m_msgBakQueues.push_back(pMsgQueue2);
                }
            }
        }
    }

    return bRet;
}

bool CDynamicMessageQueue::getMessage(CMessagePtr& msg)
{
    bool bRet = true;

    if (m_msgQueues.empty())
    {
        bRet = false;
    }
    else
    {
        const CMessageQueuePtr pMsgQueue = m_msgQueues.front();

        if (pMsgQueue == NULL)
        {
            bRet = false;
        }

        if (bRet == true)
        {
            if (pMsgQueue->getMessage(msg) == false)
            {
                m_msgQueues.pop_front();
                //delete pMsgQueue;
                bRet = getMessage(msg);
            }
        }
    }

    return bRet;
}

void CDynamicMessageQueue::recover(void)
{
    m_msgQueues = m_msgBakQueues;

    for (std::deque< CMessageQueuePtr >::iterator pIter = m_msgQueues.begin(); pIter != m_msgQueues.end(); pIter++)
    {
        (*pIter)->resetReadPtr();
    }
}

void CDynamicMessageQueue::clear(void)
{
    m_msgQueues.clear();
    m_msgBakQueues.clear();
}

bool CDynamicMessageQueue::empty(void) const
{
    return m_msgQueues.empty();
}


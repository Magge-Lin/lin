#ifndef CMESSAGEPTR_QUEUE_H
#define CMESSAGEPTR_QUEUE_H


#include "interface/CMessage.h"
#include "interface/IObject.h"

#include <iostream>
#include <deque>
#include <mutex>


/*
    该类用来动态产生消息队列。
    如果一个队列满了，则产生另一个队列来进行使用。
*/


class CMessagePtrQueue
    : virtual public IObject
{

public:

    // CMessagePtrQueue的构造函数
    CMessagePtrQueue(void);

    // CMessagePtrQueue的析构函数
    virtual ~CMessagePtrQueue(void);

    // 发送消息到队列中
    bool postMessage(const CMessagePtr& msg);

    // 从队列中提取消息
    bool getMessage(CMessagePtr& msg);

    // 复位消息队列
    void reset(void);

    // 获得消息队列的尺寸
    int32_t getSize(void) const;

    // 判断消息队列是否为空
    bool empty(void) const;

    // 重载操作符
    CMessagePtrQueue& operator= (const CMessagePtrQueue& right);

private:

    // 递归锁
    std::recursive_mutex m_rMutex;

    // 存放消息指针的队列
    std::deque< CMessagePtr > m_messagePtrQueues;

};



#endif // #ifndef CMESSAGEPTR_QUEUE_H

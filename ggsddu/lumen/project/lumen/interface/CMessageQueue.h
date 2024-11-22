#ifndef CMESSAGE_QUEUE_H
#define CMESSAGE_QUEUE_H


#include "interface/CMessage.h"
#include "interface/IObject.h"

#include <iostream>
#include <deque>
#include <mutex>


/*
    该类为缓存消息的队列，包括通知和网络消息.
*/

class CMessageQueue
    : public IObject
{
public:


	// 消息队列的内存分配模式
	enum
	{
        // 操作系统分配内存模式
		SYSTEM_MALLOC,
		// 内存池分配内存模式
		POOL_MALLOC
	};

	// 通过预设消息大小和内存分配模式构造CMessageQueue对象
	CMessageQueue(void);

	// 通过预设消息大小和内存分配模式构造CMessageQueue对象
	explicit CMessageQueue(const int32_t maxMessageSize, const int32_t mallocMode = SYSTEM_MALLOC);

	// 析构函数
	virtual ~CMessageQueue(void);

	// 发送消息到消息对列
	bool postMessage(const CMessagePtr& msg);

	// 从消息对列中取出消息
	bool getMessage(const CMessagePtr& msg);

	// 从消息对列中取出缓冲区数据
	bool getBuffer(int8_t* buf, size_t& len);

	// 发送缓冲区数据到消息对列
	bool postBuffer(const int8_t* const buf, const size_t len);

	// 复位消息队列
	void reset(void);

	// 获得消息的读位置
	int32_t getReadPtr(void) const;

	// 获得消息的写位置
	int32_t getWritePtr(void) const;

	// 消息对列中未处理的数据尺寸
	int32_t getSize(void) const;

	// 获得消息的缓冲区指针
	const int8_t* getMessageBuffer(void) const;

    // 友元
	friend class CDynamicMessageQueue;

private:

	// 消息缓冲区最大值
	static const int32_t MESSAGE_MAX_SIZE = 8192;

	// 重设消息读指针
	void resetReadPtr(void);

	// 内存分配方式
	int32_t m_mallocMode;

	// 消息最大值
	int32_t m_maxMessageSize;

	// 消息缓存指针
	int8_t* m_MessageBuffer;

	// 消息读位置
	int32_t m_readPtr;

	// 消息写位置
	int32_t m_writePtr;

	// 解析消息，保存到msg中
	bool resolveMessage(const CMessagePtr& msg);

    // 递归锁
    std::recursive_mutex m_rMutex;

};

DECLARE_PTR_TYPE(CMessageQueue)
/*
    该类用于动态生成消息队列。如果一个消息队列满了,会再生成一个消息队列供使用。
*/
class CDynamicMessageQueue
{
public:
	// 通过预设消息大小和内存分配模式构造CDynamicMessageQueue对象
	CDynamicMessageQueue(void);

	// 通过预设消息大小和内存分配模式构造CDynamicMessageQueue对象
	explicit CDynamicMessageQueue(const int32_t queueSize, const int32_t mallocMode = CMessageQueue::SYSTEM_MALLOC);

	// 析构函数
	~CDynamicMessageQueue(void);

	// 将消息保存到队列中
	bool postMessage(const CMessagePtr& msg);

	// 从队列获取信息
	bool getMessage(CMessagePtr& msg);

	// 还原消息队列
	void recover(void);

	// 清除消息队列
	void clear(void);

	// 判断消息是否为空
	bool empty(void) const;

private:

	// 队列消息大小
	int32_t m_queueSize;

	// 内存分配方式
	int32_t m_mallocMode;

	// 消息队列
	std::deque< CMessageQueuePtr > m_msgQueues;

	// 备份消息队列
	std::deque< CMessageQueuePtr > m_msgBakQueues;

};

#endif // #ifndef CMESSAGE_QUEUE_H

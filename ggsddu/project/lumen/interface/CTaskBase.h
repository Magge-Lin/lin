#ifndef CTASK_BASE_H
#define CTASK_BASE_H


#include "interface/CMessage.h"
#include "interface/CMessagePtrQueue.h"
#include "interface/IObject.h"


#include <thread>
#include <atomic>
#include <mutex>
#include <memory>

/*
    用于生成一个新的线程和消息队列
*/

class CTaskBase
    : virtual public IObject
{

public:

	// 分配内存的模式
	enum EMallocMode
	{
        // 操作系统分配内存模式
		SYSTEM_MALLOC,
        // POOL分配内存模式
		POOL_MALLOC
	};
	
	// CTaskBase的构造函数
	explicit CTaskBase(void);
	
	// CTaskBase的析构函数
	virtual ~CTaskBase(void);

	// 启动线程
	virtual int32_t start(void);

	// 停止线程
	virtual void stop(void);
	
	// 等待线程退出，阻塞方式
	virtual void wait(void) const;

	// 发送消息到消息队列中
	virtual bool postMessage(const CMessagePtr& msg);

	// 从消息队列中获取消息
	virtual bool getMessage(CMessagePtr& msg);

	// 删除线程句柄
	void clearThreadHandle(void);

	// 设置线程的运行等级
	void setPriority(const int32_t prio) const;

protected:

	// 运行线程
	virtual int32_t run(void) = 0;

	// 判断线程是否被取消
	int32_t testCancel(void)  const;

	// 消息队列
	CMessagePtrQueue m_queue;

private:


	// 操作任务
	static void taskFunc(CTaskBase* const pThis);

    // 工作线程
    std::thread m_workerThread;

    // 线程是否正在运行
    std::atomic<bool> m_isRunning;

    // 标识是否请求停止当前的线程
    std::atomic<bool> m_stopRequested;
};

DECLARE_PTR_TYPE(CTaskBase)


#endif // #ifndef CTASK_BASE_H



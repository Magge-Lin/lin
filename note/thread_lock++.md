# 对锁的补充 （实际应用）

> 1、互斥锁：mutex
> 2、读写锁：shared_mutex
> 3、原子操作：atomic
> 	自旋锁：spinlock
> 	内存屏障：atomic_thread_fence
> 	内存顺序：memory_order_relaxed、memory_order_acquire/memory_order_release、memory_order_acq_rel、memory_order_seq_cst
> 	CAS操作实现无锁消息队列：lockFreeQueue
> 4、条件变量：condition_variable
> 	生产者-消费者模型
> 	瞬时事件通知
> 	超时等待
> 	线程池中的任务同步
> 	避免虚假唤醒
> 	注意事项：std::unique_lock保护下使用条件变量

## 1、互斥锁：mutex

```C++
# 头文件

# 用法
```


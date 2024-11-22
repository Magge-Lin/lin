# thread && lock

```
thread：
	创建，管理，回收，同步

locks：
	1、互斥锁：mutex
	2、读写锁：shared_mutex
	3、原子操作：atomic
		自旋锁：spinlock
		内存屏障：atomic_thread_fence
		内存顺序：memory_order_relaxed、memory_order_acquire/memory_order_release、memory_order_acq_rel、memory_order_seq_cst
		CAS操作实现无锁消息队列：lockFreeQueue
	4、条件变量：condition_variable
		生产者-消费者模型
		瞬时事件通知
		超时等待
		线程池中的任务同步
		避免虚假唤醒
		注意事项：std::unique_lock保护下使用条件变量

多线程编程的高级主题
```



## 一、thread

### 1、线程基础

- #### 提问

```
1、什么是线程，从内核角度如何看线程资源 
	内核通过 TCB 来管理和调度线程。上下文的切换是个消耗资源的过程，所以性能优化可以减少线程的上下文切换。
2、线程跟进程的区别
	线程共享进程的资源：内存空间、文件交换
	线程独立的资源：栈、注册。
3、线程性能
	使用线程池、协程等，减少线程的创建和管理开销。
```

-  #### 多线程的基本语法

```c++
// 1、头文件
#include <thread>
#include <mutex>
// 2、创建线程的方法（通过 std::thread 对象）
std::thread t(func);
t.jion();

std::mutex s_mutex;
std::lock_guard<std::mutex> locks(s_mutex);
```

### 2、线程的创建和管理

- #### 创建线程

```c++
// 函数指针
void func1()
{
    std::cout<<"thread 1 func1."<<std::endl;
}
std::thread t1(func1);
t1.join();

// lambda 表达式
int value = 10;
std::thread t2[value](){
    std::cout<<"thread 2, lambda value = "<<value<<std::endl;
}
t2.join();

// 使用成员函数
class Demo{
public:
    void funcClass(){
        std::cout<<"thread 3, funcClass."<<std::endl;
    }
}
std::shared_ptr<Demo> demoPtr = std::make_shared<Demo>()
std::thread t3(&Demo::funcClass, demoPtr);
t3.join();

// 多线程使用 std::vector emplace_back
void threadFunc(int id){
    std::cout<<"threadFunc id = "<<id<<std::endl;
}
int numThreads = 5;
std::vector<std::thread> threads;
for(int i = 0; i < numThreads; ++i){
    threads.emplace_back(threadFunc, i);
}

for(auto& t : threads){
    try{
        t.jion();
    } catch (const std::exception& e){
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }
}
```

- #### 等待线程结束（回收线程资源）

```c++
// 1、使用 join 方法
std::thread t();
t.join();
// 2、使用 detach 方法
t.detach();
```

- #### 线程对象生命周期

```
1、线程对象的构造、析构与线程的关系
```

#### 3、线程同步:locks

## 二、locks

### 1、互斥锁(std::mutex)

```c++
// std::mutex、std::lock_guard、std::unique_lock

// 1、头文件
#include <thread>
#include <mutex>

// 2、std::mutex
std::mutex s_mutex;
s_mutex.lock();
std::cout<<"  "<<std::endl;
s_mutex.unlock();
// 注意事项：在使用 s_mutex.lock() 和 s_mutex.unlock() 时，如果程序在调用 lock() 后抛出异常，将导致未解锁的问题，从而引发死锁。
// 武夷山有轨，GPS故障发错了经纬度识别码，导致boost的函数异常抛出，没有解锁，造成了死锁.

// 3、std::lock_guard
std::mutex s_mutex;
{
	std::lock_guard<std::mutex> lock(s_mutex);
}

// 4、std::unique_lock
// 对 std::lock_guard 的优化，支持 RAII（声明周期结束自动释放资源）， 也支持手动解锁
{
    std::unique_lock<std::mutex> lock(s_mutex);	//加锁
    std::cout<<" std::unique_lock. "<<std::endl;
    lock.unlock();
    std::cout<<" lock.unlock(). "<<std::endl;
    lock.lock();	// 这里的lock.lock() 不手动释放作用域结束后，资源会自动回收
    std::cout<<" lock.unlock(). "<<std::endl;
}

// Tips: RAII 的意思指 在声明周期结束，自动回收资源.
```

### 2、读写锁：share_mutex

```c++
// 头文件
#include <iostream>
#include <thread>
#include <shared_mutex>
#include <vector>

// 使用
class SharedData {
public:
    void readData() const { // 注意这是一个 const 函数
        std::shared_lock<std::shared_mutex> lock(mutex_); // 可以合法地加锁
        // 执行读操作...
    }
    void writeData(int newData) {
        std::unique_lock<std::shared_mutex> lock(mutex_);	// 写操作使用独占锁
        // 执行写操作...
    }

private:
    mutable std::shared_mutex mutex_; // 使用 mutable，使得在 const 方法中也可用
};

/*
注意事项：
	1、读锁：std::shared_lock<std::shared_mutex> lock(mutex_);
	2、写锁：std::unique_lock<std::shared_mutex> lock(mutex_);
	3、声明读写锁，使用 mutable，使得在 const 方法中也可用：mutable std::shared_mutex mutex_;
*/ 
```

### 3、原子操作：atomic

```c++
/*
1、原子操作的定义
	原子性：表示某个操作是不可分割的，即在执行期间不会被其他线程打断.
	可见性：一个线程对共享变量所做的修改，对于其他线程是可见的。
	一致性：保证在多个线程并发访问共享数据时，数据的一致性。
2、为什么使用原子操作
	避免竞态数据：在多线程程序中，如果多个线程同时访问共享资源而没有适当的同步机制，就可能导致数据不一致。原子操作可以有效防止这种情况。
	性能：与锁相比，减少了上下文的切换和锁占用的问题。
3、原子操作的实现：
	基于CPU指令实现
４、常见的原子类型
	#include <atomic>
	std::atomic<T> atomic_T;
5、原子的局限性
	复杂性限制：不支持自定义数据结构
	ABA问题：线程读取一个值A，另一个线程将它改为B，再恢复为A，第一个线程无法发现这一变化.
7、实际应用场景
	a、计数器：高并发情况下需要维护计数状态的地方，比如请求计数、任务调度等。
	b、无锁数据结构：为了提高多线程读写性能，使用原子操作构建无锁队列、栈等数据结构。
	c、状态标志管理：例如使用标志位来控制不同线程之间的行为，无需加锁而影响性能。
*/
    
// 头文件
#include <iostream>
#include <thread>
#include <atomic>
// 声明
std::atomic<int> atomic_int;
// 写
atomic_int.store(1);
// 读
atomic_int.load();
// 替换，返回原值(++i)
int old_value = atomic_int.exchange(2);
// 增加 1，返回原值
atomic_int.fetch_add(1);
// 减少 1，返回原值
atomic_int.fetch_sub(1);
// 比较并交换
	// 在高竞争环境下，会导致假失败（原子变量未改变），返回false，因此适合在while中使用.
bool enable = atomic_int.compare_exchange_weak(预期旧值， 写入新值);	// 如果交换成功，就把新值写入，返回true.
	// 保证只有在目标变量实际发生变化时才会返回 false。因此，它适合于 if
bool enable = atomic_int.compare_exchange_strong(预期旧值， 写入新值);	// 如果交换成功，就把新值写入，返回true.  
// ##一般情况下： compare_exchange_weak 被优化为能够在高竞争情况下表现得更好，尤其是在通过循环调用该方法来处理竞态条件的时候。
void test_compare_exchange_weak() {
    int expected = 0;
    int desired = 1;
    // 尝试用 compare_exchange_weak 方法进行替换
    if (atomic_int.compare_exchange_weak(expected, desired)) {
        std::cout << "预期值正确，替换成功." << std::endl;
    } else {
        std::cout << "Weak compare and exchange failed. Expected was updated to: " << expected << std::endl;
    }
}

void test_compare_exchange_strong() {
    int expected = 0;
    int desired = 2;
    // 尝试用 compare_exchange_strong 方法进行替换
    if (atomic_int.compare_exchange_strong(expected, desired)) {
        std::cout << "Strong compare and exchange succeeded." << std::endl;
    } else {
        std::cout << "Strong compare and exchange failed. Expected was updated to: " << expected << std::endl;
    }
}

```

- #### 自旋锁：spinlock

```c++
#include <atomic>

class SpinLock {
public:
    SpinLock() : flag(ATOMIC_FLAG_INIT) {}

    void lock() {
        while (flag.test_and_set(std::memory_order_acquire));  // 自旋，直到获得锁
    }

    void unlock() {
        flag.clear(std::memory_order_release);  // 释放锁
    }

private:
    std::atomic_flag flag;  // 标志位，表示锁的状态
};
```

- #### 内存屏障：atomic_thread_fence

```c++
/*
	用于控制内存访问顺序的机制，以确保特定的读写操作按预期的顺序进行。
*/

#include <atomic>
#include <thread>
#include <iostream>

std::atomic<bool> ready(false);
int data = 0;

void producer() {
    data = 42;
    std::atomic_thread_fence(std::memory_order_release);  // 释放屏障
    ready.store(true, std::memory_order_relaxed);
}

void consumer() {
    while (!ready.load(std::memory_order_relaxed));
    std::atomic_thread_fence(std::memory_order_acquire);  // 获取屏障
    std::cout << "Data: " << data << std::endl;
}

```

- #### 内存顺序：memory_order_relaxed、memory_order_acquire/memory_order_release、memory_order_acq_rel、memory_order_seq_cst

```c++
std::memory_order_relaxed	// 不保证顺序，这种仅保证操作的原子性。模式通常用于累加等场景。
std::memory_order_acquire、std::memory_order_release	// 表盘使用，用于实现Acquire-Release语义。acquire在加载时建立“读屏障”，release在存储时建立“写屏障”。
std::memory_order_acq_rel	// 同时具备acquire和release语义，适合读-改-写操作。
std::memory_order_seq_cst	// 顺序一致性，最严格的模式，确保所有操作按程序顺序执行。
```

- #### CAS操作

```c++
/*
CAS，比较并交换是一种原子操作，用于实现无锁编程。
*/

compare_exchange_weak	// 允许在多种情况下失败并进行重试，适合在循环中使用。
compare_exchange_strong	// 保证CAS操作的成功或失败，不会出现失误。
```



### 4、条件变量 （std::condition_variable）

```c++
// 1、头文件
#include <condition_variable>

```

- #### 生产者-消费者模型

```c++
/*
	当有一个生产者和多个消费者：生产者可以通过 notify_one 通知某个消费者去处理刚刚产生的数据。
	避免虚假唤醒：即使没有满足条件，调用 notify_one 也会导致线程被唤醒，因此在实际代码中，应该用循环检查条件是否依旧不满足。	
*/
std::queue<int> buffer;
const unsigned int MAX_BUFFER_SIZE = 2;

std::mutex mtx;                         // 互斥锁
std::condition_variable cond_var;       // 条件变量

void producer()
{
    for (int i = 0; ; i++)
    {
        std::unique_lock<std::mutex> lock(mtx);
        while (buffer.size() >= MAX_BUFFER_SIZE)    // 使用 while 防止虚假唤醒
        {
            // 如果缓冲区满，则等待
            cond_var.wait(lock);        //  解锁并等待其他线程通知
        }

        buffer.push(i);
        std::cout << "producer: "<< i << std::endl;
        // cond_var.notify_all();  
        cond_var.notify_one();      //  唤醒一个消费者
    }
}
void consumer()
{
    for (int i = 0; ; i++)
    {
        std::unique_lock<std::mutex> lock(mtx);
        while (buffer.empty())
        {
            cond_var.wait(lock);
        }

        int value = buffer.front();
        buffer.pop();
        std::cout << "Consumed: " << value << std::endl;
        // cond_var.notify_all();  
        cond_var.notify_one();      // 唤醒一个生产者（如果有的话）
    }
}

int main() {

    std::thread prod_thread(producer);
    std::thread cons_thread(consumer);

    prod_thread.join();
    cons_thread.join();

    return 0;
}

// 注意事项：
/*
	1、只唤醒一个线程： notify_one() 会随机选择一个正在等待的线程进行唤醒，具体哪个不确定
		唤醒所有等待的线程，可以使用 notify_all() .
	2、确保持有互斥锁： 在调用 wait() 和 notify_one() 时，务必确保已经持有了对应的互斥锁，避免出现数据竞争和未定义行为.
    3、检查条件，将if(条件) 改为while(条件)，防止虚假唤醒
    4、性能考虑：频繁调用notify_one()，会导致上下文的开销。
*/
```

- #### 超时等待

```c++
#include <iostream>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <mutex>

class SharedResource {
public:
    void produce() {
        std::lock_guard<std::mutex> lock(mtx);
        dataReady = true; // 生产数据
        std::cout << "Produced data\n";
        condVar.notify_one(); // 通知消费者
    }

    void consume() {
        std::unique_lock<std::mutex> lock(mtx);
        
        // 等待直到数据准备好或者达到超时时间（例如3秒）
        if (condVar.wait_for(lock, std::chrono::seconds(3), [this]() { return dataReady; })) {
            // 被通知且数据已准备好
            std::cout << "Consumed data\n";
            dataReady = false; // 重置状态
        } else {
            // 超过等待时间仍未准备好数据
            std::cout << "Timeout: No data was produced within the wait period.\n";
        }
    }

private:
    std::mutex mtx;
    std::condition_variable condVar;
    bool dataReady = false;
};

int main() {
    SharedResource resource;

    // 创建消费者线程
    std::thread consumer([&resource]() {
        resource.consume();
    });

    // 模拟生产者稍后再生产数据
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 创建生产者线程
    std::thread producer([&resource]() {
        resource.produce();
    });

    consumer.join();
    producer.join();

    return 0;
}


/*
示例解释
	1、SharedResource 类：
		包含一个布尔值 dataReady 表示是否有可消费的数据。
		使用 std::mutex 和 std::condition_variable 来保护对共享资源的访问和实现同步。
	
	2、produce 方法：
		在这个方法中，锁定互斥量，然后设置 dataReady 为 true，表示已生成新数据。随后调用 notify_one() 唤醒一个可能正在等待的消费者线程。

	3、consume 方法：
		首先获取互斥量锁。
		然后使用 wait_for 方法尝试在最多 3 秒内等待，直到有数据可供消费或超时。
			如果在规定时间内被唤醒且数据已准备好，则打印相应消息，并重置状态。
			如果超过了指定时间仍未被唤醒，则打印超时信息。
	
	4、主函数：
		创建并启动消费者线程，它会尝试消费资源。
		主线程休眠一段时间以模拟生产者稍后才开始生产数据。然后创建并启动生产者线程。
		最后，主函数等待两个线程完成执行。
*/
```

- #### 线程池中的任务同步

```c++
#include <iostream>
#include <thread>
#include <queue>
#include <condition_variable>
#include <functional>
#include <vector>
#include <atomic>

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    void enqueue(std::function<void()> task);

private:
    std::vector<std::thread> workers;          // 工作线程
    std::queue<std::function<void()>> tasks;   // 任务队列

    std::mutex mtx;                            // 互斥锁
    std::condition_variable condVar;           // 条件变量
    bool stop = false;                          // 停止标志
};

// 构造函数
ThreadPool::ThreadPool(size_t numThreads) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(mtx);
                    condVar.wait(lock, [this] { return stop || !tasks.empty(); });

                    if (stop && tasks.empty()) 
                        return;

                    task = std::move(tasks.front());
                    tasks.pop();
                }

                task(); // 执行任务
            }
        });
    }
}

// 析构函数
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mtx);
        stop = true;
    }
    
    condVar.notify_all(); // 唤醒所有工作线程，以便它们能退出

    for (std::thread &worker : workers) 
        worker.join(); // 等待所有工作线程结束
}

// 添加新任务到队列
void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(mtx);
        tasks.push(std::move(task));
    }

    condVar.notify_one(); // 唤醒一个等待中的工作线程
}

int main() {
    ThreadPool pool(4); // 创建一个有4个工作线程的线程池
    
    for (int i = 0; i < 10; ++i) {
        pool.enqueue([i] {
            std::cout << "Task " << i << " is being processed by thread: "
                      << std::this_thread::get_id() << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 模拟耗时操作
        });
    }

    return 0;
}


/*
示例解释
	1.ThreadPool 类：
        workers 向量存储了所有工作线程。
        tasks 队列用于存储待处理的任务。
        使用 std:mutex 和 std:condition_variable 来控制对共享资源（即任务队列）的访问。

	2、构造函数：
		在构造函数中，我们创建指定数量的工作线程，并让它们进入循环。在循环中，工作线程会等待直到有新任务可供执行或者接收到停止信号。
		使用条件变量 condVar.wait(lock, ...) 来让工作线程在没有可用任务时进入阻塞状态。
	
	3、析构函数：
		当 ThreadPool 被销毁时，会设置停止标志并通知所有正在等待的工作线程，使它们能够安全地退出。

	4、enqueue 方法：
		用于将新任务添加到队列中，并通过调用 condVar.notify_one() 通知至少一个正在等待的工作线程，让它们尝试获取新的任务。

	5、主程序：
		在主程序中，我们创建一个包含四个工作的 ThreadPool 实例，并向其提交十个简单的打印任务，每个打印过程模拟了耗时操作（通过调用 sleep）。
*/
```

- #### 避免虚假唤醒

```c++
// 检查条件，将if(条件) 改为while(条件)，防止虚假唤醒
```

- #### 注意事项：

```c++
// std::unique_lock保护下使用条件变量
```



## 三、多线程编程的高级主题

- 线程安全与数据竞争

```
1、理解数据竞争的概念和危害
2、如何避免数据竞争，确保线程安全
```

- 多线程下的内存模型

```
1、了解多线程内存模型的基本规则
2、内存序（memory_order）的概念和用法
3、内存屏障是啥
```

- 线程池

```
1、理解线程池的概念和优势
2、设计实现简单的线程池
3、找个开源的线程池项目，研究如何使用
4、线程池的使用场景
```

- 多线程调试技巧

```
1、使用调试工具（如GDB）调试多线程程序
2、常见的多线程调试问题以及解决方法
```

## 四、实际应用案例

- 并行计算

```
1、使用多线程进行简单的并行计算任务
2、性能比较与优化
```

- 文件处理或网络编程中的多线程应用

```
1、多线程下载文件
2、多线程服务器编程的基本概念
3、实现一个数据转发服务器
4、对联挂解编网络模块进行多线程优化
```


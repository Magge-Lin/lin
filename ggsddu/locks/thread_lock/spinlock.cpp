

// 自旋锁


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



/*
                example -- > spinlock
                编译：g++ -g -o spinlock spinlock.cpp
*/

#include <iostream>
#include <thread>
#include <atomic>
#include <vector>


// 共享资源
int counter = 0;
SpinLock spinlock;

// 线程函数，增加计数器
void incrementCounter(int numIncrements) {
    for (int i = 0; i < numIncrements; ++i) {
        spinlock.lock();    // 获取锁
        ++counter;          // 修改共享资源
        spinlock.unlock();  // 释放锁
    }
}

int main() {
    int numThreads = 10;
    int incrementsPerThread = 1000;

    // 创建多个线程
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(incrementCounter, incrementsPerThread);
    }

    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }

    // 输出最终的计数器值
    std::cout << "Final counter value: " << counter << std::endl;

    return 0;
}

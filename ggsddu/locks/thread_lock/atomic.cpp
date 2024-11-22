

/*
    原子操作
*/

#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>

// 声明
std::atomic<int> atomic_int;

void func()
{
    if (atomic_int.load() < 10)     // 获取当前值
    {
        int oldValue = atomic_int.exchange(atomic_int.load() + 1);      // 原子替换当前值，并返回旧值。
        std::cout<<"oldValue = "<<oldValue<<std::endl;

        int newValue = atomic_int.fetch_add(1);         // 将其增加 1，返回增加前的值
        std::cout<<"newValue = "<<newValue<<std::endl;

        newValue = atomic_int.fetch_sub(1);         // 将其减少 1，返回减少前的值
        std::cout<<"newValue = "<<newValue<<std::endl;

        atomic_int.fetch_add(1);
        std::chrono::milliseconds(10000);
    }
}


int main()
{
    atomic_int.store(1);    // 原子写入一个新值

    const int numThreads = 5;

    std::vector<std::thread> threads;

    // 启动多个线程来增加计数器
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(func);
    }

    // 等待所有线程完成工作
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Final counter value: " << atomic_int.load() << std::endl;

    return 0;
}
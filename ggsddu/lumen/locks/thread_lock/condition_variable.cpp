
/*
        使用条件变量构建  生产者消费者模式
*/

#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

std::mutex s_mutex;     // 互斥锁
std::condition_variable cond_var;   // 条件变量

std::queue<int> buffer;
const unsigned int MAX_BUFFER_SIZE = 20;

void producer()
{
    for (int i = 0; i < 100; i++)
    {
        std::unique_lock<std::mutex> lock(s_mutex);
        while (buffer.size() >= MAX_BUFFER_SIZE)
        {
            cond_var.wait(lock);
        }

        buffer.push(i);
        std::cout << "Produced: " << i << std::endl;
        cond_var.notify_one();        
    }
}

void consumer()
{
    for (int i = 0; i < 100; i++)
    {
        std::unique_lock<std::mutex> lock(s_mutex);
        while (buffer.empty())
        {
            cond_var.wait(lock);
        }

        int value = buffer.front();
        buffer.pop();
        std::cout << "Consumed: " << value << std::endl;
        cond_var.notify_one();        
    }
}
int main()
{
    std::thread prod_thread(producer);
    std::thread cons_thread(consumer);
    prod_thread.join();
    cons_thread.join();

    return 0;
}
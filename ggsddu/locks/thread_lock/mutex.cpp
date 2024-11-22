
/*
        互斥锁
*/


#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>

std::mutex m_mutex;
static int num = 0;

void threadFunc1(int id)
{
    while (1)
    {
        m_mutex.lock();
        num++;
        std::cout<<"Thread Id: "<<std::this_thread::get_id() <<"    threadFunc thread id = "<<id << ",      num = "<<num <<std::endl; 
        m_mutex.unlock();
        std::chrono::milliseconds(1000);
    }
}
void threadFunc2(int id)
{
    while (1)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            num++;
            std::cout<<"Thread Id: "<<std::this_thread::get_id() <<"    threadFunc thread id = "<<id << ",      num = "<<num <<std::endl; 
        }
        std::chrono::milliseconds(1000);
    }
}
void threadFunc3(int id)
{
    while (1)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            num++;
            std::cout<<"Thread Id: "<<std::this_thread::get_id() <<"    threadFunc thread id = "<<id << ",      num = "<<num <<std::endl; 
        }
        std::chrono::milliseconds(1000);
    }
}

int main()
{
    int numThreads = 10;
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; i++)
    {
        if (i < 3)
        {
            threads.emplace_back(threadFunc1, i);
        }
        else if (i < 6)
        {
            threads.emplace_back(threadFunc2, i);
        }
        else if (i < 10)
        {
            threads.emplace_back(threadFunc3, i);
        }
    }
    for (auto& t : threads)
    {
        t.join();
    }

    return 0;
}


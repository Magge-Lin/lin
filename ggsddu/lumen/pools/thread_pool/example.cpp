#include "CThreadPool.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

std::mutex s_mutex;

typedef std::atomic<unsigned int> atomic_uint;
atomic_uint s_num(0);

void func(void* args, int i)
{
    std::cout << "threadId: "<< std::this_thread::get_id() << ",   ---- start ----    "<< "  s_num = " << s_num.load();

    atomic_uint* num = (atomic_uint*)args;
    num->fetch_add(1);
    
    std::cout << ",   ----  end ----s_num = " << s_num.load() << std::endl;
}


int main()
{
    CThreadPool threadpool;
    threadpool.init(5);
    threadpool.start();

    int num = 10000;
    for (size_t i = 0; i < num; i++)
    {
        threadpool.exec(func, &s_num, i);
    }

    threadpool.waitForAllDone();
    threadpool.stop();

    std::cout<<"threadPool stop."<<std::endl;

    return 0;
}
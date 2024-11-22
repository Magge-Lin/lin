#include "IAlloc.h"

#include <iostream>
#include <dlfcn.h>
#include <thread>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <random>
#include <string.h>


std::mutex s_mutex;
std::random_device rd;  // 获取一个真正的随机数种子
std::mt19937 gen(rd()); // 初始化 Mersenne Twister 随机数生成器

// 定义一个均匀分布范围 [min, max]
std::uniform_int_distribution<> dis(89934592,  589934592); // 从 1 到 100 的整数
void function()
{
    while(1)
    {
        int size = dis(gen);
        // void* pointer = IAlloc::instance()->malloc(size);
        void* pointer = malloc(size);
        memset(pointer, 0, size);
        
        if (pointer == NULL)
        {
            std::cout<<"malloc err."<<std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));


        // std::cout << "size: " << size << std::endl;

        // IAlloc::instance()->free(pointer, size);
        free(pointer);
    }
}
int main()
{
    const int numThreads = 100;
    std::vector<std::thread> threads;

    for (size_t i = 0; i < numThreads; i++)
    {
        threads.push_back(std::thread(function));
    }
    
    for (auto& t : threads) 
    {
        t.join(); 
    }

    return 0;
}
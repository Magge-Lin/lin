#include "IAlloc.h"

#include <iostream>
#include <dlfcn.h>
#include <thread>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>

std::mutex s_mutex;

void function()
{
    while(1)
    {
        {
            // std::lock_guard<std::mutex> lock(s_mutex);

            std::cout << "start...      Current thread ID: " << std::this_thread::get_id() << std::endl;
            void* pointer = IAlloc::instance()->malloc(100);

            IAlloc::instance()->free(pointer, 100);
            std::cout << "end...      Current thread ID: " << std::this_thread::get_id() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
int main()
{
    const int numThreads = 5;
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


#include <iostream>
#include <thread>
#include <memory>
#include <vector>


void threadFunc(int id)
{
    if (id > 5)
    {
        std::cout<<"Thread Id: "<<std::this_thread::get_id() <<"    failed threadFunc thread id = "<<id<<std::endl;
    }
    
    std::cout<<"Thread Id: "<<std::this_thread::get_id() <<"    threadFunc thread id = "<<id<<std::endl;
}

class threadDemo
{
public:
    void threadFunc(int id)
    {
        std::cout<<"Thread Id: "<<std::this_thread::get_id() <<"    threadDemo threadFunc thread id = "<<id<<std::endl;
    }
};

int main()
{
    // 函数指针
    std::thread t1(threadFunc, 1);
    t1.join();

    // 类成员函数
    std::shared_ptr<threadDemo> demoPtr = std::make_shared<threadDemo>();
    std::thread t2(&threadDemo::threadFunc, demoPtr, 2);
    t2.join();

    // lambda 表达式
    int value = 10;
    std::thread t3([value](){
        std::cout<<"lambda thread value = "<<value<<std::endl;
    });
    t3.join();

    // 使用 std::vector emplace_back
    int numThreads = 5;
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; i++)
    {
        threads.emplace_back(threadFunc, i);
    }
    for (auto& t : threads)
    {
        t.join();
    }

    return 0;
}


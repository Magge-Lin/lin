内存泄漏

>  https://mp.weixin.qq.com/s/fZzNNiEp0Qb374J37SP2xw



## 原因

- 未释放

```
注意类成员函数中申请的内存，在析构函数中释放.
```

- 未匹配

```
调用 malloc\new 后为使用f ree\delete 释放，需要注意类成员变量的 malloc\new 需要 在析构函数中 free\delete.
```

- 虚析构

```
std::string能否被继承，为什么？
不能，std::string 的析构函数不是虚函数，被继承后不能自动调用析构函数，会造成内存泄漏.
继承关系中，基类\派生类的构造函数、析构函数调用顺序.
```

- 循环引用

```c++
/*
	两个 share_ptr 指针相互引用，会造成引用计数永远不为0的情况，导致不会调用析构函数从而内存泄漏，对循环引用的对象使用weak_ptr 解决循环引用内存泄漏的问题.

	share_ptr 配合 weak_ptr 的使用方法 && weak_ptr 访问对象成员的办法：
*/


class B;	// 先声明 B 类
class A {
public:
    std::weak_ptr<B> b_ptr;
};

class B {
public:
    std::weak_ptr<A> a_ptr;
};

std::shared_ptr<B> lockedB = a->b_ptr.lock();
if (lockedB) {
    std::cout << "B is accessible." << std::endl;
} else {
    std::cout << "B is not accessible." << std::endl;
}
    
```

## 避免

- 避免在堆上分配

- 手动释放

- 避免使用裸指针

- 使用STL或者自己实现的对象

- 智能指针

```C++
#include <memory>
#include <iostream>

// 控制权限转移
std::unique_ptr<int> ptr = std::make_unique<int>(10);	
// 控制权限共享 --引用计数
std::shared_ptr<int> ptr1 = std::make_shared<int>(20);
// share 循环引用时，引用计数相互指引永不为0，导致内存泄漏，使用 weak_ptr 
std::shared_ptr<int> sharedPtr = std::make_shared<int>(30);
std::weak_ptr<int> weakPtr = sharedPtr;
std::shared_ptr<int> weak2sharedPtr = weakPtr.lock();
```

- RAII

```c++
// 	资源获取即初始化，C++构造的对象最终会被销毁的原则，利用C++对象生命周期的概念来控制程序的资源.

std::mutex m_mutex;
{
    m_mutex.lock();
    
    m_mutex.ulock();
}

{
    std::lock_guard<std::mutex> guard(m_mutex);
}
```

## 定位

> 首先，不管啥办法能定位到就是好办法.

- 日志 && 统计

```
宏定义 malloc、new、free、delete等，添加日志的打印\计数.

使用hook技术捕获malloc、new、free、delete等，计算开销是否平衡.
```

- 工具

```shell
#  linux : valgrind
> sudo apt install valgrind
> gcc -g leak.c -o leak
> valgrind --leak-check=full ./leak
> addr2line -f -e ./leak -a 0xaabbccdd

lin@Ubuntu:~/share/memlock$ valgrind --leak-check=full ./leak 
==9370== Memcheck, a memory error detector
==9370== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==9370== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==9370== Command: ./leak
==9370== 
==9370== 
==9370== HEAP SUMMARY:
==9370==     in use at exit: 10 bytes in 1 blocks
==9370==   total heap usage: 1 allocs, 0 frees, 10 bytes allocated
==9370== 
==9370== 10 bytes in 1 blocks are definitely lost in loss record 1 of 1
==9370==    at 0x4846828: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==9370==    by 0x10915E: func (leak.c:4)
==9370==    by 0x109172: main (leak.c:8)
==9370== 
==9370== LEAK SUMMARY:
==9370==    definitely lost: 10 bytes in 1 blocks
==9370==    indirectly lost: 0 bytes in 0 blocks
==9370==      possibly lost: 0 bytes in 0 blocks
==9370==    still reachable: 0 bytes in 0 blocks
==9370==         suppressed: 0 bytes in 0 blocks
==9370== 
==9370== For lists of detected and suppressed errors, rerun with: -s
==9370== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
```

​	windows：UMDH

> [使用 UMDH 查找用户模式内存泄漏 - Windows drivers | Microsoft Learn](https://learn.microsoft.com/zh-cn/windows-hardware/drivers/debugger/using-umdh-to-find-a-user-mode-memory-leak)







## memleak.c 的使用

### 1、编译

```shell
# 动态库：
	gcc -fPIC -shared -g -o libmemleak.so memleak.c -ldl
# example:
	gcc -g -o example example.c -L. -lmemleak
```

### 2、运行

```shell
# 动态库目录
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
# 创建目录
mkdir mem
# 运行
./example
# cat mem/0x6532765123.mem
[+] caller: 0x11b2, addr: 0x5968a18d24b0, size: 10
# 查看内存泄漏位置
addr2line -f -e ./example -a 0x11b2
# 0x00000000000011b2
# main
# /home/lin/lumen/debug/memleak/example.c:12
```


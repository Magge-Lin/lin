# mem_pool

```
1、内存池
2、boost::pool 内存池的使用
3、内存池C++封装单例
4、内存池性能测试，比较有内存池和无内存池的性能差别
```

## 1、内存池

```
boost pool
google TCMalloc
jemalloc
```

## 2、boost::pool 内存池的使用方法

```c++
// k_v 容器存储管理多个内存池，按照内存大小统一归类管理
std::map<size_t, boost::pool<>*> m_memPools;		// k = 申请的内存大小， v = 内存池

// 申请内存时，先找到对应size的内存池，再从对应内存池获取一个size大小的内存块
malloc(size);

// 释放内存时，先找到对应size的内存池，再从该内存池中回收 ptr 的内存块
free(ptr, size);
```

## 3、对 boost::pool 封装单例的使用

```
CAlloc.cpp
CAlloc.h
IAlloc.cpp
IAlloc.h
libAlloc.so

// 使用
demo.cpp
```

## 4、性能测试

```
内存碎片太多会导致内存泄漏的报错，申请内存失败。

测试正常通信状态下，车跑一圈用的时间做对比。
```


# conn_pool

> redis 连接池
>
> mysql 连接池
>
> redis 异步操作

## 一、mysql_pool

### 1、基于 C++ 的 mysql 连接池接口

```C++

```



### 2、mysql 连接池的使用

```c++

```



### 3、mysql 连接池的性能测试比较

```c++

```



### 4、使用 C++ 单例的方式封装 mysql 连接池

```C++

```



## 二、redis_pool

### 1、基于 C++ 的 redis 连接池接口

```

```

### 2、redis 连接池的使用

```

```



### 3、redis 连接池的性能测试比较

```

```



### 4、使用 C++ 单例的方式封装 redis 连接池

```

```

## 三、redis async

1、redis 异步操作的语法



## 四、异步数据库查询时遇到的问题

>  异步程序，是否需要回调，或者回调通知，比如 数据库查操作，如何回归当前线程已经查到数据了。

### 1、getDatabase() 返回值异步的处理方式

- 回调方式

```c++
// 回调方式
void TaskGetSql(std::any sqlName, std::any check){
    std::vector<std::any> data = IDatabase::getDatabase(sqlName, check);
    char* buff = doSomething(data);		// 业务处理函数
    // char* buff 这里没调 delete，实际开发会在 sendMessage 后delete，或者构造 CMessage 对象管理接收和发送缓存数据
    
    std::lock_guard<std::mutex> lock(m_mtx);	 // 加锁以防止其他线程干扰，确保sendMessage()执行时数据一致性
    sendMessage(buff);
    // 但是这里需要个锁，防止心跳超时关闭链接并且析构网络管理对象
}

void handleMessage(int msgId) {
    if (msgId == 1) {		// 从数据库查数据处理后 发给 client
        int seqnum = 100;	// 模拟查询条件
        std::string sqlname = "database 1";
        std::thread t(TaskGetSql, std::make_any<std::string>(sqlname), std::make_any<int>(seqNum));
        t.join();
    }
    else if(msgId == 2){
        // do ..
    }
}

// 异步的逻辑流程有点复杂
```








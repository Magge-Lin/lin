# 发现cpu占用100%的检查方法

## 1、分析

```
1、先查看软件的版本，前几个版本是不是正常的，从哪个版本开始的，检测一下对应版本上传的代码.
2、使用工具软件检测一下占用资源的线程，拿到线程Id号，能查日志的查日志，能debug的debug.
3、我遇到的CPU占用过高的场景：
    1、真的不够了
    2、需要优化：使用异步，用条件变量替换sleep，对网络数据添加缓存区
    3、while死循环没有sleep/condition_variable
```



## 2、平台

- ### windows

```bat
# 1、查看cpu、内存、磁盘等
任务管理器查看，进程使用资源情况

# 2、查看具体进程中线程的使用情况
ProcessExplorer  

very Sleepy   抓取一段的的线程运行数据
```



- linux

```shell
# 1、查看cpu、内存、磁盘等
htop

# 2、查看占用过高的进程Id

# 3、找到该进程中占用资源过高的线程

# 4、看日志或者gdb找到该线程，查看堆栈信息

# 5、review Code
```

## 3、htop 的使用

- 如何查看具体含义

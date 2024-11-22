# GDB 调试

## 1、多文件动态库的编译和使用：

```shell
g++ -fPIC -shared -o libAlloc.so IAlloc.cpp CAlloc.cpp 
g++ -o demo demo.cpp -L. -lAlloc
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
./demo
```

## 2、gdb调试实践：

```shell
###			调试为运行的程序		demo
g++ -g -o demo demo.cpp -L. -lAlloc
gdb ./demo
#开始
run     # start
#断点
b demo.cpp:13
b CAlloc::instance()
info b
#查看变量
p s_alloc
display s_alloc
#查看断点上下代码
list
#继续执行
c
# 按行执行
n
# 进入函数内部
step
#查看线程
info threads
#选择线程
thread 1
#查看堆栈信息
backtrace
#结束调试
kill
ctrl C   q

###			调试正在运行的程序 	demo
#查看PID
ps aux | grep demo
#捕获demo程序到gdb
gdb attach PID
#调试结束 归还程序
detach

###			调试宕机出现core的程序	demo
#查看系统配置，能否产生 core 文件
ulimit -c		# 返回0，即不能生产core.
#修改系统配置，可以生产core文件		可以在当前 shell 会话有效
ulimit -c unlimited
# 找到 demo 宕机后生成的 core 文件
sudo find / -name "core._home_lin_lumen_mem_pool_*" 2>/dev/null
> /var/lib/apport/coredump/core._home_lin_lumen_mem_pool_demo.1000.c6aae5f5-89dc-437a-853a-3b45ad0219a7.24712.10122296
# 使用 gdb 查看 core 信息
gdb ./demo /var/lib/apport/coredump/core._home_lin_lumen_mem_pool_demo.1000.c6aae5f5-89dc-437a-853a-3b45ad0219a7.24712.10122296


#######				gdb 调试动态库 libAlloc.so 程序 demo
# 编译时候动态库 libAlloc.so 和 运行程序 demo 带 -g
g++ -fPIC -shared -g -o libAlloc.so IAlloc.cpp CAlloc.cpp
g++ -g -o demo demo.cpp -L. -lAlloc
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
gdb ./demo				#这个时候进入gdb一般看不到动态库，还未加载
start
b demo.cpp::main()
run
info sharedlibrary		#查看当前加载的动态库文件
# 这个时候可以动态库代码文件中添加断点，进入调试

# 手动加载动态库文件
（gdb） sharedlibrary libAlloc.so
# 或者启动的时候添加
LD_LIBRARY_PATH=./ gdb ./demo

```

## 3、实践

```
在当前项目文件下，编译 libAlloc.so 动态库，使用 demo 编译连接，然后调试找到宕机的位置和原因。
```

## 4、多线程程序的调试

```shell
# gdb 启动
```

## 5、宕机后如何查找 core dump 中的日志

```

```


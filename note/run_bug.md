1、多线程编程时候遇到的问题：数据竞争，边界数据，对锁的应用

```
原子操作的使用场景：
	1、网络连接状态的标志位，每个设备控制器 双机双网，控制连接状态
	2、数据缓冲区管理，无锁队列结构中原子的操作写入和读取位置
	3、网络通信中，一般会使用应用层心跳数据包，判断对端应用是否存活，检测到心跳超时即主动断开，但是不能直接关闭网络连接（如果还有线程正在发送数据，就会出现问题），添加对端应用存活标志位，收发数据前先判断标志位，再收发，一个线程写多个线程读需要进行原子操作，防止出错。
	4、联挂解编中联挂状态、主\从控车状态，对双主双从，连续状态的计数，要使用原子操作在多线程中比较大小以及自增。
分布式锁 redlock-cpp：
	ATS客户端原本设计，对岔区的控制权限只有在登录用户之间转移，不接受共享；后来都江堰项目业主希望多个调度可以共享控制权限，快速操作。多个客户端对同一个设备下发相同命令，会导致下发两遍；在现有项目上使用redis数据库构建分布式锁。
使用协程，并使用协作式调度主动出让而不是抢占：
	一个线程解决并发问题，避免数据竞争和死锁，通过对协程调度的优化，避免数据竞争。
	
```

2、网络数据包是高IO的，并发量不大，但是频率高，IO交换频繁，会导致动态申请和释放堆内存比较多，内存碎片比较多导致申请失败，以及效率不高，使用内存池进行优化；
	遇到的问题：偶发的内存申请失败导致的宕机，性能上的提升
	怎么确定是内存碎片的问题，如何检查的
	怎么用的内存池
	如何测试的
	如何确定性能提升了

3、遇到过的死锁情况

```
有一说一，暂时还没遇到过死锁的场景
但是，每次传代码都会有检查和测试，静态代码检查
```

4、多线程宕机问题：应用层协议心跳数据包超时断开连接的时候，其他线程还在往外发数据，这个时候直接关闭IO会导致宕机。
	怎么检测定位的：
	如何解决的：
		直接断开，停止发送，停止接收，会不会有影响
		先设置标志位，等发完再调用handle_close()
		关闭连接的时候，还在发或者还在收数据，会不会有影响。		

5、应用层协议分包，设计网络缓冲区

6、协程

7、内存泄漏

```
太原地铁2号线项目，综合监控客户端集成，通过ATS软件启动ISCS系统，通过窗口句柄handle通信，在堆上申请内存把指针通过postmessage发送到对方。本来应该谁申请谁释放的，但是这个申请喝使用分别是两个软件，应该由使用者释放。

windows环境下查找内存泄漏的问题.
```

8、双主

9、hook技术

```shell
# dlsym
# hook 作为动态库使用
LD_PRELOAD=/path/to/preload/library.so./your_program

sudo addgroup -a <root> docker
```

10、内存池

```
发现内存够，内存碎片太多，导致申请内存失败
如何发现的，看日志发现内存申请失败，但是看对应时间的系统内存使用情况是够的
高网络IO、时刻表列车到离站信息是动态申请释放的

如何解决：
1、现场临时定期重启服务器，只能临时解决，服务器有专人定期维护，客户端一般都是长时间不会重启还是会有问题。
2、使用内存池进行优化

项目上使用boost库，使用了boost库的 std::boost::pool<> 
g++ -fPIC -shared -o libAlloc.so IAlloc.cpp CAlloc.cpp 
g++ -o demo demo.cpp -L. -lAlloc
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
./demo
```

11、工程化配置文件

```
配置文件读取
```

12、kafka 存报警文件

使用场景：

```
1、在一些多公司项目集成上，需要共享报警文件，读取MySQL耗时较多(每次都需要读取当天所有告警，100ms读取一次)
2、服务器跟多客户端之间同步数据，发生变化的数据传一次，每隔固定时间传所有数据，服务器缓存10000条告警数据。
```

设计

```
添加 kafka 消息队列 topic 用每天的日期
```

参数

```
broker ：4
topic ：年月日		根据时间段去划分
replication ：4，本来两个副本就可以，但是有4个broker
生产者：主服务器，产生报警，放到 kafka
消费者：hmi 客户端，其他公司的服务器.
```











多线程

1、网络通信模块，收，发

2、联挂解编  多车多线程，并发冲突，内存序

3、心跳超时关闭连接，为等待其他线程结束就释放了



优化：

1、网络数据包有UDP、TCP的
	TCP有粘包以及协议层分包的问题，无锁环形队列缓存
	UDP 就几种固定长度的数据包，用内存池去管理内存防止过多内存碎片

2、



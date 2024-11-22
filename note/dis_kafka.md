# kafka



> 理论
>
> 实践

## 一、理论1 （基本概念）

```
生产者Producer、消费者consumer、服务器Broker、消费组consumer group、主题topic、分区partition、副本replication、主副本leader、从副本follower、消息message、消费者消费的位置offset、管理集群zookeeper
```



## 二、实践 1 （命令行操作）

### 1、安装 kafka

```shell
# 1、先下载安装 java
sudo apt install default-jdk
# 2、再下载解压 kafka
wget https://archive.apache.org/dist/kafka/3.5.0/kafka-3.5.0-src.tgz   # 替换为最新版本的链接
# 3、解压缩
tar -xzf kafka-3.5.0-src.tgz
# 4、重命名方便使用
mv kafka-3.5.0-src kafka

```

#### 1.1 kafka 目录

```
bin			可执行文件
config		配置文件
libs		库文件
```



### 2、 zookeeper

#### 2.1 zookeeper config

```
zookeeper.properties
server.properties
```



#### 2.2 启动&  停止 zookeeper

```shell
# 前台运行
sh zookeeper-server-start.sh ../config/zookeeper.properties

# 后台运行
sh zookeeper-server-start.sh -deamon ../config/zookeeper.properties

# 停止
sh zookeeper-server.stop.sh

```



#### 2.3 相关命令

```shell
ps aux | grep zookeeper

# 默认端口 2181
lsof -i:2181
```



### 3、启动 fakfa 服务器

#### 3.1 kafka config

- 修改 server.properties （在 config 目录），增加zookeeper的配置

```shell
# server.properties 中原本无 zookeeper 配置 
zookeeper.connect=localhost:2181
```

- 启动 kafka

```shell
# -daemon	是否后台启动
sh kafka-server-start.sh -daemon ../config/server-properties
```

- 查看

```shell
# 默认端口 9092
lsof -i:9092
```

- 关闭

```shell
sh kafka-server-stop.sh -daemon ../config/server-propertes
```



### 4、kafka 命令行基本操作

#### 4.1 创建 topic

```shell
sh kafka-topics.sh --create --bootstrap-server localhost:9092 --replication-factor 1 --partitions 1 --topic lin
```

成功会显示：

> Created topic lin.

参数说明：

- --create 创建主题的动作指令
- --zookeeper 指定 kafka 所连接的 zookeeper 服务地址(Kafka 2.8.0 及以上版本，不再支持 zookeeper)
- --bootstrap-server 指定 Kafka 集群的一个或多个 broker 地址，以便客户端可以连接到集群.
- --parlication-factor 指定副本数量
- --partitions 指定分区个数
- --topic 指定要创建的主题名称

--replication-factor 1	创建 1个 副本
--partitions 1		创建 1个 分区
--topic lin                      主题 lin



#### 4.2 查看 topic

```shell
sh kafka-topics.sh --list --bootstrap-server localhost:9092
```



#### 4.3 查看 topic 属性

```shell
sh kafka-topics.sh --describe --bootstrap-server localhost:9092 --topic lin
```



#### 4.4 消费消息

```shell
sh kafka-console-consumer.sh --bootstrap-server 127.0.0.1:9092 --topic lin --from-beginning --group 1
# --from-beginning 从头开始消费
# --group 消费组
```



#### 4.5 发送消息

```shell
sh kafka-console-producer.sh --broker-list 127.0.0.1:9092 --topic lin
```

#### 4.6 kafka-topics.sh 使用方法

##### 4.6.1 查看帮助 --help

```shell
sh kafka-topics.sh --help
```



##### 4.6.2 副本数量不能大于 broker 数量

```shell
# 当 broker = 1 时，副本数量填 2以上都会报错
sh kafka-topics.sh --create --bootstrap-server localhost:9092 --replication-factor 2 --partitions 1 --topic lin
# 分区数量没有要求，但是影响性能影响顺序
sh kafka-topics.sh --create --bootstrap-server localhost:9092 --replication-factor 1 --partitions 1111 --topic lin
```



##### 4.6.3 创建主题	--create

必须填的三个参数：

- --partition	分区数量
- --topic		主题名
- --replication-factor	副本数量（备份几次）

```shell
sh kafka-topics.sh --create --bootstrap-server localhost:9092 --replication-factor 1 --partitions 1 --topic lin
```

其他参数：

- --if-not-exists
- --if-exists

##### 4.6.4 查看 broker 上所有的主题	--list

```shell
sh kafka-topics.sh --list --bootstrap-server localhost:9092
```



##### 4.6.5 查看指定主题 topic 的详细信息	--describe

```shell
sh kafka-topics.sh --describe --bootstrap-server localhost:9092 --topic lin
```



##### 4.6.6 增加主题分区数量	--alter

```shell
# 增加到 33 个
sh kafka-topics.sh --alter --bootstrap-server localhost:9092 --partitions 33 --topic lin

# look
sh kafka-topics.sh --describe --bootstrap-server localhost:9092 --topic lin
```

注意事项：

- 不要使用 --alter 去常识减少分区的数量
- 减少分区的数量，只能先删除整个主题 topic 再重新创建

##### 4.6.7 删除主题 topic	--delete

```shell
# 删除
sh kafka-topics.sh --delete --bootstrap-server localhost:9092 --topic test

# 查看
sh kafka-topics.sh --list --bootstrap-server localhost:9092
```

需要修改 /config/server.properties 配置

```ini
# topic setting
delete.topic.enable=true
```

重启 kafka 服务器

```shell
bash kafka-server.stop.sh -daemon ../config/server.properties
bash kafka-server.start.sh -daemon ../config/server.properties
```



## 三、理论 2 （存储机制和可靠性）

### 1、集群

### 2、存储机制

### 3、高吞吐：存储机制（顺序性/零拷贝）

### 4、可靠性

## 四、实践 2 （编码使用）

### 1、安装 librdkafka

```shell
git clone https://github.com/edenhill/librdkafka.git
cd librdkafka
git checkout v1.7.0
./configure
make
sudo make install
sudo ldconfig
```

### 2、C++ API

#### 2.1 RdKafka::Conf 

```c++
enum ConfType{
    CONF_GLOBAL,	// 全局配置
    CONF_TOPIC		// Topic 配置
};

// 用来处理返回值
enum ConfResult{
    CONF_UNKNOWN = -2,		// 表示配置状态未知
    CONF_INVALID = -1,		// 表示配置无效
    CONF_OK = 0				// 表示配置有效且成功
}

// 创建配置对象
static Conf* create(ConfType type);

// 设置对象的属性值，成功返回 CONF_OK, 错误时错误信息输出到 errstr
Conf::ConfResult set(const std::string& name, const std::string& value, std::string& errstr);

// 设置 dr_cb 属性值
Conf::ConfResult set(const std::string& name, DeliveryReportCb* dr_cb, std::string& errstr);

// 设置 event_cb 属性值
Conf::ConfResult set(const std::string& name, EventCb* event_cb, std::string& errstr);

// 设置用于自动订阅 Topic 的默认 Topic 配置
Conf::ConfResult set(const std::string& name, const Conf* topic_conf, std::string& errstr);

// 设置 partitioner_cb 属性值，配置对象必须是CONF_TOPIC类型
Conf::ConfResult set(const std::string& name, PartitionerCb* partitioner_cb, std::string& errstr);

// 设置 partitioner_key_pointer_cb 属性值
Conf::ConfResult set(const std::string& name, PartitionerKeyPointerCb* partitioner_kp_cb, std::string& errstr);

// 设置 socket_cb 属性值
Conf::ConfResult set(const std::string& name, SocketCb* socket_cb, std::string& errstr);

// 设置 open_cb 属性值
Conf::ConfResult set(const std::string& name, OpenCb* open_cb, std::string& errstr);

// 设置 rebalance_cb 属性值
Conf::ConfResult set(const std::string& name, RebalanceCb* rebalance_cb, std::string& errstr);

// 设置 offset_commit_cb 属性值
Conf::ConfResult set(const std::string& name, OffsetCommitCb* offset_commit_cb, std::string& errstr);

// 查询单条属性配置值
Conf::ConfResult get(const std::string& name, std::string& value) const;

// 按 name, value 元组序列化配置对象的属性名称和属性值到链表
std::liststd::string* dump();

// 如果是CONF_GLOBAL类型配置对象，返回底层数据结构 rd_kafka_conf_t 句柄，否则返回NULL
virtual struct rd_kafka_conf_s* c_ptr_global() = 0;

// 如果是 CONF_TOPIC 类型配置对象，返回底层数据结构的 rd_kafka_topic_conf_t 句柄，否则返回0
virtual struct rd_kafka_topic_conf_s* c_ptr_topic() = 0;

```



#### 2.2 RdKafka::Topic

```c++
// 使用 conf 配置创建名为 topic_str 的 Topic 句柄
static Topic* create(Handle* base, const std::string& topic_str, Conf* conf, std::string& errstr);

// 获取 Topic 名称
const std::string name();

// 获取 parition 分区是否可用，只能在 RdKafka::PartitionerCb 回调函数内被调用
bool partition_available(int32_t partition) const;

// 存储 Topic 的 partition 分区的 offset 位移，只能用于 RdKafka::Consumer，不能用于 RdKafka::KafkaConsumer 高级接口类.使用本接口时，auto.commit.enable 参数必须设置为 false
ErrorCode offset_store(int32_t partition, int64_t offset);

// 返回底层数据结构的 rd_kafka_topic_t 句柄，不推荐利用 rd_kafka_topic_ t句柄调用 C API，但如果 C++ API没有提供相应功能，可以直接使用 C API 和 librdkafka 核心交互
virtual struct rd_kafka_topic_s* c_ptr() = 0;

// 未赋值分区
static const int32_t PARTITION_UA = -1;

// 特殊位移，从开始消费
static const int64_t OFFSET_BEGINNING = -2;

// 特殊位移，从末尾消费
static const int64_t OFFSET_END = -1;

// 使用 offset 存储
static const int64_t OFFSET_STORED = -1000;

```

#### 2.3 RdKafka::Message

Message 表示一条消费或生产的消息，或是事件.

```c++
// 如果消息是一条错误事件，返回错误字符串，否则返回控字符串
std::string errstr() const;

// 如果消息是一条错误事件，返回错误代码，否则返回0
ErrorCode err() const;

// 返回消息的 Topic 对象.如果消息的 Topic 对象没有显示使用 RdKafka::Topic::create() 创建，需要使用 topic_name 函数
Topic* topic() const;

// 返回消息的 Topic 名称
std::string topic_name() const;

// 如果分区可用，返回分区号
int32_t partition() const;

// 返回消息数据
void* payload() const;

// 返回消息数据的长度
size_t len() const;

// 返回字符串类型的消息key
const std::string* key() const;

// 返回void类型的消息key
const void* key_pointer() const;

// 返回消息key的二进制长度
size_t key_len() const;

// 返回消息或错误的位移
int64_t offset() const;

// 返回 RdKafka::Producer::produce() 提供的 msg_opaque
void* msg_opaque() const;

// 返回消息时间戳
virtual MessageTimestamp timestamp() const = 0;

// 返回 produce 函数内生产消息的微秒级时间延迟，如果延迟不可用，返回-1
virtual int64_t latency() const = 0;

// 返回底层数据结构的C rd_kafka_message_t 句柄
virtual struct rd_kafka_message_s* c_ptr() = 0;

// 返回消息在 Topic Log 的持久化状态
virtual Status status() const = 0;

// 返回消息头
virtual RdKafka::Headers* headers() = 0;

// 返回消息头，错误信息会输出到 err
virtual RdKafka::Headers* headers(RdKafka::ErrorCode* err) = 0;

```

#### 2.4 RdKafka::TopicPartition

```c++
// 创建一个 TopicPartition 对象
static TopicPartition* create(const std::string& topic, int partition);

// 创建 TopicPartition 对象（比上边多一个偏移量）
static TopicPartition* create(const std::string& topic, int partition, int64_t offset); 

// 销毁所有 TopicPartition 对象
static void destroy(std::vector<TopicPartition*>& partitions);

// 返回Topic名称
const std::string& topic() const;

// 返回分区号
int partition();

// 返回位移
int64_t offset();

// 设置位移
void set_offset(int64_t offset);

// 返回错误码
ErrorCode err();

```

#### 2.5 RdKafka::Handle

客户端handle基类

```c++
// 返回 handle 名称
const std::string name();

// 返回客户端成员Id
const std::string memberid() const;

// 轮询处理指定的 Kafka 句柄的 Event，返回事件数量.事件会触发应用程序提供的回调函数调用.timeout_ms参数指定回调函数指定阻塞等待的最大时间间隔；对于非阻塞调用，指定 timeout_ms 参数为0；永远等待事件，设置 timeout_ms 参数为-1.RdKafka::KafkaConsumer实例禁止使用 poll 方法，使用 RdKafka::KafkaConsumer::consume() 方法代替.
int poll(int timeout_ms);

// 返回当前出队列的长度，出队列包含等待发送到Broker的消息、请求和Broker要确认的消息、请求.
int outq_len();

// 从Broker请求元数据，成功返回 RdKafka::ERR_NO_ERROR，超时返回 RdKafka::ERR_TIMED_OUT，错误返回其它错误码.
ErrorCode metadata(bool all_topics, const Topic* only_rkt, Metadata** metadatap, int timeout_ms);

// 暂停分区链表中分区的消费和生产，返回ErrorCode::NO_ERROR.partitions中分区会返回成功或错误信息.
virtual ErrorCode pause(std::vector<TopicPartition*>& partitions) = 0;

// 恢复分区链表中分区的生产和消费，返回 ErrorCode::NO_ERROR.
// partitions 中分区会返回成功或错误信息.
virtual ErrorCode resume(std::vector<TopicPartition*>& partitions) = 0;

// 查询topic主题partition分区的高水位和低水位，高水位输出到high，低水位输出到low，成功返回 RdKafka::ERR_NO_ERROR，失败返回错误码.
virtual ErrorCode query_watermark_offsets(const std::string& topic, int32_t partition, int64_t* low, int64_t* high, int timeout_ms) = 0;

```

#### 2.6 RdKafka::Producer

```c++
// 创建一个新的 Producer 客户端对象，conf 用于替换默认配置对象，本函数调用后 conf 可以重用.
// 成功返回新的 Producer 客户端对象，失败返回 NULL，errstr 可读错误信息
static Producer* create(Conf* conf, std::string& errstr);

// 生产和发送单条消息到 Broker.
ErrorCode produce(Topic* topic, int32_t partition, int msgflags, void* payload, size_t len, const std::string* key, void* msg_opaque);
/*
- topic : 主题
- partition : 分区
- msgflags : 可选项：
		RK_MSG_BLOCK	在消息队列满时阻塞produce函数，如果dr_cb回调函数被使用，应用程序必须调用		
		rd_kafka_poll函数确保消息队列的投递消息投递完.当消息队列满时，失败会导致produce函数的永久阻塞.
		RK_MSG_FREE		RdKafka 调用produce完成后会释放 payload 数据
		RK_MSG_COPY		表示 payload 数据会被拷贝，在 Produce 调用完成后指针指向的内存不变.
		另外：
			RK_MSG_FREE、RK_MSG_COPY 互斥
			如果produce函数调用时，指定了RK_MSG_FREE, 并返回了错误码，
			与payload指针相关的内存数据必须由使用者释放.
- payload : 成都为len的小修负载数据
- len : payload消息数据的长度
- key : key 可选择，key = NULL时，kafka将消息随机分配到目标主题的可用分区中，无法保证顺序.
				 key != NULL时，kafka将消息分配到特定分区，可以用来保证相同key消息的顺序性.
- msg_opaque : msg_opaque 可选的应用程序提供给每条消息的 opaque 指针，opaque 指针会在dr_cb回调函数内提供.
- 返回错误码：
	ERR_NO_ERROR : 消息成功发送并入队列.
	ERR_QUEUE_FULL : 最大消息数量达到 queue.buffering.max.message.
	ERR_MSG_SIZE_TOO_LARGE : 消息数据大小太大，超过 messages.max.bytes 配置的值.
	ERR_UNKNOWN_PARTITION : 请求一个 kafka 集群内的未知分区.
	ERR_UNKNOWN_TOPIC : topic 是 kafka集群的未知主题.
*/

// 生产和发送单条消息到Broker，传递key数据指针和key长度
ErrorCode produce(Topic* topic, int32_t partition, int msgflags, void* payload, size_t len, const void* key, size_t key_len, void* msg_opaque);

// 生产和发送单条消息到Broker，传递消息数据和key数组
ErrorCode produce(Topic* topic, int32_t partition, const std::vector<char>* payload, const std::vector<char>* key, void* msg_opaque);

// 生产和发送消息到Broker，接收数组类型的key 和 payload，数组会被复制
ErrorCode produce(Topic* topic, int32_t partition, const std::vector<char>* payload, const std::vector<char>* key, void* msg_opaque);

// 等待所有未完成的所有produce请求完成
// 为了确保所有队列和已经执行的Produce请求在中止前完成，flush操作优先于销毁生产者实例完成.
// 本函数会调用Producer::poll()函数，因此会触发回调函数.
ErroeCode flush(int timeout_ms);


// 清理生产者当前处理的消息.本函数调用时可能会阻塞一定时间，当后台线程队列在清理时.应用程序需要在调用poll或flush函数后，执行清理消息的dr_cb回调函数.
ErrorCode purge(int timeout_ms);

// 初始化Producer实例的事务.
// 失败返回 RdKafka::Error 错误对象，成功返回NULL.
// 通过调用 RdKafka::Error::is_retriable() 函数可以检查返回的错误对象是否有权限重试，调用 RdKafka::Error::is_fatal()检查返回的错误对象是否是严重错误.返回的错误对象必须delete.
virtual Error* init_transactions(int timeout_ms) = 0;

// 启动事务.
// 本函数调用前，init_transactions()函数必须被成功调用.成功返回NULL，失败返回错误对象.通过调用RdKafka::Error::is_fatal_error()函数可以检查是否是严重错误，返回的错误对象必须delete.
virtual Error* begin_transaction() = 0;

// 发送TopicPartition位移链表到由group_metadata指定的Consumer Group协调器，如果事务提交成功，位移才会被提交.
virtual Error send_offsets_to_transaction(const std::vector<TopicPartition>& offsets, const ConsumerGroupMetadata* group_metadata, int timeout_ms) = 0;

// 提交当前事务.在实际提交事务时，任何未完成的消息会被完成投递.
// 成功返回NULL，失败返回错误对象.通过调用错误对象的方法可以检查是否有权限重试，是否是严重错误、可中止错误等.
virtual Error* commit_transaction(int timeout_ms) = 0;

// 停止事务.本函数从非严重错误、可终止事务中用于恢复.
// 未完成消息会被清理.
virtual Error* abort_transaction(int timeout_ms) = 0;



```

#### 2.7 RdKafka::Consumer

RdKafka::Consumer 是简单的非Rebalance、非Group的消费者

```c++
// 创建一个 Kafka Consumer 客户端对象
static Consumer* create(Conf* conf, std::string& errstr);

// 从 Topic 尾部转换位移为逻辑位移
static int64_t OffsetTail(int64_t offset);

// 从topic主题partition分区的offset位移开始消费消息，offset可以是普通位移，也可以是OFFSET_BEGINNING或OFFSET_END，rdkafka会试图从Broker重复拉取批量消息到本地队列使其维持queued.min.messages参数值数量的消息。start函数在没有调用stop函数停止消费时不能对同一个TopicPartition调用多次。
// 应用程序会使用consume函数从本地队列消费消息。
ErrorCode start(Topic* topic, int32_t partition, int64_t offset);

// 在消息队列queue的topic主题的partition分区开始消费。
ErrorCode start(Topic* topic, int32_t partition, int64_t offset, Queue* queue);

// 停止从topic主题的partition分区消费消息，并清理本地队列的所有消息。应用程序需要在销毁所有 Consumer对象前停止所有消费者。
ErrorCode stop(Topic* topic, int32_t partition);

// 定位 topic 的 partition 分区的 Consumer 位移到 offset
ErrorCode seek(Topic* topic, int32_t partition, int64_t offset, int timeout_ms);

// 从topic主题和partition分区消费一条消息。timeout_ms是等待获取消息的最大时间。消费者必须提前调用start函数。应用程序需要检查消费的消息是正常消息还是错误消息。应用程序完成时消息对象必须销毁。
Message* consume(Topic* topic, int32_t partition, int timeout_ms);

// 从指定消息队列queue消费一条消息
Message* consume(Queue* queue, int timeout_ms);

// 从topic主题和partition分区消费消息，并对每条消费的消息使用指定回调函数处理。
// consume_callback提供了比consume更高的吞吐量。
// opaque参数回被传递给consume_cb的回调函数。
int consume_callback(Topic* topic, int32_t partition, int timeout_ms, ConsumeCb* consume_cb, void* opaque);

// 从消息队列queue消费消息，并对每条消费的消息使用指定回调函数处理。
int consume_callback(Queue* queue, int timeout_ms, RdKafka::ConsumeCb* consume_cb, void* opaque);

```

#### 2.8 RdKafka::KafkaConsumer

```

```



### 3、kafka Producer C++

#### 3.1 RdKafka Producer 使用流程

##### 3.1.1、创建kafka配置实例(Conf)

```c++
RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
```

##### 3.1.2、创建topic配置实例

```c++
RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
```

##### 3.1.3、设置Kafka配置实例Broker属性

```c++
RdKafka::Conf::ConfResult RdKafka::Conf::set(const std::string& name, const std::string& value, std::string& errstr);
```

##### 3.1.4、设置topic配置实例属性

```c++
RdKafka::Conf::ConfResult RdKafka::Conf::set(const std::string& name, const std::string& value, std::string& errstr);
```

##### 3.1.5、注册回调函数

```c++
//事件是从RdKafka传递错误、统计信息、日志等消息到应用程序的通用接口
Conf::ConfResult RdKafka::Conf::set("event_cb", RdKafka::EventCb* dr_cb, std::string& errstr);
//SocketCb回调函数用于打开一个Socket套接字
Conf::ConfResult RdKafka::Conf::set("socket_cb", RdKafka::SocketCb* socket_cb, std::string& errstr);
//Open回调函数用于使用flags、mode打开指定path的文件
Conf::ConfResult RdKafka::Conf::set("open_cb", RdKafka::OpenCb* open_cb, std::string& errstr);
//用于RdKafka::KafkaConsunmer的组再平衡回调函数
Conf::ConfResult RdKafka::Conf::set("rebalance_cb", RdKafka::RebalanceCb* rebalance_cb, std::string& errstr);
//用于消费者组的位移提交回调函数
Conf::ConfResult RdKafka::Conf::set("offset_commit_cb", RdKafka::OffsetCommitCb* offset_commit_cb, std::string& errstr);
// 对消费的每条消息会调用ConsumeCb回调函数
Conf::ConfResult RdKafka::Conf::set("consume_cb", RdKafka::ConsumeCb* consume_cb, std::string& errstr);

```

分区策略回调函数需要注册到 Topic 配置实例：

```c++
Conf::ConfResult RdKafka::Conf::set("partitioner_cb", RdKafka::PartitionerCb* dr_cb, std::string& errstr);
Conf::ConfResult RdKafka::Conf::set("partitioner_key_pointer_cb", RdKafka::PartitionerKeyPointerCb* dr_cb, std::string& errstr);

```

##### 3.1.6、创建Kafka Producer 客户端实例

```c++
static RdKafka::Producer* RdKafka::Producer::create(RdKafka::Conf* conf, std::string& errstr);
// conf为Kafka配置实例

```

##### 3.1.7、创建 Topic 实例

```c++
static RdKafka::Topic* RdKafka::Topic::create(RdKafka::Handle* base, 
                                              const std::string& topic_str, 
                                              RdKafka::Conf* conf, 
                                              std::string& errstr);
// conf为Topic配置实例

```

##### 3.1.8、生产消息

```C++
RdKafka::ErrorCode RdKafka::Producer::produce(RdKafka::Topic* topic,
                                              int32_t partition,
                                              int msgflags,
                                              void* payload,
                                              size_t len,
                                              const std::string* key,
                                              void* msg_opaque);

```

##### 3.1.9、阻塞等待Producer生产消息完成

```C++
int RdKafka::Producer::poll(int timeout_ms);
```

##### 3.1.10、等待Produce请求完成

```c++
RdKafka::ErrorCode RdKafka::Producer::flush(int timeout_ms);
```

##### 3.1.11、销毁Kafka Producer客户端实例

```c++
int RdKafka::wait_destroyed(int timeout_ms);
```

#### 3.2 Kafka Producer实例

##### 3.2.1、KafkaProducer.h文件：

```C++

```

##### 3.2.2、KafkaProducer.cpp 文件：

```c++

```

##### 3.2.3、main.cpp 文件：

```

```

##### 3.2.4、CMakeLists.txt 文件：

```

```

#### 3.3 Kafka 消息查看

查看topic消息

```shell
kafka-console-consumer.sh --bootstrap-server localhost:9092 --topic test --form-beginning
```



### 4、kafka consumer C++

#### 4.1 Kafka Consumer 使用流程



### 5、librdkafka 配置参数



## 五、实践 3 （集群部署、配置文件参数调优）




























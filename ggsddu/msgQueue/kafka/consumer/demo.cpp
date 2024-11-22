#include "KafkaConsumer.h"

int main(int argc, char* argv[])
{
    std::string brokers = "127.0.0.1:9092";
    std::vector<std::string> topics;
    topics.push_back("test");
    // topics.push_back("test2");
    std::string group = "testGroup";
    if(argc >= 2) {
        group = argv[1];
    }
    std::cout << "group " << group << std::endl;
    
    KafkaConsumer consumer(brokers, group, topics, RdKafka::Topic::OFFSET_BEGINNING);
    consumer.pullMessage();

    RdKafka::wait_destroyed(5000);
    return 0;
}
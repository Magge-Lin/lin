
/*
    使用原子操作实现的无锁消息队列
*/

#include <atomic>
#include <memory>
#include <iostream>
#include <thread>
#include <vector>

template <typename T>
class LockFreeQueue {
private:
    struct Node {
        std::shared_ptr<T> data;
        std::atomic<Node*> next;

        Node() : next(nullptr) {}
        Node(T value) : data(std::make_shared<T>(value)), next(nullptr) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;

public:
    LockFreeQueue() {
        Node* dummy = new Node;  // 创建一个哨兵节点
        head.store(dummy);
        tail.store(dummy);
    }

    ~LockFreeQueue() {
        while (Node* old_head = head.load()) {
            head.store(old_head->next);
            delete old_head;
        }
    }

    // 入队操作
    void enqueue(T value) {
        Node* new_node = new Node(value);
        Node* old_tail;

        while (true) {
            old_tail = tail.load();
            Node* next = old_tail->next.load();

            if (old_tail == tail.load()) {  // 确保tail没有变化
                if (next == nullptr) {
                    // 尝试将next设置为新节点
                    if (old_tail->next.compare_exchange_weak(next, new_node)) {
                        // 更新tail指针
                        tail.compare_exchange_strong(old_tail, new_node);
                        return;
                    }
                } else {
                    // tail指向旧尾后面的节点
                    tail.compare_exchange_strong(old_tail, next);
                }
            }
        }
    }

    // 出队操作
    std::shared_ptr<T> dequeue() {
        Node* old_head;

        while (true) {
            old_head = head.load();
            Node* old_tail = tail.load();
            Node* next = old_head->next.load();

            if (old_head == head.load()) {  // 确保head没有变化
                if (old_head == old_tail) {  // 队列可能为空
                    if (next == nullptr) {
                        return nullptr;  // 队列为空
                    }
                    // tail落后，尝试更新
                    tail.compare_exchange_strong(old_tail, next);
                } else {
                    // 尝试获取数据
                    std::shared_ptr<T> res = next->data;
                    if (head.compare_exchange_strong(old_head, next)) {
                        delete old_head;  // 释放旧头节点
                        return res;
                    }
                }
            }
        }
    }
};

/*
                example
*/

int main() {
    LockFreeQueue<int> queue;

    // 启动生产者线程
    auto producer = [&queue]() {
        for (int i = 0; i < 10; ++i) {
            queue.enqueue(i);
            std::cout << "Produced: " << i << std::endl;
        }
    };

    // 启动消费者线程
    auto consumer = [&queue]() {
        for (int i = 0; i < 10; ++i) {
            while (true) {
                auto value = queue.dequeue();
                if (value) {
                    std::cout << "Consumed: " << *value << std::endl;
                    break;
                }
            }
        }
    };

    std::thread prod_thread(producer);
    std::thread cons_thread(consumer);

    prod_thread.join();
    cons_thread.join();

    return 0;
}

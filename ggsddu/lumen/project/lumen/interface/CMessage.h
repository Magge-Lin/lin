
#ifndef CMESSAGE_H
#define CMESSAGE_H

#include "interface/IObject.h"
#include "interface/Common.h"
#include "interface/IAlloc.h"
#include "interface/CMessageDataBlock.h"

#include <iostream>
#include <vector>
#include <any>
#include <memory>
#include <cstdint>

/*
        该类描述了消息,可以通过消息服务框架来发送和接受消息。
*/
const uint16_t MSGID_UNKNOWN                = 0;


class CMessageDataBlock;
DECLARE_PTR_TYPE(CMessageDataBlock)

class CMessage;
DECLARE_PTR_TYPE(CMessage)


class CMessage
    : public IObject
{
public:

    // 通过消息类型等参数来构造CMessage
    CMessage(void);

    // 通过消息类型等参数来构造CMessage
    explicit CMessage(const uint16_t msgId, const bool bNet = false, const uint16_t nodeId = 0U);

    // 通过消息缓冲区等参数来构造CMessage
    explicit CMessage(const int8_t* const message, const uint16_t len, const bool bNet = false);

    // 通过消息类型等参数来初始化CMessage
    void initMessage(const uint16_t msgId = MSGID_UNKNOWN, const uint16_t nodeId = 0U);

    // 通过消息缓冲区等参数来初始化CMessage
    void initMessage(const int8_t* const message, const uint16_t len);

    // CMessage的析构函数
    virtual ~CMessage(void);

    // 获得消息ID
    uint16_t getMessageId(void) const;

    // 获得消息节点ID
    uint16_t getNodeId(void) const;

    // 设置消息节点ID
    void setNodeId(const uint16_t nodeId);

    // 获得消息缓冲区指针
    const int8_t* getBuffer(void);

    // 获得消息的长度
    uint16_t getLength(void) const;

    // 复位消息的读写位置
    void reset(void);

    // 该消息是否有效
    bool isValid(void) const;

    // 该消息是否为网络消息
    bool isNet(void) const;

    // 重载操作符<<,用于输入 uint16_t 类型数据到消息
    CMessage& operator << (const uint16_t& usv);
    // 重载操作符,用于从消息中输出uint16_t类型数据
    CMessage& operator >> (uint16_t& usv);

    // 重载操作符<<,用于输入 int16_t 类型数据到消息
    CMessage& operator << (const int16_t& sv);
    // 重载操作符,用于从消息中输出 int16_t 类型数据
    CMessage& operator >> (int16_t& sv);

    // 重载操作符<<,用于输入 uint32_t 类型数据到消息
    CMessage& operator << (const uint32_t& uintv);
    // 重载操作符>>,用于从消息中输出 uint32_t 类型数据
    CMessage& operator >> (uint32_t& uintv);

    // 重载操作符<<,用于输入 int32_t 类型数据到消息
    CMessage& operator << (const int32_t& intv);
    // 重载操作符>>,用于从消息中输出 int32_t 类型数据
    CMessage& operator >> (int32_t& intv);


    // 重载操作符<<,用于输入std::any类型数据到消息
    CMessage& operator << (const std::any& value);
    CMessage& operator >> (std::any& value);


    void put(const uint16_t& uint16_tv);
    void get(uint16_t& uint16_tv);

    void put(const int16_t& shortv);
    void get(int16_t& shortv);

    void put(const uint32_t& uintv);
    void get(uint32_t& uintv);

    void put(const int32_t& intv);
    void get(int32_t& intv);

    void put(const std::any& anyv);
    void get(std::any& anyv);

    void put(const std::string& strv);
    void get(std::string& strv);

    void put(const IObjectPtr& obj);
    void get(IObjectPtr& obj);

    void put(const IObjectPtr* const obj);
    void get(IObjectPtr* const obj);


private:

    // 对weak_ptr对象进行内存管理
    template<typename T>
    class malloc_weak_ptr
        : public std::weak_ptr<T>
        {
        public:
            // 类型构造函数
            malloc_weak_ptr() : std::weak_ptr<T>() {}

            // 重载new运算符
            void* operator new(size_t size)
            {
                return IAlloc::instance()->malloc(size);
            }

            // 重载delete运算符
            void operator delete(void* p, size_t size)
            {
                IAlloc::instance()->free(p, size);
            }
        };

private:

    // 消息类型
    uint16_t m_messageId;

    // 节点类型
    uint16_t m_nodeId;

    // 消息数据块
    CMessageDataBlockPtr m_messageDataBlock;

    // 消息是否有效
    bool m_valid;

    // 是否网络消息
    bool m_bNet;

};









#endif
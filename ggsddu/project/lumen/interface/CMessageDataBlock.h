#ifndef CMESSAGEDATABLOCK_H
#define CMESSAGEDATABLOCK_H

#include "interface/Common.h"
#include "interface/IObject.h"
#include "interface/MessageId.h"
#include "interface/IAlloc.h"

#include <iostream>
#include <map>
#include <vector>
#include <any>

class CMessageDataBlock;
DECLARE_PTR_TYPE(CMessageDataBlock)




class CMessageDataBlock
{
public:

    // CMessageDataBlock 的构造函数
    CMessageDataBlock(void);

    // CMessageDataBlock 的析构函数
    virtual ~CMessageDataBlock(void);

    // 初始化消息数据块,数据的写入位置指向首部
    void initialize(void);

    // 使用输入数据来初始化消息数据块
    void initialize(const int8_t* const data, const size_t size);

    // 获得消息数据块的缓冲区
    const int8_t* getBuffer(void) const;

    //  获得消息数据块的长度
    int32_t getLength(void) const;

    // 设置消息数据块的长度
    void setLength(const uint16_t length);

    // 获得消息数据块的写入位置
    int32_t getPosition(void) const;

    // 设置消息数据块的写入位置
    void setPosition(const int32_t pos);

    // 放入指定类型的数据到数据块中
    template<typename T>
    void put(const T& data)
    {
        int32_t needSize = sizeof(T) + m_length;

        if (needSize > m_blockSize)
        {
            remalloc(needSize);
        }

        memcpy(m_position, &data, sizeof(T));
        m_position += sizeof(T);
        m_length +=  sizeof(T);
    }

    // 从数据块中获取指定类型的数据
    template<typename T>
    void get(T& data)
    {
        memcpy(&data, m_position, sizeof(T));
        m_position += sizeof(T);
    }

    // 放入数据到数据块中
    void put(const void* const data, const size_t size);

    // 从数据块中获取数据
    void get(void* const data, const size_t size);

    // 根据位置和数据来设置数据块的缓冲区
    template<typename T>
    void setBuf(int32_t pos, const T& data)
    {
        memcpy(&m_buffer[pos], &data, sizeof(T));
    }

    // 根据位置和数据来设置数据块的缓冲区
    template<typename T>
    void getBuf(int32_t pos, T& data)
    {
        memcpy(&data, &m_buffer[pos], sizeof(T));
    }

    // 获得数据块的尺寸
    int32_t getBlockSize(void) const;

private:

    // 重新分配指定尺寸的内存空间
    void* remalloc(const size_t needSize);

    // 数据块的最大尺寸
    uint16_t m_blockSize;

    // 数据块的有效数据长度
    uint16_t m_length;

    // 数据块的缓冲区指针
    int8_t* m_buffer;

    // 每次增长的数据块尺寸
    static const uint16_t INCREASE_DATABLOCK_SIZE = 512U;

    // 当前指向数据块的指针
    int8_t* m_position;
};



#endif // #ifndef CMESSAGEDATABLOCK_H
 
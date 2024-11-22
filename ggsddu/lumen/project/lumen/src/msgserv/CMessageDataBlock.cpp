#include "interface/CMessageDataBlock.h"

#include <cstring>
#include <vector>
#include <cassert>


CMessageDataBlock::CMessageDataBlock(void)
: m_position(NULL)
, m_buffer(NULL)
, m_length(0U)

{
    m_blockSize = static_cast<uint16_t>(INCREASE_DATABLOCK_SIZE);
    m_buffer = static_cast<int8_t*>(IAlloc::instance()->malloc(m_blockSize));
    m_position = m_buffer;
}

CMessageDataBlock::~CMessageDataBlock(void)
{
    if (m_buffer != NULL)
    {
        IAlloc::instance()->free(m_buffer, m_blockSize);
    }
}

const int8_t* CMessageDataBlock::getBuffer(void) const
{
    return (m_buffer);
}

int32_t CMessageDataBlock::getLength(void) const
{
    return (m_length);
}

void CMessageDataBlock::setLength(const uint16_t length)
{
    m_length = length;
}

int32_t CMessageDataBlock::getPosition(void) const
{
    return static_cast<int32_t>(m_buffer - m_position);
}

void CMessageDataBlock::setPosition(const int32_t pos)
{
    m_position = m_buffer + pos;
}

void CMessageDataBlock::initialize(void)
{
    setLength(0U);
    setPosition(0);
}

int32_t CMessageDataBlock::getBlockSize(void) const
{
    return (m_blockSize);
}

void CMessageDataBlock::put(const void* const data, const size_t size)
{
    if ((data != NULL) && (size != 0U))
    {
        const uint32_t needSize = size + static_cast<uint32_t>(m_length);

        if (static_cast<uint16_t>(needSize) > m_blockSize)
        {
            (void)remalloc(needSize);
        }

        (void)memcpy(m_position, data, size);
        m_position += size;
        m_length += static_cast<uint16_t>(size);
    }
}

void CMessageDataBlock::get(void* const data, const size_t size)
{
    if ((data != NULL) && (size != 0U))
    {
        (void)memcpy(data, m_position, size);
        m_position += size;
    }
}

void CMessageDataBlock::initialize(const int8_t* const data, const size_t size)
{
    if ((data != NULL) && (size != 0U))
    {
        if (size > m_blockSize)
        {
            (void)remalloc(size);
        }

        (void)memcpy(m_buffer, data, size);
        m_position = m_buffer;
        m_length = static_cast<uint16_t>(size);
    }
}

void* CMessageDataBlock::remalloc(const size_t needSize)
{
    int8_t* buffer = NULL;

    if (needSize != 0U)
    {
        assert(needSize > m_blockSize);
        const size_t prevBlockSize = m_blockSize;

        if ((static_cast<uint16_t>(needSize) % INCREASE_DATABLOCK_SIZE) != 0U)
        {
            m_blockSize = ((static_cast<uint16_t>(needSize) / INCREASE_DATABLOCK_SIZE) + 1U) * INCREASE_DATABLOCK_SIZE;
        }
        else
        {
            m_blockSize = static_cast<uint16_t>(needSize);
        }

        int8_t* const prevBuffer = m_buffer;
        m_buffer = static_cast<int8_t*>(IAlloc::instance()->malloc(m_blockSize));

        if (m_length > 0U)
        {
            (void)memcpy(m_buffer, prevBuffer, m_length);
        }

        m_position = m_buffer + m_length;
        IAlloc::instance()->free(prevBuffer, prevBlockSize);
        buffer = m_buffer;
    }

    return buffer;
}

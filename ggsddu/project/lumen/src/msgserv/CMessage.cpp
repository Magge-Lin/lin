#include "interface/CMessage.h"
#include "interface/MessageId.h"

#include <iostream>
#include <arpa/inet.h>
#include <cassert>

CMessage::CMessage()
: IObject()
, m_messageId(MSGID_UNKNOWN)
, m_nodeId(0U)
, m_bNet(false)
, m_messageDataBlock(new CMessageDataBlock)
{
    initMessage(m_messageId, m_nodeId);
}

CMessage::CMessage(const uint16_t msgId, const bool bNet, const uint16_t nodeId)
: IObject()
, m_messageId(msgId)
, m_nodeId(nodeId)
, m_bNet(bNet)
, m_messageDataBlock(new CMessageDataBlock)
{
    initMessage(msgId, nodeId);
}

CMessage::CMessage(const int8_t* const message, const uint16_t len, const bool bNet)
: IObject()
, m_bNet(bNet)
, m_messageDataBlock(new CMessageDataBlock)
{
    initMessage(message, len);
}

CMessage::~CMessage(void)
{
}

//msg struct :
//sync header(eb 90 eb 90)
//length
//msg id
//node id
//data
void CMessage::initMessage(const uint16_t msgId, const uint16_t nodeId)
{
    m_messageId = msgId;
    m_nodeId = nodeId;
    m_messageDataBlock->initialize();
    //head  4byte
    m_messageDataBlock->put(static_cast<uint8_t>(0xeb));
    m_messageDataBlock->put(static_cast<uint8_t>(0x90));
    m_messageDataBlock->put(static_cast<uint8_t>(0xeb));
    m_messageDataBlock->put(static_cast<uint8_t>(0x90));
    //length skip length field later setting
    m_messageDataBlock->put(static_cast<uint16_t>(0U));

    if (m_bNet == true)
    {
        const uint16_t tempMsgId = htons(m_messageId);
        m_messageDataBlock->put(tempMsgId);
        const uint16_t tempNodeId = htons(m_nodeId);
        m_messageDataBlock->put(tempNodeId);
    }
    else
    {
        m_messageDataBlock->put(m_messageId);
        m_messageDataBlock->put(m_nodeId);
    }
}

void CMessage::initMessage(const int8_t* const message, const uint16_t len)
{
    if (message != NULL)
    {
        assert(len >= 10);
        m_messageDataBlock->initialize(message, len);
        uint8_t syncWord[4];
        m_messageDataBlock->get(syncWord[0]);
        m_messageDataBlock->get(syncWord[1]);
        m_messageDataBlock->get(syncWord[2]);
        m_messageDataBlock->get(syncWord[3]);

        if ((syncWord[0] == 0xebU) && (syncWord[1] == 0x90U) && (syncWord[2] == 0xebU) && (syncWord[3] == 0x90U))
        {
            uint16_t length = 0U;
            m_messageDataBlock->get(length);

            if (m_bNet == true)
            {
                length = ntohs(length);
            }

            length += 4U; //sync head;
            assert(length == m_messageDataBlock->getLength());
            //message id
            m_messageDataBlock->get(m_messageId);

            if (m_bNet == true)
            {
                m_messageId = ntohs(m_messageId);
            }

            //node id
            m_messageDataBlock->get(m_nodeId);

            if (m_bNet == true)
            {
                m_nodeId = ntohs(m_nodeId);
            }

            m_valid = true;
        }
        else
        {
            m_valid = false;
        }
    }
    else
    {
        m_valid = false;
    }
}

uint16_t CMessage::getNodeId(void) const
{
    return (m_nodeId);
}

void CMessage::setNodeId(const uint16_t nodeId)
{
    m_nodeId = nodeId;
    uint16_t tempNodeId = m_nodeId;

    if (m_bNet)
    {
        tempNodeId = htons(m_nodeId);
    }

    m_messageDataBlock->setBuf(8, tempNodeId);
}

const int8_t* CMessage::getBuffer(void)
{
    uint16_t length = getLength();
    length -= 4U; //sync head

    if (m_bNet == true)
    {
        length = htons(length);
    }

    m_messageDataBlock->setBuf(4, length);
    return (m_messageDataBlock->getBuffer());
}

uint16_t CMessage::getLength(void) const
{
    return (m_messageDataBlock->getLength());
}

void CMessage::reset(void)
{
    uint16_t length = getLength();
    length -= 4U; //sync head

    if (m_bNet == true)
    {
        length = htons(length);
    }
    m_messageDataBlock->setPosition((sizeof(uint16_t) * 3) + 4);
}

bool CMessage::isValid(void) const
{
    return (m_valid);
}

bool CMessage::isNet(void) const
{
    return (m_bNet);
}
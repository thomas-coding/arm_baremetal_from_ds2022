// ------------------------------------------------------------
// Message processor implementation
//
// Copyright (c) 2011 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------

/*
 * Messages transmitted have a 4 byte header;
 * <0x7E><adr><Len.1><Len.2>
 *   where adr is the destination address for the message.
 *   This allows the message to be routed to a processing
 *   module which understands the content of the message.
 *         0x00 | 0x80 = system message
 *         0x01 | 0x80 = variable message
 *         0x02 | 0x80 = printf message
 *         0x03 | 0x80 = event message
 *   where Len.1 = length[0:6]  | 0x80
 *         Len.2 = length[7:14] | 0x80
 * followed by the binary data which is control escaped stuffed.
 * The length is the byte count of the stuffed data block which 
 * follows the header. The true length of the data within the 
 * data block must be reconstructed by the receiver when the 
 * effects of stuffing have been undone.
 * 
 * NOTE1: The max length of the data block is 16K.
 * NOTE2: Forming the length bytes as above prevents either of 
 *        the length bytes requiring stuffing and keeps the
 *        header to a 4 byte packet (native word size for DCC).
 * NOTE3: Control escaped stuffing means replacing all occurrences
 *        of 0x7E with 0x7D,0x5E and all occurrences of 0x7D with
 *        0x7D,0x5D.
 *        So within the data block,
 *          <0x7E> -> <0x7D><0x5E>
 *          <0x7D> -> <0x7D><0x5D>
 *        This prevents the 0x7E value from appearing within the
 *        data block and so can be used to flag the start of a
 *        new message.
 * NOTE4: Padding bytes between messages are allowed and may be
 *        set to any values other than 0x7E or 0x7D.
 */

/*-----------------------------------------------------------------------*/
/*                            I N C L U D E S                            */
/*-----------------------------------------------------------------------*/

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "./event_viewer/types.h"
#include "./event_viewer/evMsg.h"

/* We use FLAG as a marker for the start of a message. */
static const uint8 FLAG = (uint8)'\x7E';
/* We use CESC to escape FLAG and other CESC bytes within the message body */
static const uint8 CESC = (uint8)'\x7D';
/* The length of the header */
static const int HDR_LEN = 4; 

/*-----------------------------------------------------------------------*/
/*                               CMsgProc                                */
/*-----------------------------------------------------------------------*/

/*
 * Creates a message processor 'object'
 *
 * param maxMsgLen the max length in bytes of a message
 *
 * return a pointer to the created EVMSG 'object'
 */
EVMSG* evMsg_create(int maxMsgLen)
{
    EVMSG *pMsg = 0;
    if (maxMsgLen > 16*512)
        return (EVMSG*)NULL;
    pMsg = malloc(sizeof(EVMSG));
    if (pMsg == (EVMSG*)NULL)
        return (EVMSG*)NULL;
    pMsg->m_pTxData = (uint8*)malloc(maxMsgLen+HDR_LEN);
    if (pMsg->m_pTxData == (uint8*)NULL)
    {
        free(pMsg);
        return (EVMSG*)NULL;
    }
    pMsg->m_MaxMsgLen = maxMsgLen;
    pMsg->m_pTx = pMsg->m_pTxData + HDR_LEN;
    pMsg->m_pTxData[0] = FLAG;
    pMsg->m_pTxData[1] = 0x80; /* Address */
    pMsg->m_pTxData[2] = 0x80; /* Length[0:6]  | 0x80 */
    pMsg->m_pTxData[3] = 0x80; /* Length[7:14] | 0x80 */
    return pMsg;
}

/*
 * Writes a single byte into the message
 *
 * param pMsg points to a message object previously created
 *             by a call to evMsg_create()
 * param b the byte to write into the message
 */
static void evMsg_writeBYTE(EVMSG* pMsg, uint8 b)
{
    /* If space available ... */
    if ((pMsg->m_pTx - pMsg->m_pTxData) < pMsg->m_MaxMsgLen)
    {
        /* Check for stuffing being required. We need to stuff
         * FLAG and CESC bytes, others are left alone. */
        if (b == FLAG)
        {
            *pMsg->m_pTx++ = CESC;
            *pMsg->m_pTx++ = FLAG ^ 0x20;
        }
        else if (b == CESC)
        {
            *pMsg->m_pTx++ = CESC;
            *pMsg->m_pTx++ = CESC ^ 0x20;
        }
        else
        {
            *pMsg->m_pTx++ = b;
        }
    }
}

/*
 * Writes a block of data into the message
 *
 * param pMsg points to a message object previously created
 *             by a call to evMsg_create()
 * param byteCount the number of bytes to write
 * param pData points to the data block to write
 *
 * Note: The caller must check there is enough space to write
 *       the block (by calling evMsg_canWrite()) prior to 
 *       calling this function.
 */
void evMsg_writeBlock(EVMSG* pMsg, int byteCount, const uint8* pData)
{
    while (byteCount--)
    {
        evMsg_writeBYTE(pMsg, *pData++);        
    }
}

/*!
 * Writes a block of 32 bit word data into the message
 *
 * param pMsg points to a message object previously created
 *             by a call to evMsg_create()
 * param wCount the number of 32 bit words to write
 * param pData points to the data block to write
 *
 * Note: The caller must check there is enough space to write
 *       the block (by calling evMsg_canWrite()) prior to 
 *       calling this function.
 * Note: the data is written lsb first (little endian)
 */
void evMsg_writeWordBlock(EVMSG* pMsg, int wCount, const uint32* pData)
{
    while (wCount--)
    {
        uint32 data = *pData++;
        evMsg_writeBYTE(pMsg, data & 0xFF);        
        evMsg_writeBYTE(pMsg, (data >> 8) & 0xFF);        
        evMsg_writeBYTE(pMsg, (data >> 16) & 0xFF);        
        evMsg_writeBYTE(pMsg, (data >> 24) & 0xFF);        
    }
}

/*
 * Resets the message ready to write another
 *
 * param pMsg points to a message object previously created
 *             by a call to evMsg_create()
 * param msgType is used to route the message to a processing
 *                module which will understand the content 
 */
void evMsg_reset(EVMSG* pMsg, uint8 msgType)
{
    pMsg->m_pTxData[1] = msgType | 0x80;
    pMsg->m_pTx = pMsg->m_pTxData + HDR_LEN;
}

/*
 * Returns the number of bytes placed into the message since
 * it was created or since the last call to evMsg_reset()
 *
 * param pMsg points to a message object previously created
 *       by a call to evMsg_create()
 *
 * Note: the value returned may not match the number of bytes you
 *       have written due to the stuffing process
 */
int evMsg_getWriteCount(EVMSG* pMsg)
{
    return (pMsg->m_pTx - (pMsg->m_pTxData + HDR_LEN));
}

/*
 * Indicates if it is safe to write a number of bytes into the message.
 *
 * param pMsg points to a message object previously created
 *             by a call to evMsg_create()
 * param byteCount the number of bytes you would like to write
 *        into the message
 *
 * return 0 if not enough space available, non-zero if ok.
 */
int evMsg_canWrite(EVMSG* pMsg, int byteCount)
{
    int spaceAvailable = pMsg->m_MaxMsgLen - evMsg_getWriteCount(pMsg);
    return (byteCount <= (spaceAvailable/2));
}

/*
 * Prepares a message for raw transmission and returns a pointer to the raw message and a byte count.
 *
 * param pMsg points to a message object previously created
 *             by a call to evMsg_create()
 * param pMsgLen assigned the number of raw bytes to send
 *
 * return a pointer to the raw bytes
 */
uint8* evMsg_getRawMsg(EVMSG* pMsg, int* pMsgLen)
{
    int len = evMsg_getWriteCount(pMsg);
    uint8 lenB1 = (uint8)(len | 0x80);
    uint8 lenB2 = (uint8)((len >> 7) | 0x80);
    pMsg->m_pTxData[2] = lenB1;
    pMsg->m_pTxData[3] = lenB2;
    *pMsgLen = len + HDR_LEN;
    return pMsg->m_pTxData;
}


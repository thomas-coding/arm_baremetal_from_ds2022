// ------------------------------------------------------------
// Code to output strings to the Event Viewer
//
// Copyright (c) 2011-2019 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------

/*-----------------------------------------------------------------------*/
/*                            I N C L U D E S                            */
/*-----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <arm_compat.h> // provides (AC5) compiler intrinsics such as __enable_irq()
#include "./event_viewer/evMsg.h"
#if defined(EV_ITM)
#  include "./event_viewer/evITM.h"
#endif
extern unsigned long ev_getTimeStamp(void);

/*-----------------------------------------------------------------------*/
/*                        M o d u l e  L o c a l s                       */
/*-----------------------------------------------------------------------*/

/*
 * The record header structure used to hold a variable length text record
 */
typedef __unaligned struct __attribute__((packed))
{
    unsigned long  timestamp;   /* Local absolute timestamp */
    unsigned short textLen;     /* The length of text which follows */
} EV_TEXT_RECORD;

/*
 * Points to the buffer we use to store text blocks in readiness for
 * transmission. This memory area contains:
 *   *{EV_TEXT_RECORD+TEXT}
 * The model used is that the text store is a fixed size allocated
 * memory area. An alternative implementation could use a ring buffer
 * of record which point to dynamically allocated string buffers.
 * However, I did not do this on purpose due to its potential
 * impact on overall program performance (heap performance).
 */
static char* pTextStore = 0;
/* The size of the allocated storage area */
static const int TEXT_STORE_SIZE = 512;
/* Holds the amount of free store available within pTextStore */
static volatile int freeTextStore = 0; 
/* The pointer into pTextStore where we next read from */
static volatile char* pReadPtr = 0;
/* The pointer into pTextStore where we next write to */
static volatile char* pWritePtr = 0;
/*
 * Temp buffer to hold text being constructed.
 *
 * Note: Making this a static makes the evprintf() functions etc non-reentrant
 */
static char textBuffer[256];

/*
 * Used to ensure exclusive access to the data store and its associated maintenance variables
 *
 * return !=0 if we got access and must call unlockDataStoreAccess()
 *        to free up access, ==0 if we got access and do not need
 *        to call unlockDataStoreAccess()
 */
static int lockDataStoreAccess(void)
{
    /* This simple implementation just makes sure that interrupts
     * are disabled - if they are not already disabled */
    return !__disable_irq();
}

/*
 * Frees up program access to the data store and
 * its associated maintenance variables. Following this call
 * anyone is allowed to access the data store.
 */
static void unlockDataStoreAccess(void)
{
    __enable_irq();
}

/*
 * Writes a header record and its associated text block in to the text store.
 *
 * param pHdr points to the header to write
 * param pTextBuffer points to the text to write
 * param len the length of text in pTextBuffer
 *
 * Note:  Any trailing '\0' in pTextBuffer is not written to the text store
 *
 */
static void writeTextStore(EV_TEXT_RECORD* pHdr, char* pTextBuffer, int len)
{
    /* Get sole access to the data store and associated variables */
    int mustUnlock = lockDataStoreAccess();
    char* pSrc = (char*)pHdr;
    char* pDst = (char*)pWritePtr;
    /* Write the header */
    size_t blockLen = sizeof(EV_TEXT_RECORD);
    while (blockLen--)
    {
        *pDst++ = *pSrc++;
        if (pDst >= pTextStore+TEXT_STORE_SIZE)
            pDst = pTextStore;
    }
    /* Write the text buffer */
    pSrc = pTextBuffer;
    blockLen = (size_t)len;
    while (blockLen--)
    {
        *pDst++ = *pSrc++;
        if (pDst >= pTextStore+TEXT_STORE_SIZE)
            pDst = pTextStore;
    }
    freeTextStore -= (int)(sizeof(EV_TEXT_RECORD)+len);
    pWritePtr = pDst;
    /* Release access to the data store and associated variables */
    if (mustUnlock)
        unlockDataStoreAccess();
}

/*
 * Writes a text buffer in to the text store
 *
 * param pTextBuffer points to the text to write
 * param len the length of the text
 *
 * returns len on success or 0 if no space to write
 */
static int storeTextBuffer(char* pTextBuffer, int len)
{
    int rSize = 0;
    int totalSize = sizeof(EV_TEXT_RECORD) + len;
    if (totalSize <= freeTextStore)
    {
        EV_TEXT_RECORD hdr;
        hdr.timestamp = ev_getTimeStamp();
        hdr.textLen = len;
        writeTextStore(&hdr, pTextBuffer, len);
        rSize = len;
    }
    return rSize;
}

/*
 * Returns a number of bytes from the text store but does not advance the text store read pointer.
 *
 * param pBuffer where we write the data to
 * param len the length in bytes to read
 *
 * returns the text store read pointer value if a proper
 * read had taken place i.e. what pReadPtr value
 * would be if a read had taken place
 */
static char* peekTextStore(char* pBuffer, int len)
{
    char* pSrc = (char*)pReadPtr;
    char* pDst = pBuffer;
    while (len--)
    {
        *pDst++ = *pSrc++;
        if (pSrc >= pTextStore+TEXT_STORE_SIZE)
            pSrc = pTextStore;
    }
    return pSrc;
}

/*
 * Reads a number of bytes out of the text store
 *
 * param pBuffer where we write the data to
 * param len the length in bytes to read
 */
static void readTextStore(char* pBuffer, int len)
{
    /* Get sole access to the data store and associated variables */
    int mustUnlock = lockDataStoreAccess();
    pReadPtr = peekTextStore(pBuffer, len);
    freeTextStore += len;
    /* Release access to the data store and associated variables */
    if (mustUnlock)
        unlockDataStoreAccess();
}

/*
 * Moves the text store read pointer forwards/backwards
 *
 * param len the 'distance' in bytes to move, +ve to move forwards,
 *       -ve to move backwards
 */ 
static void offsetTextStoreReadPtr(int len)
{
    /* Get sole access to the data store and associated variables */
    int mustUnlock = lockDataStoreAccess();
    /* Moving forwards increases the amount of free storage,
     * moving backwards decreases it */
    freeTextStore += len;
    if (len > 0)
    {
        /* Forwards */
        while (len--)
        {
            if (++pReadPtr >= pTextStore+TEXT_STORE_SIZE)
                pReadPtr = pTextStore;
        }
    }
    else if (len < 0)
    {
        /* Backwards */
        while (len++)
        {
            if (pReadPtr == pTextStore)
                pReadPtr = pTextStore+(TEXT_STORE_SIZE-1);
            else
                --pReadPtr;
        }
    }
    /* Release access to the data store and associated variables */
    if (mustUnlock)
        unlockDataStoreAccess();
}
            
/*-----------------------------------------------------------------------*/
/*                        M o d u l e  P u b l i c s                     */
/*-----------------------------------------------------------------------*/

/*
 * Initialises the evprintf module
 */
void evprintf_init(void)
{
    pTextStore = (char*)malloc(TEXT_STORE_SIZE);
    pReadPtr = pTextStore;
    pWritePtr = pTextStore;
    if (pTextStore != (char*)NULL)
    {
        freeTextStore = TEXT_STORE_SIZE;
    }
    else
    {
        freeTextStore = 0;
    }
}

/*
 * Finalises the evprintf module
 */
void evprintf_final(void)
{
    freeTextStore = 0;
    if (pTextStore != (char*)NULL)
    {
        free(pTextStore);
        pTextStore = (char*)NULL;
    }
    pReadPtr = (char*)NULL;
    pWritePtr = (char*)NULL;
}

/*
 * A printf implementation which prints to the EventViewer
 *
 * param format String format parameter, same as your system's printf
 *
 * returns negative on printf error, 0 if no space to send to
 *          the EventViewer, positive is the number of chars output
 *
 * Note: The generated string must not be longer than the size of
 *       textBuffer. If it is, we will probably crash
 */
int  evprintf(const char * format, ... )
{
    int formedLen = 0;
    /* Do the usual var args stuff - assumption is that textBuffer 
     * is large enough! */
    va_list args;
    va_start(args, format);
    formedLen = vsprintf(textBuffer, format, args);
    va_end(args);
    if (formedLen > 0)
    {
        formedLen = storeTextBuffer(textBuffer, formedLen);
    }
    return formedLen;
}

/*
 * If we have any data to send we will write it into the message object.
 *
 * Note: We will only write one or more _complete_
 *       EV_TEXT_RECORD+TEXT entries into the message object.
 */
void evprintf_msgGenerator(EVMSG* pMsg)
{
    EV_TEXT_RECORD hdr;
    int msgFull = 0;
    evMsg_reset(pMsg, evMsg_PRINTF);
    /* While our text store is not empty ... */
    while (!msgFull && (freeTextStore < TEXT_STORE_SIZE))
    {
        /* Look at (but don't read) the header record to see how long the text block is */
        peekTextStore((char*)&hdr, (int)sizeof(EV_TEXT_RECORD));
        /* If the message processor has space ... */
        if (evMsg_canWrite(pMsg, (int)sizeof(EV_TEXT_RECORD)+hdr.textLen))
        {
            /* Write the header record */
            evMsg_writeBlock(pMsg, (int)sizeof(EV_TEXT_RECORD), (const uint8*)&hdr);
            /* Skip past the header record in our text store */
            offsetTextStoreReadPtr((int)sizeof(EV_TEXT_RECORD));
            /* Read out the text part ... */
            readTextStore(textBuffer, hdr.textLen);
            /*  ... and write into the message */
            evMsg_writeBlock(pMsg, hdr.textLen, (const uint8*)textBuffer);
        }
        else
        {
            msgFull = 1;
        }
    }
}

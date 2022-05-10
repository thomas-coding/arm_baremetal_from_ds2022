// ------------------------------------------------------------
// Code to output the messages packets over ITM
//
// Copyright (c) 2011 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------

/*-----------------------------------------------------------------------*/
/*                            I N C L U D E S                            */
/*-----------------------------------------------------------------------*/
#include "./event_viewer/evITM.h"
#include "./event_viewer/evMsg.h"
#include "./event_viewer/cortexM4_itm.h"

/* ITM Lock Status/Access Flags */
#define ITM_LS_LOCK_EXISTS       0x01
#define ITM_LS_LOCKED            0x02
#define ITM_LS_BYTE_ACCESS       0x04
#define ITM_LA_UNLOCK            0xC5ACCE55

/* ITM Trace Control Flags */
#define ITM_DWTENA_MASK          0x08
#define ITM_SYNCENA_MASK         0x04
#define ITM_TSENA_MASK           0x02
#define ITM_ITMENA_MASK          0x01

#define ITM_IS_LOCKED (ITM_LOCK_STATUS & (ITM_LS_LOCK_EXISTS | ITM_LS_LOCKED)) == (ITM_LS_LOCK_EXISTS | ITM_LS_LOCKED)

/*-----------------------------------------------------------------------*/
/*                        M o d u l e  L o c a l s                       */
/*-----------------------------------------------------------------------*/

/* Indicates how many bytes are ready to be transmitted over ITM */
static volatile int ITMByteCount = 0;
/* Points to the next byte to send over ITM */
static volatile uint8 *pITMBuf = (volatile uint8 *) 0;
/* Used to build up messages which are sent over ITM */
static EVMSG* pMsg = (EVMSG*) 0;
/* Points to the start of a 0 terminated array of
 * message generator functions */
static EV_MSGGEN* msgGeneratorFns;
/* Current index into msgGeneratorFns */
static int nextGenerator = 0;
/* A flag to record if the ITM unlocking failed. -1 for untried, 1 for failed, 0 for success */
//static int failedToUnlockITM = -1;
/* The ITM port we use for the encoded events ITM channel */
static unsigned int evITMPort = 1;

/*
 * Attempts to unlock the ITM so that we can write to the ITM configuration registers
 * return 1 if the ITM was unlocked, 0 if it was not
 */
static int ITMUnlock(void) {
	/* Check the Lock Status and unlock it if need be */
	if (ITM_IS_LOCKED) {
		ITM_LOCK_ACCESS = ITM_LA_UNLOCK;
		if (ITM_IS_LOCKED) {
			//failedToUnlockITM = 1;
			return 0;
		}
	}
	//failedToUnlockITM = 0;
	return 1;
}

/*
 * Initialises the ITM
 *
 * param traceControl the value to send to the ITM_TRACE_CONTROL register
 * param portsInUseByBit a bitmask of the ITM ports to enable
 *
 * return 1 if the ITM was initialised, 0 otherwise
 */
static int ITMInitialise(unsigned int traceControl,
		unsigned int portsInUseByBit) {
	/* Unlock */
	int itmUnlocked = ITMUnlock();

	if (itmUnlocked == 1) {
		/* Stop trace (so that starting trace will generate a sync packet) */
		ITM_TRACE_CONTROL = 0;
		/* Set the trace control register */
		ITM_TRACE_CONTROL = traceControl;
		/* Only enable the ports if the ITMENA has been set */
		if ((traceControl & ITM_ITMENA_MASK) > 0) {
			/* Enable trace channels */
			ITM_TRACE_ENABLE = portsInUseByBit;
		}
	}
	return itmUnlocked;
}

/*
 * Attempts to write a word to the given port
 *
 * param port The stimulus port to write to (0:31)
 * param value The value to write to the stimulus port
 *
 * returns 1 if the value was written, 0 if not
 */
static int ITMWriteToPort(unsigned port, unsigned value) {
	int tryCount;
	/* Wait for ITM Tx register to be writable */
	for (tryCount = 0; tryCount < 10000; ++tryCount) {
		if (ITM_STIMULUS_PORTS[port] == 1) {
			ITM_STIMULUS_PORTS[port] = value;
			return 1;
		}
	}
	return 0;
}

/*
 * Used to write data from pITMBuf into the ITM transmit register
 * returns non-zero if data was output to the ITM transmit register
 *
 * Note:  Updates pITMBuf, ITMByteCount
 */
static int evITMWrite(void) {
	unsigned int value = 0;
	int didWrite = 0;
	unsigned valueSize = 0;

	/* Anything to do? */
	if (ITMByteCount) {
		/* Prepare the value to transmit */
		for (valueSize = 0; valueSize < 4 && valueSize < ITMByteCount;
				valueSize++)
			value |= pITMBuf[valueSize] << (8 * valueSize);

		if (ITMWriteToPort(evITMPort, value)) {
			/* Update the pointers/counters */
			ITMByteCount -= valueSize;
			pITMBuf += valueSize;
			didWrite = 1;
		}
	}
	return didWrite;
}

/*-----------------------------------------------------------------------*/
/*                        M o d u l e  P u b l i c s                     */
/*-----------------------------------------------------------------------*/

/*
 * Initialises the ITM transport for Event Viewer messages
 *
 * param msgGenerators an array of functions which we should call
 *       to pick up messages to be transmitted
 * param evITMPortChannel the ITM channel we should use to send
 *       the encoded event data stream
 * param ITMPortMask the bit set of ITM channels we want enabled.
 *       This should include the bit for the evITMPortChannel as
 *       well as any others required by calls to evITM_writeStringToPort()
 */
void evITM_init(EV_MSGGEN msgGenerators[], unsigned int evITMPortChannel,
		unsigned int ITMPortMask) {
	unsigned int ITMCtrl = 0;
	evITMPort = evITMPortChannel;
	/* Create our message 'object' */
	pMsg = evMsg_create(256);
	/* No ITM data to start off with */
	ITMByteCount = 0;
	pITMBuf = 0;
	/* Store a reference to the message generator function array */
	msgGeneratorFns = msgGenerators;
	nextGenerator = 0;
	/* Setup the ITM */
	ITMCtrl = (ITM_ATB_ID & 0x7F) << 16 | ITM_SYNCENA_MASK /*| ITM_TSENA_MASK */
			| ITM_ITMENA_MASK;
	ITMInitialise(ITMCtrl, ITMPortMask);
}

/*
 * Used to keep the ITM transmit system alive.
 * This should be called 'often' (say every 10ms) to make
 * sure that ITM transmit data is sent.
 */
void evITM_pump(void) {
	int write = 0;

	/* Have we been initialised? */
	if ((pMsg != (EVMSG*) 0) && (msgGeneratorFns != (EV_MSGGEN*) 0)) {
		/* Do we have ITM data to transmit? */
		if (ITMByteCount && pITMBuf) {
			/* Just restart the ITM traffic */
			write = EV_ITM_WORDS_TO_WRITE;
		} else {
			/* Call the message generator functions until one of them
			 * gives us a message or we have been round all of them */
			int firstGeneratorFn = nextGenerator;
			evMsg_reset(pMsg, 0);
			do {
				if (msgGeneratorFns[nextGenerator] == (EV_MSGGEN) 0) {
					/* Wrap to start of array */
					nextGenerator = 0;
				} else {
					/* Try to collect a message from the generator fn */
					msgGeneratorFns[nextGenerator++](pMsg);
				}
			} while ((nextGenerator != firstGeneratorFn)
					&& (evMsg_getWriteCount(pMsg) == 0));
			/* Did we get a message? */
			if (evMsg_getWriteCount(pMsg) != 0) {
				/* Get a pointer to the raw message byte stream and its length */
				pITMBuf = evMsg_getRawMsg(pMsg, (int*) &ITMByteCount);
			}
			/* Do we have ITM data to transmit? */
			if (ITMByteCount && pITMBuf) {
				/* Restart the ITM traffic */
				write = EV_ITM_WORDS_TO_WRITE;
			}
		}
		// Write a few words
		if (write) {
			for (; write > 0; write--)
				if (evITMWrite() == 0)
					break;
		}
	}
}

/*
 * Writes a string to the given port
 *
 * param port The stimulus port to write to [0..]
 * param string The ASCII string to write
 *
 */
void evITM_writeStringToPort(unsigned port, const char *string) {
	unsigned buffer = 0;
	unsigned subChar = 0;
	while (*string != '\0') {
		buffer = 0;
		for (subChar = 0; subChar < 4 && string[subChar] != '\0'; ++subChar)
			buffer |= (((unsigned int) string[subChar]) << (subChar * 8));
		ITMWriteToPort(port, buffer);
		string += subChar;
	}
	/* Write a null if our transfer terminated at the end of a word */
	if (subChar == 4)
		ITMWriteToPort(port, '\0');
}


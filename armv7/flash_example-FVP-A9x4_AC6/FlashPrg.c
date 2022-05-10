/* Copyright (c) 2003-2013 Arm Limited (or its affiliates). All rights reserved. */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */

/***********************************************************************/
/*                                                                     */
/*  FlashPrg.c:  Flash Programming Functions adapted                   */
/*               for FVP VE Cortex-A9x4                                */
/*                                                                     */
/***********************************************************************/

#include "FlashOS.h"        // FlashOS Structures

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

// Memory Map
#define FLASH_BASE      0x80000000
#define FLASH_SIZE      0x00080000      // Size = 512kB
#define SECTOR_SIZE     0x00001000		// 4K sector size

/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */
int Init (unsigned long adr, unsigned long clk, unsigned long fnc) {
	return 0;
}

/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */
int UnInit (unsigned long fnc) {
	return 0;
}

/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */
int EraseChip (void) {
	/* Fill entire flash device with 0xFF */
	u32* pFlash = (u32*)FLASH_BASE;
	unsigned int len = FLASH_SIZE/4;
	while (len-- != 0)
		*pFlash++ = 0xFFFFFFFF;
	return 0;
}

/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */
int EraseSector (unsigned long adr) {
	/* Fill entire flash sector with 0xFF */
	/* Align adr to sector start */
	u32* pFlash = (u32*)(adr & ~(SECTOR_SIZE-1));
	/* Fill the sector a 32 bit word at a time */
	unsigned int len = SECTOR_SIZE/4;
	while (len-- != 0)
		*pFlash++ = 0xFFFFFFFF;
	return 0;
}

/*  
 *  Blank Check Checks if Memory is Blank
 *    Parameter:      adr:  Block Start Address
 *                    sz:   Block Size (in bytes)
 *                    pat:  Block Pattern
 *    Return Value:   0 - OK,  1 - Failed
 */
int BlankCheck (unsigned long adr, unsigned long sz, unsigned char pat) {
	u8* pFlash = (u8*)(adr);
	while (sz != 0) {
		if (*pFlash++ != pat)
			return 1;
		sz -= 1;
	}
	return 0;
}

/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */
int ProgramPage (unsigned long adr, unsigned long sz, unsigned char *buf) {
	/* Transfer bytes until adr aligned on 32 word boundary */
	while ((adr & 3) && (sz > 0)) {
		*(u8*)adr = *buf++;
		--sz;
		++adr;
	}
	/* Transfer whole words */
	while (sz >= 4) {
		u32 w = buf[3];
		w = (w << 8) | buf[2];
		w = (w << 8) | buf[1];
		w = (w << 8) | buf[0];
		*(u32*)adr = w;
		adr += 4;
		buf += 4;
		sz -= 4;
	}
	/* Transfer any remaining bytes */
	while (sz > 0) {
		*(u8*)adr = *buf++;
		++adr;
		--sz;
	}
	return 0;
}

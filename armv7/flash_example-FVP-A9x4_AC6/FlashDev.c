/* Copyright (c) 2003-2010 Arm Limited (or its affiliates). All rights reserved. */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */

/***********************************************************************/
/*                                                                     */
/*  FlashDev.c:  Device Description for Flash                          */
/*                                                                     */
/***********************************************************************/

#include "FlashOS.h"        // FlashOS Structures


struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "FVP VE Cortex-A9x4 Flash", // Device Name
   ONCHIP,                     // Device Type
   0x80000000,                 // Device Start Address
   0x00080000,                 // Device Size in Bytes
   4096,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   2000,                       // Program Page Timeout 2 Sec
   500,                        // Erase Sector Timeout 500 mSec

// Specify Size and Address of Sectors
   0x1000, 0x000000,           // Sector Size 4kB
   SECTOR_END
};

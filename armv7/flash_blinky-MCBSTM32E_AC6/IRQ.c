/*----------------------------------------------------------------------------
 * Name:    IRQ.c
 * Purpose: IRQ Handler
 * Note(s):
 *----------------------------------------------------------------------------
 * Copyright (c) 2011 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 *----------------------------------------------------------------------------*/

#include "stm32f10x.h"
#include "LED.h"
#include "ADC.h"

uint8_t  clock_1s;                           /* Flag activated each second    */


/*----------------------------------------------------------------------------
  Systick Interrupt Handler
  SysTick interrupt happens every 10 ms
 *----------------------------------------------------------------------------*/
void SysTick_Handler (void) {
  static unsigned long ticks = 0;
  static unsigned long timetick;
  static unsigned int  leds = 0x01;

  if (ticks++ >= 99) {                       /* Set Clock1s to 1 every second */
    ticks    = 0;
    clock_1s = 1;
  }

  /* Blink the LEDs depending on ADC_ConvertedValue                           */
  if (timetick++ >= (AD_last >> 8)) {
    timetick   = 0;
    leds     <<= 1;
    if (leds > (1 << LED_NUM)) leds = 0x01;
    LED_Out (leds);
  }

  ADC_StartCnv();
}

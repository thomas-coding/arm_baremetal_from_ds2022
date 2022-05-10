/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher
 * Note(s): possible defines:
 *            __USE_LCD   - enable Output on LCD
 *----------------------------------------------------------------------------
 * Copyright (c) 2008-2021 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "stm32f10x.h"                  /* STM32F10x.h definitions            */
#include "GLCD.h"
#include "Serial.h"
#include "LED.h"
#include "ADC.h"

#define __FI        1                   /* Font index 16x24                   */

char text[10];

/* Import external variables from IRQ.c file                                  */
extern uint8_t  clock_1s;

/* variable demonstrating trace  capabilities */
uint16_t AD_DbgVal;


/*----------------------------------------------------------------------------
  Function that initializes User Buttons
 *----------------------------------------------------------------------------*/
void BUT_Init(void) {
  RCC->APB2ENR |= (1UL << 8);           /* Enable GPIOG clock                 */

  GPIOG->CRH   &= ~0x0000000F;          /* Configure the GPIO for Button      */
  GPIOG->CRH   |=  0x00000004;
}

/*----------------------------------------------------------------------------
  Function that initializes Joystick
 *----------------------------------------------------------------------------*/
void JOY_Init(void) {
  RCC->APB2ENR |= (1UL << 8);           /* Enable GPIOG clock                 */
  RCC->APB2ENR |= (1UL << 5);           /* Enable GPIOD clock                 */

  GPIOG->CRL   &= ~0xF0000000;          /* Configure the GPIO for Joystick    */
  GPIOG->CRL   |=  0x40000000;
  GPIOG->CRH   &= ~0xFFF00000;
  GPIOG->CRH   |=  0x44400000;

  GPIOD->CRL   &= ~0x0000F000;          /* Configure the GPIO for Joystick    */
  GPIOD->CRL   |=  0x00004000;
}

/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  uint32_t ad_avg = 0;
  uint16_t ad_val = 0, ad_val_ = 0xFFFF;
   int32_t joy    = 0, joy_    = -1;
   int32_t but    = 0, but_    = -1;

  LED_Init();                                /* LED Initialization            */
  SER_Init();                                /* UART Initialization           */
  ADC_Init();                                /* ADC Initialization            */
  BUT_Init();                                /* User Button Initialization    */
  JOY_Init();                                /* Joystick Initialization       */

#ifdef __USE_LCD
  GLCD_Init();                               /* GLCD Initialization           */

  GLCD_Clear(White);                         /* Clear graphical LCD display   */
  GLCD_SetBackColor(Blue);
  GLCD_SetTextColor(White);
  GLCD_DisplayString(0, 0, __FI, "   MCBSTM32E Demo   ");
  GLCD_DisplayString(1, 0, __FI, "     Blinky AC6     ");
  GLCD_DisplayString(2, 0, __FI, "                    ");
  GLCD_SetBackColor(White);
  GLCD_SetTextColor(Blue);
  GLCD_DisplayString(4, 0, __FI, "AD value:            ");
  GLCD_DisplayString(6, 0, __FI, "Buttons :            ");
  GLCD_DisplayString(8, 0, __FI, "Joystick:            ");
#endif

  SysTick_Config(SystemCoreClock / 100);     /* Generate interrupt each 10 ms */

  while (1) {                                /* Loop forever                  */
    /* Collect all input signals                                              */

    /* AD converter input                                                     */
    if (AD_done) {                           /* If conversion has finished    */
      AD_done = 0;

      ad_avg += AD_last << 8;                /* Add AD value to averaging     */
      ad_avg ++;
      if ((ad_avg & 0xFF) == 0x10) {         /* average over 16 values        */
        ad_val = (ad_avg >> 8) >> 4;         /* average divided by 16         */
        ad_avg = 0;
      }
    }

    /* Joystick input                                                         */
    joy = 0;
    if (GPIOG->IDR & (1 << 14)) joy |= (1 << 0);  /* Joystick left            */
    if (GPIOG->IDR & (1 << 13)) joy |= (1 << 1);  /* Joystick right           */
    if (GPIOG->IDR & (1 << 15)) joy |= (1 << 2);  /* Joystick up              */
    if (GPIOD->IDR & (1 <<  3)) joy |= (1 << 3);  /* Joystick down            */
    if (GPIOG->IDR & (1 <<  7)) joy |= (1 << 4);  /* Joystick select          */
    joy ^= 0x1F;

    /* Button inputs                                                          */
    but = 0;
    if (GPIOG->IDR & (1 <<  8)) but |= (1 << 0);  /* Button User (User)       */
    but ^= 0x01;

    /* Show all signals                                                       */
    if (ad_val ^ ad_val_) {                  /* AD value changed              */
      ad_val_ = ad_val;

      AD_DbgVal = ad_val;

      sprintf(text, "0x%04X", ad_val);       /* format text for print out     */
#ifdef __USE_LCD
      GLCD_SetTextColor(Red);
      GLCD_DisplayString(4,  9, __FI,  (unsigned char *)text);
      GLCD_Bargraph (144, 5*24, 176, 20, (ad_val>>2));
#endif
    }

    if (but ^ but_) {                        /* Show buttons status           */
#ifdef __USE_LCD
      if (but & (1 << 0)) {
        GLCD_SetTextColor(Red);
        GLCD_DisplayString(6,  9, __FI, "User");
      } else {
        GLCD_SetTextColor(LightGrey);
        GLCD_DisplayString(6,  9, __FI, "User");
      }
#endif

      but_ = but;
    }

    if (joy ^ joy_) {                         /* Show joystick status          */
#ifdef __USE_LCD
      if (joy & (1 << 0)) {                   /* check  "Left" Key             */
        GLCD_SetTextColor(Red);
        GLCD_DisplayChar(8,  9, __FI, 0x80+ 9);
      } else {
        GLCD_SetTextColor(LightGrey);
        GLCD_DisplayChar(8,  9, __FI, 0x80+ 8);
      }
      if (joy & (1 << 1)) {                   /* check "Right" Key             */
        GLCD_SetTextColor(Red);
        GLCD_DisplayChar(8, 11, __FI, 0x80+11);
      } else {
        GLCD_SetTextColor(LightGrey);
        GLCD_DisplayChar(8, 11, __FI, 0x80+10);
      }
      if (joy & (1 << 2)) {                  /* check "Up" Key                 */
        GLCD_SetTextColor(Red);
        GLCD_DisplayChar(7, 10, __FI, 0x80+ 5);
      } else {
        GLCD_SetTextColor(LightGrey);
        GLCD_DisplayChar(7, 10, __FI, 0x80+ 4);
      }
      if (joy & (1 << 3)) {                  /* check "Down" Key               */
        GLCD_SetTextColor(Red);
        GLCD_DisplayChar(9, 10, __FI, 0x80+ 7);
      } else {
        GLCD_SetTextColor(LightGrey);
        GLCD_DisplayChar(9, 10, __FI, 0x80+ 6);
      }
      if (joy & (1 << 4)) {                  /* check "Sel" Key                */
        GLCD_SetTextColor(Red);
        GLCD_DisplayChar(8, 10, __FI, 0x80+ 1);
      } else {
        GLCD_SetTextColor(LightGrey);
        GLCD_DisplayChar(8, 10, __FI, 0x80+ 0);
      }
#endif

      joy_ = joy;
    }

    /* Print message with AD value every second                               */
    if (clock_1s) {
      clock_1s = 0;

      printf("AD value: %s\r\n", text);
    }
  }
}

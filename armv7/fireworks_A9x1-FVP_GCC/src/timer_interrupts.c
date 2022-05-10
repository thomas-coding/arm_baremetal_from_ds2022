/* Bare-metal fireworks example for Cortex-A9 VE FVP model */

/* Timer and interrupts */

/* Copyright (c) 2011-2016 Arm Limited (or its affiliates). All rights reserved. */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */

// LED Base address
#define LED_BASE (volatile unsigned int *)0x1C010008

#define PERIPHBASE      0x2C000000  // base address of private peripheral space

// Macro to construct code for GIC register accesses, from definitions below
#define GIC_REGISTER(offset) (volatile unsigned int*)(PERIPHBASE + offset)

//Interrupt controller interfaces Registers
#define GIC_Priority_Mask      GIC_REGISTER(0x0104)

//Private timers and watchdogs
#define GIC_Timer_Reload       GIC_REGISTER(0x0600)
#define GIC_Timer_Control      GIC_REGISTER(0x0608)
#define GIC_Timer_Int_Status   GIC_REGISTER(0x060c)

//Interrupt Distributor Registers
#define GIC_Control            GIC_REGISTER(0x1000)
#define GIC_SetEnable0_31      GIC_REGISTER(0x1100)
#define GIC_ClearEnable0_31    GIC_REGISTER(0x1180)
#define GIC_ClearEnable32_63   GIC_REGISTER(0x1184)
#define GIC_Priority28_31      GIC_REGISTER(0x141C)
#define GIC_ClearPending0_31   GIC_REGISTER(0x1280)
#define GIC_ClearPending32_63  GIC_REGISTER(0x1284)


void nudge_leds(void) // Move LEDs along
{
	static int state = 1;
	static int value = 1;

	if (state)
	{
		int max = (1 << 7);
		value <<= 1;
		if (value == max)
			state = 0;
	}
	else
	{
		value >>= 1;
		if (value == 1)
			state = 1;
	}

	*LED_BASE = value;	// Update LEDs hardware
}


// Timer Interrupt Handler
void C_interrupt_handler(void)
{
	// Clear interrupt
	*GIC_Timer_Int_Status = 1;

	// Perform tick actions
	nudge_leds();
}


// Initialize Timer 0 and Interrupt Controller
void init_timer(void)
{
	volatile unsigned int irq_id = 0, priority = 0, timer = 0;

	// Init Timer
	// PRESCALER = 174
	// Auto-increment = auto increment mode
	// Enable interrupt
	*GIC_Timer_Control = 0xA606;
    //Set Timer Reload
    *GIC_Timer_Reload = 0x2000;

    //Init DIC
    //Disable Secure and Non-secure interrupts
	*GIC_Control = 0x00;

	//Disable All interrupts
	*GIC_ClearEnable0_31 = 0xFFFFFFFF;
	*GIC_ClearEnable32_63 = 0xFFFFFFFF;
    //Clear All pending interrupts
	*GIC_ClearPending0_31 = 0xFFFFFFFF;
    *GIC_ClearPending32_63 = 0xFFFFFFFF;

    //Enable internal timer interrupt (ID = 29)
    *GIC_SetEnable0_31 = *GIC_SetEnable0_31 | (1 << 29);
    //High Priority interrupt
    *GIC_Priority28_31 = *GIC_Priority28_31 & ~(0xFF00);
    //Set 32 supported levels
    *GIC_Priority_Mask = (1<<3);

    //Enable Secure interrupts
    *GIC_Control = 0x01;

    //Clear Flag
    *GIC_Timer_Int_Status = 1;

     //Timer Go
    *GIC_Timer_Control = *GIC_Timer_Control | 0x1;
}

/* ------------------------------------------------------------
 evconfig.h
 Header File

 Copyright (c) 2011 Arm Limited (or its affiliates). All rights reserved.
 Use, modification and redistribution of this file is subject to your possession of a
 valid End User License Agreement for the Arm Product of which these examples are part of 
 and your compliance with all applicable terms and conditions of such licence agreement.
------------------------------------------------------------
*/

#if !defined(EVCONFIG_H)
#define EVCONFIG_H

/*
 * This block controls/defines the output of a state event which
 * tracks the state of the program. 
 */
/* Set non-zero to turn on the event generation */
#define EV_EVENT_TRACK_PROG_STATE 0
/* The source id of the program state event */
#define EV_EVENT_SRC_PROG_STATE 1
/* The INIT state value */
#define STATE_INIT 1
/* The UPDATE_DISPLAY state value */
#define STATE_UPDATE_DISPLAY 2
/* The UPDATE_LEDS state value */
#define STATE_UPDATE_LEDS 3
/* The RUNNING state value */
#define STATE_RUNNING 4
/* The WAIT state value */
#define STATE_WAIT 5
/* The KBD state value */
#define STATE_KBD 6

/*
 * This block controls/defines the output of a duration event
 * which shows entry/exit to the timer IRQ
 */
/* Set non-zero to turn on the event generation */
#define EV_EVENT_LOG_TIMER_IRQ 0
/* The source id of the timer irq duration event */
#define EV_EVENT_SRC_TIMER_IRQ 2

/*
 * This block controls/defines the output of an event
 * which logs writes to the LCD H/W
 */
/* Set non-zero to turn on the event generation */
#define EV_EVENT_LOG_LCD_ACCESS 0
/* The source id of the LCD event */
#define EV_EVENT_SRC_LCD 3
/* The event id for an LCD data write */
#define EV_EVENT_SRC_LCD_DATA 1

/*
 * This block controls/defines the output of an event
 * which logs calls to the default vector address IRQ
 */
#define EV_EVENT_LOG_DVAR_IRQ 0
/* The source id of the DVAR IRQ event */
#define EV_EVENT_SRC_DVAR_IRQ 4
/* The event id for the DVAR data */
#define EV_EVENT_SRC_DVAR_IRQ_DATA 1

/*
 * This block defines the attribute IDs. We can define up to 
 * 256 known attributes
 */
/* The attribute ID for the LCD command value */
#define EV_EVENT_ATTRIB_LCDCMD_VALUE 1
/* The attribute ID for the LCD data value */
#define EV_EVENT_ATTRIB_LCDDATA_VALUE 2
/* The attribute ID for the AD value */
#define EV_EVENT_ATTRIB_AD_VALUE 3
/* The attribute ID for the VIC0 ISR value */
#define EV_EVENT_ATTRIB_VICISR_VALUE 4
/* The attribute ID for the VIC0 INTSR value */
#define EV_EVENT_ATTRIB_VICRINTSR_VALUE 5
/* The attribute ID for the VIC0 INTER value */
#define EV_EVENT_ATTRIB_VICINTER_VALUE 6
/* The attribute ID for the N value */
#define EV_EVENT_ATTRIB_N_VALUE 7


/*
 * This block controls/defines the output of the variable values
 */
/* Set non-zero to turn on the variable value generation */
#define EV_VAR_LOG_ANALOG_VALUES 0
#define EV_VAR_ANALOG_VALUE_A 1
#define EV_VAR_ANALOG_VALUE_B 2
/*
 * This block controls the linked relation between events
 */
/* The event has no linked relationship */
#define EV_EVENT_LINK_NO 0
/* The event would be linked */
#define EV_EVENT_LINKED 1
/* The event links to another event */
#define EV_EVENT_LINKING 2

#endif

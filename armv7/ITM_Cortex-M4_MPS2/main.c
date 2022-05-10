// ------------------------------------------------------------
// ITM example for Cortex-M4 on MPS2
//
// Copyright (c) 2015-2019 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------


#include <stdio.h>
#include <time.h>

#include "./event_viewer/evITM.h"
#include "./event_viewer/evprintf.h"

#define RAW_ITM_CHANNEL 1 /* The ITM channel we use for raw text */
#define EV_ITM_CHANNEL 2  /* The ITM channel we use for encoded events */

int main(int argc, char** argv)
{
    /*
     * Configure ITM
     */
    char outputStr[64];
    /* IMPORTANT */
    /* To capture ITM data with the Debugger, select Run->Debug Configurations, then select the ITM_Cortex-M4_MPS2 debug config.
    In its Connections panel, press 'DTSL Options Edit...'.  In the 'Trace Capture' tab, select (for DSTREAM-ST) 'Streaming Trace' or (for DSTREAM) 'DSTREAM 4GB Trace Buffer'.
    In the 'ITM' tab, tick 'Enable ITM Trace'.  Click on OK to save the DTSL options.  For more information, see the readme.html */

    // Define msgGenerators. An array of functions to pick up messages to be transmitted
    EV_MSGGEN msgGens[2];

    // Event viewer Init
    evprintf_init();            // Initialises the evprintf module that outputs strings to the Event Viewer

    // Initialise array of function pointers
    msgGens[0] = evprintf_msgGenerator;    // Pointer to function that generates strings to the Event Viewer
    msgGens[1] = NULL;

    // Initialises the ITM transport for Event Viewer messages
    // Use EV_ITM_CHANNEL to send the encoded event data stream
    // Enable all ITM channels
    evITM_init(msgGens, EV_ITM_CHANNEL, 0xFFFFFFFF);


    /*
     * Use ITM
     */
    printf("Hello world from Cortex-M4\n");
    // Output a raw string to the first port. No encoding will be performed.
    sprintf(outputStr, "ITM raw text message\n");
    evITM_writeStringToPort(RAW_ITM_CHANNEL, outputStr);
    // Output a time-stamped message to the encoded port as setup in evITM_init(...).
    // This outputs to a transmit buffer before pumping it out to the ITM registers.
    sprintf(outputStr, "ITM event encoded message\n");
    evprintf(outputStr);
    evITM_pump();
    printf("Sent ITM trace events\n");

    return 0;
}

unsigned long ev_getTimeStamp(void)
{
    return clock();
}

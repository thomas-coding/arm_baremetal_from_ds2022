/* Bare-metal fireworks example */

/* Aeroplane banner (120x600) pixel array */

/* Copyright (c) 2010-2016 Arm Limited (or its affiliates). All rights reserved. */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */

#define BANNER_WIDTH 600
#define BANNER_HEIGHT 120
typedef unsigned short pixel_t;
extern const pixel_t banner_data[BANNER_WIDTH * BANNER_HEIGHT];

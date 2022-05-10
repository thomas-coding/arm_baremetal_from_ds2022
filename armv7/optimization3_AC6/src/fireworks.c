/* Copyright (c) 2006-2021 Arm Limited (or its affiliates). All rights reserved. */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */

/* Original code by Stephen Luce */


extern void start_perfmon(void); // used in step 2
extern void stop_perfmon(void);  // used in step 2

#define FRAME_BUFFER 0x80600000
#define LCD_WIDTH 800
#define LCD_HEIGHT 600

#define PLOT3 0 // draw 3x3 sparks

#define HAVE_BANNER 0

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <arm_compat.h>
#include "SineTable.h"
#define sine(t) (((int)sineTable[(t)&0xff])<<2)
#define cosine(t) sine((t) + 64)
#include "BurstTable.h"

#define BLACK 0x0000
#define SPLAT 0x0421

// Display colours in RGB 565 format
#define RED   (unsigned short)0xf800
#define GREEN (unsigned short)0x07e0
#define BLUE  (unsigned short)0x001f

#define WHITE (unsigned short)(RED | GREEN | BLUE)

#define YELLOW  (unsigned short)(RED | GREEN)
#define CYAN    (unsigned short)(GREEN | BLUE)
#define MAGENTA (unsigned short)(RED | BLUE)

static unsigned short colors[] =
{
	WHITE,

	RED,
	GREEN,
	BLUE,

	YELLOW,
	CYAN,
	MAGENTA
};

// NEON needs to access all the data of a structure to
// be able to vectorize a loop that includes the structure.
// Therefore the original Sparks structure has been split
// into three, and the enum split into separate Shells and Sparks

typedef struct Shell Shell;
typedef struct SparkPosition SparkPosition;
typedef struct SparkData SparkData;

enum
{
	burstShell = 0,
	fountainShell,
	twinkleShell,
};

enum
{
	burstTrailSpark=0,
	burstSpark,
	fountainSpark,
	twinkleSpark,
};

// Each spark type now has its own declaration.
// This is done to be able to treat all of them
// independently while creating them or moving them.
// This could be improved by reintegrating the "type" data
// in the spark structure but it would need to be done
// carefully to preserve NEON vectorization.

static Shell * shells = NULL;
static int shellCount = 0;
static int maxShells = 7;

static int shellDrag[3]={6,0,7};
static int sparkDrag[4]={3,5,5,4};
static int sparkFade[4]={6,4,6,6};

static SparkPosition * __restrict fountainSparkPosition;
static SparkPosition * __restrict previousFountainSparkPosition;
static SparkData * __restrict fountainSparkData;
static int fountainSparkCount=0;
static int fountainSparkCountTop=0;
static const int maxFountainSpark=5000;

static SparkPosition * __restrict burstTrailSparkPosition;
static SparkPosition * __restrict previousBurstTrailSparkPosition;
static SparkData * __restrict burstTrailSparkData;
static int burstTrailSparkCount=0;
static int burstTrailSparkCountTop=0;
static const int maxBurstTrailSpark=5000;

static SparkPosition * __restrict twinkleSparkPosition;
static SparkPosition * __restrict previousTwinkleSparkPosition;
static SparkData * __restrict twinkleSparkData;
static int twinkleSparkCount=0;
static int twinkleSparkCountTop=0;
static const int maxTwinkleSpark=5000;

static SparkPosition * __restrict burstSparkPosition;
static SparkPosition * __restrict previousBurstSparkPosition;
static SparkData * __restrict burstSparkData;
static int burstSparkCount=0;
static int burstSparkCountTop=0;
static const int maxBurstSpark=5000;

static const int gravity = 1 << 11;

#define inBounds(s) ((s)->x >= 0 && (s)->x < (LCD_WIDTH << 16) && (s)->y >= 0 && (s)->y < (LCD_HEIGHT << 16))
#define inBoundsShell(s) (((s)->x1 >= 0 && (s)->x1 < (LCD_WIDTH << 16) && (s)->y1 >= 0 && (s)->y1 < (LCD_HEIGHT << 16)) && ((s)->x2 >= 0 && (s)->x2 < (LCD_WIDTH << 16) && (s)->y2 >= 0 && (s)->y2 < (LCD_HEIGHT << 16)))

struct Shell 
{
	int x1;		// Position of the Shell in the previous iteration
	int y1;
	int x2;		// Position of the Shell in the current iteration
	int y2;
	int xs;
	int ys;
	int age;
	int type;	
	int color;	
	int data1;
};

struct SparkPosition
{
	int x;
	int y;
};

struct SparkData
{
	int color;
	int age;
	int xs;
	int ys;
};

#if HAVE_BANNER
#include <string.h>
#include "banner_data.h"

int banner_x = -BANNER_WIDTH;

static void plot_pixels(int pixels_width, int pixels_height, const pixel_t *pixels, int x, int y) {
	/* not very general: does not handle cropping on the top edge */
	int excess_width = (x + pixels_width) - LCD_WIDTH;
	int excess_height = (y + pixels_height) - LCD_HEIGHT;
	int width = (0 < excess_width) ? pixels_width - excess_width : pixels_width;
	int height = (0 < excess_height) ? pixels_height - excess_height : pixels_height;
	int skipleft = 0;
	int row;

	if (x < 0) {
		skipleft = -x;
		width += x;
		x = 0;
	}
	if (width < 0) return;
	if (height < 0) return;

	for (row = 0; row < height; ++row) {
		memcpy(&((unsigned short *)FRAME_BUFFER)[(row + y) * LCD_WIDTH + x],
			   &pixels[row * pixels_width + skipleft],
			   width * sizeof(pixel_t));
	}
}

static void moveBanner() {
	plot_pixels(BANNER_WIDTH, BANNER_HEIGHT, banner_data, banner_x, 0);
	++banner_x;
	if (banner_x == LCD_WIDTH) banner_x = -BANNER_WIDTH;
}
#endif /* HAVE_BANNER */

/* returns a pseudo random value >= 0 and < limit */
static __inline int my_random(int limit)
{
	return (rand() & 0xffff) * limit >> 16;
}

#if PLOT3
static void plot3(int x, int y, int color)
{
	if (0 < x)
		((volatile unsigned short *)FRAME_BUFFER)[y * LCD_WIDTH + x - 1] = color;
	((volatile unsigned short *)FRAME_BUFFER)[y * LCD_WIDTH + x] = color;
	if (x+1 < LCD_WIDTH)
		((volatile unsigned short *)FRAME_BUFFER)[y * LCD_WIDTH + x + 1] = color;
}

static void plot(int x, int y, int color)
{
	if (0 < y)
		plot3(x, y - 1, color);
	plot3(x, y, color);
	if (y+1 < LCD_HEIGHT)
		plot3(x, y + 1, color);
}
#elif PLOT2
static void plot2(int x, int y, int color)
{
	((volatile unsigned short *)FRAME_BUFFER)[y * LCD_WIDTH + x] = color;
	if (x+1 < LCD_WIDTH)
		((volatile unsigned short *)FRAME_BUFFER)[y * LCD_WIDTH + x + 1] = color;
}

static void plot(int x, int y, int color)
{
	plot2(x, y, color);
	if (y+1 < LCD_HEIGHT)
		plot2(x, y + 1, color);
}
#else
static void plot(int x, int y, int color)
{
//	__nop();  // Used in Step 3
	((volatile unsigned short *)FRAME_BUFFER)[y * LCD_WIDTH + x] = color;
}
#endif


// This function has been stripped of display and fade effects
// to leave something which can be auto-vectorized.
// This simple function can handle the moving of all types of sparks.
// Optimal NEON code generation could be achieved if loopCount was a multiple of four
// (which is not the case here) and then a for() loop like this could be used:
// 	for(i = 0; i < (loopCount >> 2) << 2; i++)

static void moveSpark(SparkPosition * __restrict sparkPosition, SparkPosition * __restrict previousSparkPosition, 
                      SparkData * __restrict sparkData, int loopCount, int type)
{
	int i;

// Add me in Step 2

//   	if (loopCount >= 0x1000)
//   	{
//   		start_perfmon();

	for(i = 0; i < loopCount; i++)
	{
		previousSparkPosition[i].x = sparkPosition[i].x;
		previousSparkPosition[i].y = sparkPosition[i].y;

		sparkPosition[i].x += sparkData[i].xs;
		sparkPosition[i].y += sparkData[i].ys;

		sparkData[i].age -=1;
		sparkData[i].color += 0;

		sparkData[i].ys += gravity;

		sparkData[i].xs -= sparkData[i].xs >> sparkDrag[type];
		sparkData[i].ys -= sparkData[i].ys >> sparkDrag[type];
	}

// Add me in Step 2
//   	stop_perfmon();
//   	}
}

// Create a new Shell. Apart from introduction of x2 and y2
// no modification from the original program.
// This function cannot be optimized for NEON because there is no loop.

static void newShell(void)
{
	int direction, speed;
	int r = my_random(100);
	int shellType = r < 10 ? fountainShell : r < 20 ? twinkleShell : burstShell;
	Shell * s = shells + shellCount;

	s->x1 = my_random(LCD_WIDTH) << 16;
	s->y1 = (LCD_HEIGHT - 1) << 16;
	s->x2 = s->x1;
	s->y2 = s->y1;
	s->type = shellType;
	shellCount++;

	switch(shellType)
	{
		case fountainShell:
			s->age = 255;
			s->xs = 0;
			s->ys = 0;
			s->color = my_random(7);
			break;
		case twinkleShell:
			s->data1 = 3;
			s->color = 0;

			direction = my_random(20) +10;
			if(s->x1 > LCD_WIDTH << 15)
			{
				direction = -direction;	
			}
			speed = 14 + my_random(6);
			s->xs = sine(direction) * speed / 2;
			s->ys = cosine(direction) * -speed / 2;
			s->age = 200 + my_random(50);
			break;
		case burstShell:
			s->data1 = my_random(11) - 5;
			s->color = 4;
			direction = my_random(31) - 15;
			speed = 14 + my_random(8);
			s->xs = sine(direction) * speed / 2;
			s->ys = cosine(direction) * -speed / 2;
			s->age = 90 + my_random(40);
			break;
	}
}


// As function calls (like random) within loops prevent auto-vectorization,
// this loop has been split into two.  First calculate all the "rand" values
// and store them in an array and then create all the sparks.
// The second loop is auto-vectorized optimally because the loop count is a multiple of four.

static void newBurst(Shell *p)
{
	int doubleBurst = my_random(10) < 2; // doubleBurst = 0 or 1
	int colour;
	int speed = 16;

	int i, j;
	int age[256];

	for(j = 0; j < doubleBurst + 1; j++)
	{
		colour = my_random(7);

		if ( (burstSparkCount + 256) < maxBurstSpark)
		{
			for(i = 0; i < 256; i++)
			{
				age[i] = 40 + my_random(20);
			}

			for(i = 0; i < 256; i++)
			{
				burstSparkData[burstSparkCount + i].xs = p->xs + ((int)burstXTable[i]) * speed;
				burstSparkData[burstSparkCount + i].ys = p->ys + ((int)burstYTable[i]) * speed;
				burstSparkData[burstSparkCount + i].age = age[i];
				burstSparkData[burstSparkCount + i].color = colour;

				burstSparkPosition[burstSparkCount + i].x = p->x2;
				burstSparkPosition[burstSparkCount + i].y = p->y2;
			}

			burstSparkCount += 256;
		}
	}

    if (burstSparkCount > burstSparkCountTop)
        burstSparkCountTop = burstSparkCount;
}


// This function has no loop so no auto-vectorization opportunities here.

static void newTrail(Shell *p)
{
	int direction;

	if ( burstTrailSparkCount + 1 < maxBurstTrailSpark )
	{
		direction = p->age * p->data1 + my_random(20);

		burstTrailSparkData[burstTrailSparkCount].xs = p->xs + sine(direction) * 2;
		burstTrailSparkData[burstTrailSparkCount].ys = p->ys + cosine(direction) * 2;
		burstTrailSparkData[burstTrailSparkCount].color = p->color;
		burstTrailSparkData[burstTrailSparkCount].age = my_random(30) + 30;

		burstTrailSparkPosition[burstTrailSparkCount].x = p->x2;
		burstTrailSparkPosition[burstTrailSparkCount].y = p->y2;

		burstTrailSparkCount++;
	}

    if (burstTrailSparkCount > burstTrailSparkCountTop)
        burstTrailSparkCountTop = burstTrailSparkCount;
}


// As function calls (like random) within loops prevent auto-vectorization,
// this loop has been split into two.  First calculate all the
// "rand" and "sine" values and store them in an array and then create all the sparks.
// The second loop is auto-vectorized optimally because the loop count is a multiple of four.
// The former spray1() has been merged into this spray().

static void spray(Shell *p)
{
	int i = 0;

	if ( (fountainSparkCount + 4) < maxFountainSpark)
	{
		int direction[4], speed[4], offset[4], age[4];
		int sin[4], cosin[4];

		for (i = 0; i < 4; i++)
		{
			direction[i] = my_random(21) - 10;
			speed[i] = my_random(40);
			offset[i] = my_random(16);
			age[i] = my_random(100) + 100;
			sin[i] = sine(direction[i]);
			cosin[i] = cosine(direction[i]);
		}

		if(p->age == 128)
		{
			p->color = my_random(7);
		}

		for (i = 0; i < 4; i++)
		{
			fountainSparkData[fountainSparkCount + i].xs = sin[i] * (speed[i] >> 2);    // /4 replaced by >> 2 to vectorize
			fountainSparkData[fountainSparkCount + i].ys = cosin[i] * (-speed[i] >> 2); // /4 replaced by >> 2 to vectorize
			fountainSparkData[fountainSparkCount + i].age = age[i];
			fountainSparkData[fountainSparkCount + i].color = p->color;

			fountainSparkPosition[fountainSparkCount + i].x = p->x2 + fountainSparkData[fountainSparkCount + i].xs * (offset[i] >> 4); // /16 replaced by >> 4 to vectorize
			fountainSparkPosition[fountainSparkCount + i].y = p->y2 + fountainSparkData[fountainSparkCount + i].ys * (offset[i] >> 4); // /16 replaced by >> 4 to vectorize
		}

		fountainSparkCount += 4;
	}

    if (fountainSparkCount > fountainSparkCountTop)
        fountainSparkCountTop = fountainSparkCount;
}


// As function calls (like random) within loops prevent auto-vectorization,
// this loop has been split into two.  First calculate all the
// "rand" and "sine" values and store them in an array and then create all the sparks.
// The second loop is auto-vectorized optimally because the loop count is a multiple of four.
// The former twinkleTrail1() has been merged into this twinkleTrail().

static void twinkleTrail(Shell *p)
{
	int i = 0;

	if ( (twinkleSparkCount + 4) < maxTwinkleSpark)
	{
		int direction[4], speed[4];
		int sin[4], cosin[4];

		for (i = 0; i < 4; i++)
		{
			direction[i] = my_random(256);
			speed[i] = my_random(4) + 1;
			sin[i] = sine(direction[i]);
			cosin[i] = cosine(direction[i]);
		}	

		for (i = 0; i < 4; i++)
		{				
			twinkleSparkData[twinkleSparkCount + i].xs = p->xs + ((sin[i] * speed[i]) >> 2);    // /4 replaced by >> 2 to vectorize
			twinkleSparkData[twinkleSparkCount + i].ys = p->ys + ((cosin[i] * -speed[i]) >> 2); // /4 replaced by >> 2 to vectorize
			twinkleSparkData[twinkleSparkCount + i].age = 140;
			twinkleSparkData[twinkleSparkCount + i].color = p->color;

			twinkleSparkPosition[twinkleSparkCount + i].x = p->x2;
			twinkleSparkPosition[twinkleSparkCount + i].y = p->y2;
		}

		twinkleSparkCount += 4;
	}

    if (twinkleSparkCount > twinkleSparkCountTop)
        twinkleSparkCountTop = twinkleSparkCount;
}


// This function is the Shell equivalent of moveSparks().
// Move all Shells (max 7), and create Sparks as needed.
// As this function is a loop of maximum 7 iterations and uses
// different types of shell / spark, it cannot be auto-vectorized.

static void moveShell(Shell *s)
{
	int i;

	for (i = 0; i < shellCount; i++)
	{
		(s+i)->age--;

		if((s+i)->age == 0)
		{
			if((s+i)->type == burstShell)
			{
				newBurst((s+i));
			}

			plot((s+i)->x1 >> 16, (s+i)->y1 >> 16, BLACK);
			plot((s+i)->x2 >> 16, (s+i)->y2 >> 16, BLACK);

			*(s+i) = shells[--shellCount];
			i--;
		}
		else
		{
			(s+i)->x1 = (s+i)->x2;
			(s+i)->y1 = (s+i)->y2;

			(s+i)->x2 += (s+i)->xs;
			(s+i)->y2 += (s+i)->ys;

			(s+i)->ys += gravity;

			(s+i)->xs -= (s+i)->xs >> shellDrag[(s+i)->type];
			(s+i)->ys -= (s+i)->ys >> shellDrag[(s+i)->type];

			switch((s+i)->type)
			{
				case burstShell :
					newTrail(s+i);
					break;
				case fountainShell :
					spray(s+i);
					break;
				case twinkleShell:
					twinkleTrail(s+i);
					break;
			}
		}
	}
}


// This function will display the spark and get rid of the
// "dead" spark (the one which have reached age = 0).  This function
// also includes all the parts which were initially in moveSpark().
// The main difference is that in the original fireworks the program
// was calculating each spark and drawing it. Here we calculate
// the position of all sparks (using NEON) from moveSpark()
// and then draw them all in a second pass with this function.

static void displaySpark(SparkPosition * __restrict sparkPosition, SparkPosition * __restrict previousSparkPosition, 
                         SparkData * __restrict sparkData, int * count, int type)
{
	int i;

	for(i = 0; i < *count; i++)
	{
		if ((sparkData+i)->age == 0)
		{
			if inBounds(previousSparkPosition+i)
                plot((previousSparkPosition+i)->x >> 16, (previousSparkPosition+i)->y >> 16, BLACK);
			if ( i != ((*count)-1) )
			{
				sparkPosition[i] = sparkPosition[(*count)-1];
				previousSparkPosition[i] = previousSparkPosition[(*count)-1];
				sparkData[i] = sparkData[(*count)-1];
				i--;
			}
			(*count)--;
		}
		else
		{
			if (inBounds(sparkPosition+i))
			{
				short color = colors[(sparkData+i)->color];
				short fade = (sparkData+i)->age << 5 >> sparkFade[type];
				if (fade < 32)
				{
					if (type == twinkleSpark)
					{
						color = 0;
						sparkData[i].age = 1; // We do not need to move this spark anymore
					}
					else
					{
						color &= fade * SPLAT;
					}
				}
				plot((sparkPosition+i)->x >> 16, (sparkPosition+i)->y >> 16, color);
			}
			if (inBounds(previousSparkPosition+i))
                plot((previousSparkPosition+i)->x >> 16, (previousSparkPosition+i)->y >> 16, BLACK);
		}
	}
}


// This function is moving the shells (max 7) and then moving
// the Sparks by calling displaySpark().

static void displayThem(void)
{
	int i;

	for(i = 0; i < shellCount; i++)
	{
		if (inBoundsShell(shells+i))
		{
			short color = colors[(shells+i)->color];
			plot((shells+i)->x1 >> 16, (shells+i)->y1 >> 16, BLACK);
			plot((shells+i)->x2 >> 16, (shells+i)->y2 >> 16, color);
		}
		else
		{
			plot((shells+i)->x1 >> 16, (shells+i)->y1 >> 16, BLACK);
		}
	}

	displaySpark(burstTrailSparkPosition, previousBurstTrailSparkPosition, burstTrailSparkData, &burstTrailSparkCount, burstTrailSpark);
	displaySpark(burstSparkPosition,      previousBurstSparkPosition,      burstSparkData,      &burstSparkCount,      burstSpark);
	displaySpark(twinkleSparkPosition,    previousTwinkleSparkPosition,    twinkleSparkData,    &twinkleSparkCount,    twinkleSpark);
	displaySpark(fountainSparkPosition,   previousFountainSparkPosition,   fountainSparkData,   &fountainSparkCount,   fountainSpark);
}


// Allocate memory for all structures.
// This is similar to the original fireworks but with more instances because all structures have been split.

static void initSparks(void)
{
	shells = malloc(sizeof (Shell) * maxShells);

	fountainSparkPosition = (SparkPosition*)malloc(sizeof(SparkPosition)*maxFountainSpark);
	previousFountainSparkPosition = (SparkPosition*)malloc(sizeof(SparkPosition)*maxFountainSpark);
	fountainSparkData = (SparkData*)malloc(sizeof(SparkData)*maxFountainSpark);

	burstTrailSparkPosition = (SparkPosition*)malloc(sizeof(SparkPosition)*maxBurstTrailSpark);
	previousBurstTrailSparkPosition = (SparkPosition*)malloc(sizeof(SparkPosition)*maxBurstTrailSpark);
	burstTrailSparkData = (SparkData*)malloc(sizeof(SparkData)*maxBurstTrailSpark);

	twinkleSparkPosition = (SparkPosition*)malloc(sizeof(SparkPosition)*maxTwinkleSpark);
	previousTwinkleSparkPosition = (SparkPosition*)malloc(sizeof(SparkPosition)*maxTwinkleSpark);
	twinkleSparkData = (SparkData*)malloc(sizeof(SparkData)*maxTwinkleSpark);

	burstSparkPosition = (SparkPosition*)malloc(sizeof(SparkPosition)*maxBurstSpark);
	previousBurstSparkPosition = (SparkPosition*)malloc(sizeof(SparkPosition)*maxBurstSpark);
	burstSparkData = (SparkData*)malloc(sizeof(SparkData)*maxBurstSpark);
}


static void drawSparks(void)
{
	if(shellCount < maxShells)
	{
		newShell();
	}

	moveShell(shells);

    moveSpark( burstTrailSparkPosition, previousBurstTrailSparkPosition, burstTrailSparkData, burstTrailSparkCount, burstTrailSpark);
	moveSpark( burstSparkPosition,      previousBurstSparkPosition,      burstSparkData,      burstSparkCount,      burstSpark);
	moveSpark( fountainSparkPosition,   previousFountainSparkPosition,   fountainSparkData,   fountainSparkCount,   fountainSpark);
	moveSpark( twinkleSparkPosition,    previousTwinkleSparkPosition,    twinkleSparkData,    twinkleSparkCount,    twinkleSpark);

	displayThem();
#if HAVE_BANNER
		moveBanner();
#endif /* HAVE_BANNER */
}

static void fireworks(void)
{
	int i;

    srand(1);  /* for repeatable results.  Or use  srand(clock());  for 'more random' results */

	initSparks();

    while (1)
    {
    	//clock_t start;//  Add me in Step 3
    	//start = clock(); //  Add me in Step 3
        for (i = 0; i < 10000; i++)
        {
            drawSparks();
        }
        //start = clock() - start; //  Add me in Step 3
        //double time_taken = ((double)start/100); //  Add me in Step 3
        //printf("Time in seconds = %f\n", time_taken);  //  Add me in Step 3
    }

}


static void clear_screen(void)
{
	unsigned int i;

	for (i = 0; i < (LCD_WIDTH*LCD_HEIGHT); i++)
	{
		((volatile unsigned short *)FRAME_BUFFER)[i] = BLACK;
	}
}

// declaration of 'extern' functions
extern void enable_caches(void); // in startup.S
extern void init_screen(unsigned int fb, unsigned int w, unsigned int h, unsigned int processor);   // in screen.c


int main(void)
{	unsigned int id = 0;

	printf("Fireworks Optimization Example\r\n\r\n");

	enable_caches(); // in startup.S
    //init the screen registers
	init_screen(FRAME_BUFFER, LCD_WIDTH, LCD_HEIGHT, id);   // in screen.c
	clear_screen();
    //run the fireworks!!
	fireworks();

	return 0;
}

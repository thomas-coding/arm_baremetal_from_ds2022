/* ------------------------------------------------------------
 types.h
 Header File

 Copyright (c) 2011 Arm Limited (or its affiliates). All rights reserved.
 Use, modification and redistribution of this file is subject to your possession of a
 valid End User License Agreement for the Arm Product of which these examples are part of 
 and your compliance with all applicable terms and conditions of such licence agreement.
------------------------------------------------------------
*/

/* types.h */
#if !defined (__TYPES_)                
/* Prevent multiple includes */
#define __TYPES_

/* Shortened text for signed char */
typedef signed char         s_char;
/* Shortened text for unsigned char */
typedef unsigned char       u_char;
/* Shortened text for unsigned int */
typedef unsigned int        u_int;
/* Shortened text for unsigned short */
typedef unsigned short      u_short;
/* Shortened text for signed short int */
typedef short int           s_short;
/* Shortened text for unsigned long */
typedef unsigned long       u_long;
/* Shortened text for signed long */
typedef long                s_long;
/* Best we can do to represent a BYTE */
typedef unsigned char       BYTE;

/*
*  Setup the current limits and size typedefs for the defined processor.
*  This will define constants for the sizes of integral types, with
*  minimums and maximum values allowed for the define.
*  Note: mins and maxs will be different for different processors.
*                               Size (Bytes)    Alignment (Bytes)
*/

/* size casting */
typedef signed char    int8;      /*  1                   1   */
typedef unsigned char  uint8;     /*  1                   1   */
typedef short int      int16;     /*  2                   2   */
typedef unsigned short uint16;    /*  2                   2   */
typedef long int       int32;     /*  4                   4   */
typedef unsigned long  uint32;    /*  4                   4   */
#if !defined(_WIN32)
typedef unsigned long long uint64;/*  8                   ?   */
#else
typedef unsigned __int64 uint64;
#endif
/* generic names for size types */
typedef signed char    schar;    /*  1                   1   */
typedef unsigned char  uchar;    /*  1                   1   */
typedef short int      sshort;   /*  2                   2   */
typedef unsigned short ushort;   /*  2                   2   */
typedef unsigned int   uint;     /*  2                   2   */
typedef short          sint;     /*  2                   2   */
typedef long int       slint;    /*  4                   4   */
typedef long int       slong;    /*  4                   4   */
typedef unsigned long  ulint;    /*  4                   4   */
typedef unsigned long  ulong;    /*  4                   4   */

#define DIM(array) (sizeof(array)/sizeof(array[0]))

#endif




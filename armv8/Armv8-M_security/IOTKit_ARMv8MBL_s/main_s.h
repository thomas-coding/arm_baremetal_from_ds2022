/*
** Copyright (c) 2017 Arm Limited (or its affiliates). All rights reserved.
** Use, modification and redistribution of this file is subject to your possession of a
** valid End User License Agreement for the Arm Product of which these examples are part of 
** and your compliance with all applicable terms and conditions of such licence agreement.
*/


/* Secure-only functions */
int secure_fn1(int x);
void nonsecure_init(void);

/* Non-secure Callable functions */
int __attribute__((cmse_nonsecure_entry)) ns_callable_fn1(int); 
int __attribute__((cmse_nonsecure_entry)) ns_callable_fn2(int);

/* Non-secure function type declaration */ 
typedef void __attribute__((cmse_nonsecure_call)) nsfunc(void);

/* Secure data declaration */
extern const int secure_data __attribute__((section(".ARM__at_0x1FFFDC")));

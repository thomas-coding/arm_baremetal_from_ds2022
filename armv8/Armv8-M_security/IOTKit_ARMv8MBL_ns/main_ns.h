/*
** Copyright (c) 2017 Arm Limited (or its affiliates). All rights reserved.
** Use, modification and redistribution of this file is subject to your possession of a
** valid End User License Agreement for the Arm Product of which these examples are part of
** and your compliance with all applicable terms and conditions of such licence agreement.
*/


/* Non-secure Callable (Secure) functions */
extern int ns_callable_fn1(int); 
extern int ns_callable_fn2(int);

/* Non-secure functions */
void read_secure_data(unsigned int);
void write_data(unsigned int, unsigned int);

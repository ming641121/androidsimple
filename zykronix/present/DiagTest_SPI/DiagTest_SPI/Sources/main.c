/*
** Copyright (C) Magic Pixel Inc. All rights reserved.
*/

#include <stdio.h>
#include "declare.h"
#include "Global.h"
extern void Main_SPI(void);
int main(void)
{
  #pragma import(__use_no_semihosting_swi)   // ensure no functions that use semihosting
                                             // SWIs are linked in from the C library
       Main_SPI();
}


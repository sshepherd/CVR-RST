/* color.c
   ======= 
   Author: R.J.Barnes
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>
#include "rfbuffer.h"



unsigned int FrameBufferColor(unsigned int r,unsigned int g,unsigned int b,
                              unsigned int a) {
  unsigned int c;
  c=(a<<24) | (r<<16) | (g<<8) | b;
  return c;
}


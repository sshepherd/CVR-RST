/* freq.h
   ======
   Author: R.J.Barnes
*/


/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/

#ifndef _FREQ_H
#define _FREQ_H


#define DEFAULT_FREQ 13000

struct FreqTable {
  int num;
  int dfrq;
  int *start;
  int *end;
};

struct FreqTable *FreqLoadTable(FILE *fp);
int FreqTest(struct FreqTable *ptr,int freq);
   	
#endif

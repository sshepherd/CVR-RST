/* oldgtablewrite.h
   ================
   Author: R.J.Barnes
*/


/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/




#ifndef _OLDGTABLEWRITE_H
#define _OLDGTABLEWRITE_H

int OldGridTableWrite(int fp,struct GridTable *ptr,char *log,int xtd);
int OldGridTableFwrite(FILE *fp,struct GridTable *ptr,char *log,int xtd);

#endif

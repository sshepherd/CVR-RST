/* iqseek.h
   ========= 
   Author: R.J.Barnes
*/


/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/



#ifndef _IQSEEK_H
#define _IQSEEK_H

int IQSeek(int fid,
	      int yr,int mo,int dy,int hr,int mt,int sc,double *atme,
	      struct IQIndex *inx);
int IQFseek(FILE *fp,
	    int yr,int mo,int dy,int hr,int mt,int sc,double *atme,
	       struct IQIndex *inx);

#endif

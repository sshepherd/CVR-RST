/* iqwrite.h
   ======== 
   Author: R.J.Barnes
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/




#ifndef _IQWRITE_H
#define _IQWRITE_H


int IQEncode(struct DataMap *ptr,struct IQ *iq,unsigned int *badtr,int16 *samples);

int IQWrite(int fid,struct RadarParm *prm,
	    struct IQ *iq,unsigned int *badtr,int16 *samples);

int IQFwrite(FILE *fp,struct RadarParm *prm,
	     struct IQ *iq,unsigned int *badtr,int16 *samples);

#endif


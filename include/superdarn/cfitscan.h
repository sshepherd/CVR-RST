/* cfitscan.h
  ==========
   Author: R.J.Barnes
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/



#ifndef _CFITSCAN_H
#define _CFITSCAN_H

int CFitReadRadarScan(struct CFitfp *fp,int *state,
                 struct RadarScan *ptr,struct CFitdata *fit,int tlen,
                 int lock,int chn);

int CFitToRadarScan(struct RadarScan *ptr,struct CFitdata *fit);


#endif


/* inv_mag.h
   =========
   Author: R.J.Barnes
*/


/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/




#ifndef _INVMAG_H
#define _INVMAG_H


void RPosRngBmAzmElv(int bm,int rn,int year,
                  struct RadarSite *hdw,double frang,
                  double rsep,double rx,double height,
		  double *azm,double *elv);

void RPosInvMag(int bm,int rn,int year,struct RadarSite *hdw,double frang,
             double rsep,double rx,double height,
             double *mlat,double *mlon,double *azm);

#endif

/* nrfit.h
   =====
   Author: R.J.Barnes
*/


/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/




#ifndef _NRFIT_H
#define _NRFIT_H

void nrfit(float *x,float *y,int ndata,float *sig,int mwt,
         float *a,float *b,float *siga,float *sigb,float *chi2,
	 float *q);

#endif














/* fitdata.h
   ========== 
   Author: R.J.Barnes
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/




#ifndef _FITDATA_H
#define _FITDATA_H

#ifndef _FITBLK_H
#include "fitblk.h"
#endif

struct FitData {
  struct {
    int major;
    int minor;
  } revision;
  struct FitNoise noise;
  struct FitRange *rng;
  struct FitRange *xrng;
  struct FitElv  *elv;
};

struct FitData *FitMake();
void FitFree(struct FitData *ptr);
int FitSetRng(struct FitData *ptr,int nrang);
int FitSetXrng(struct FitData *ptr,int nrang);
int FitSetElv(struct FitData *ptr,int nrang);

void *FitFlatten(struct FitData *ptr,int nrang,size_t *size);
int FitExpand(struct FitData *ptr,int nrang,void *buffer);

#endif

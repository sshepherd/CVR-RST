/* cnvmapindex.h
   ============= 
   Author: R.J.Barnes
*/


/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/



#ifndef _CNVMAPINDEX_H
#define _CNVMAPINDEX_H


struct CnvMapIndex {
  int num;
  double *tme;
  int *inx;
};

void CnvMapIndexFree(struct CnvMapIndex *);
struct CnvMapIndex *CnvMapIndexLoad(int fid);
struct CnvMapIndex *CnvMapIndexFload(FILE *fp);
 


#endif

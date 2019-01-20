/* evalefield.h
   ============  
   Author: R.J.Barnes
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/




#ifndef _EVALEFIELD_H
#define _EVALEFIELD_H

void CnvMapEvalEfield(int Lmax,double *coef,double *plm,
		     struct CnvGrid *vptr,double latmin,
		     struct CnvMapData *ptr);

#endif

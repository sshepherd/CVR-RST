/* cnvmapwrite.h 
   ============
   Author: R.J.Barnes
*/


/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/




#ifndef _CNVMAPWRITE_H
#define _CNVMAPWRITE_H

int CnvMapWrite(int fd,struct CnvMapData *map,struct GridData *grd);
int CnvMapFwrite(FILE *fp,struct CnvMapData *map,struct GridData *grd);

#endif










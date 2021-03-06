/* rawseek.c
   =========
   Author: R.J.Barnes
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"
#include "oldrawread.h"



int OldRawSeek(struct OldRawFp *fp,
	      int yr,int mo,int dy,int hr,int mt,int sc,double *atme) {
  int status=0;
  double tval=0;
  int rjmp=0;
  struct RadarParm prm;
  struct RawData raw;

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
 
  if (tval<fp->ctime) {
    lseek(fp->rawfp,fp->frec,SEEK_SET);
    fp->ptr=fp->frec;
    status=OldRawRead(fp,&prm,&raw);
  }
 
  do {
    rjmp=fp->rlen;
    status=OldRawRead(fp,&prm,&raw);
  } while ((tval>=fp->ctime) && (status==0));
   

  lseek(fp->rawfp,-(fp->rlen+rjmp),SEEK_CUR);
  fp->ptr-=fp->rlen+rjmp;
  
  status=OldRawRead(fp,&prm,&raw);
  
  lseek(fp->rawfp,-fp->rlen,SEEK_CUR);
  fp->ptr-=fp->rlen;
  
  if (atme !=NULL) *atme=fp->ctime;
  return 4;
}



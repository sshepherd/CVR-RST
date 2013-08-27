/* istp.h
   ====== 
   Author: R.J.Barnes
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/


#define FILL_VALUE 9999.0

struct imfdata {
  char sat[8];
  char ins[8];
  int cnt;
  double *time;
  float *BGSMc;
  float *BGSEc;
};


  

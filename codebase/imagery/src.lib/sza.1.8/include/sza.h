/* sza.h
   =====
   Author: R.J.Barnes
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/



#ifndef _SZA_H
#define _SZA_H

double SZASolarDec(int yr,int mo,int dy,int hr,int mt,int sc);
double SZAEqOfTime(int yr,int mo,int dy,int hr,int mt,int sc);
double SZAAngle(double lon,double lat,double dec,double Hangle);


#endif

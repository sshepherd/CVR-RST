/* plot_imf.h
   ================
   Author: R.J.Barnes
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/



void plot_imf(struct Plot *plot,
	      float xoff,float yoff,float rad,
	      float bx,float by,float bz,float mxval,
  	      unsigned int color,unsigned char mask,float width,
              char *fontname,float fontsize,
              void *txtdata);

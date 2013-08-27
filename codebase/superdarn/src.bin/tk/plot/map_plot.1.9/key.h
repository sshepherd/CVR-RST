/* key.h
   =====
   Author: R.J.Barnes
*/



/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/



struct key {
  int num;
  double min,max;
  unsigned int defcol;
  unsigned char *a;
  unsigned char *r;
  unsigned char *g;
  unsigned char *b;
};

int load_key(FILE *fp,struct key *key);

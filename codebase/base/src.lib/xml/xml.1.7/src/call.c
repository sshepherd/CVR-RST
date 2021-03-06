/* rxml.c
   ======
   Author: R.J.Barnes
*/
 

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>
#include "rxml.h"



int XMLCallEnd(struct XMLdata *ptr,char *name,char *buf,int sze) {
  if (ptr->interface.end.func !=NULL) 
      return (ptr->interface.end.func)(name,buf,sze,ptr->interface.end.data);
  return 0;   
}

int XMLCallStart(struct XMLdata *ptr,
                 char *name,char end,int atnum,char **atname,char **atval,
                 char *buf,int sze) {
  
 if (ptr->interface.start.func !=NULL)
   return (ptr->interface.start.func)(name,end,atnum,atname,atval,
                               buf,sze,ptr->interface.start.data);
  return 0;

}







/* fontdb.h
   ======== 
   Author R.J.Barnes
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/



#ifndef _FONTDB_H
#define _FONTDB_H

struct FrameBufferFontDB {
  char *path;
  char *buf;
  int flg;
  int num;
  struct FrameBufferFont **font;
  struct FrameBufferFont *dfont;
};

void FrameBufferFontDBFree(struct FrameBufferFontDB *ptr);
struct FrameBufferFontDB *FrameBufferFontDBLoad(FILE *fp);
struct FrameBufferFont *FrameBufferFontDBFind(struct FrameBufferFontDB *ptr,char *name,int sze);

#endif
 

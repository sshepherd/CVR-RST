/* dmap.c
   ====== 
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
#include "rconvert.h"
#include "dmap.h"



struct DataMap *DataMapMake() {
  struct DataMap *ptr;
  ptr=malloc(sizeof(struct DataMap));
  if (ptr==NULL) return ptr;
  ptr->sze=0;
  ptr->buf=NULL;
  ptr->scl=NULL;
  ptr->snum=0;
  ptr->arr=NULL;
  ptr->anum=0;
  return ptr;
}

struct DataMapScalar* DataMapMakeScalar(char *name,int mode,int type,
                                        void *data) {
  struct DataMapScalar *ptr;

  if (name==NULL) return NULL;
  ptr=malloc(sizeof(struct DataMap));
  if (ptr==NULL) return ptr;
  ptr->name=malloc(strlen(name)+1);
  if (ptr->name==NULL) {
    free(ptr);
    return NULL;
  }
  strcpy(ptr->name,name);
  ptr->mode=mode;
  ptr->type=type;
  ptr->data.vptr=data;
  return ptr;
}




void DataMapFreeScalar(struct DataMapScalar *ptr) {
  if (ptr==NULL) return; 
  if (ptr->name !=NULL) free(ptr->name);
  if ((ptr->mode !=0) && (ptr->data.vptr !=NULL)) free(ptr->data.vptr);
  free(ptr);
  return;
}

int DataMapAddScalar(struct DataMap *ptr,
		     char *name,int type,void *data) {
  struct DataMapScalar *s;
  if (ptr==NULL) return -1;
  s=DataMapMakeScalar(name,0,type,data);
  if (s==NULL) return -1;

  if (ptr->scl==NULL) ptr->scl=malloc(sizeof(struct DataMapScalar *));
  else {
    struct DataMapScalar **tmp;
    tmp=realloc(ptr->scl,(ptr->snum+1)*sizeof(struct DataMapScalar *));
    if (tmp==NULL) {
      DataMapFreeScalar(s);
      return -1;
    }
    ptr->scl=tmp;
  }
  ptr->scl[ptr->snum]=s;
  ptr->snum++;
  return 0;
}

void *DataMapStoreScalar(struct DataMap *ptr,
                         char *name,int type,void *data) {

  struct DataMapScalar *s;
  void *tmp=NULL;
 
  if (ptr==NULL) return NULL;
  if (data==NULL) return NULL;
  switch (type) {
  case DATACHAR:
    tmp=malloc(sizeof(char));
    if (tmp==NULL) break;
    *((char *) tmp)=*((char *) data);
    break;
  case DATASHORT:
    tmp=malloc(sizeof(int16));
    if (tmp==NULL) break;
    *((int16 *) tmp)=*((int16 *) data);
    break;
  case DATAINT:
    tmp=malloc(sizeof(int32));
    if (tmp==NULL) break;
    *((int32 *) tmp)=*((int32 *) data);
    break;
  case DATAFLOAT:
    tmp=malloc(sizeof(float));
    if (tmp==NULL) break;
    *((float *) tmp)=*((float *) data);
    break;
  case DATADOUBLE:
    tmp=malloc(+sizeof(double));
    if (tmp==NULL) break;
    *((double *) tmp)=*((double *) data);
    break;
  default:
    tmp=malloc(sizeof(char *));
    if (tmp==NULL) break;
    *((char **) tmp)=*((char **) data);
    break;
  }
  if (tmp==NULL) return NULL;

  s=DataMapMakeScalar(name,1,type,tmp);
  if (s==NULL) return NULL;

  if (ptr->scl==NULL) tmp=malloc(sizeof(struct DataMapScalar *));
  else tmp=realloc(ptr->scl,(ptr->snum+1)*sizeof(struct DataMapScalar *));
  if (tmp==NULL) {
    DataMapFreeScalar(s);
    return NULL;
  }
  ptr->scl=tmp;
  ptr->scl[ptr->snum]=s;
  ptr->snum++;
  return tmp;
}

struct DataMapArray* DataMapMakeArray(char *name,int mode,
                                      int type,int dim,
                                      int32 *rng,void *data) {
  
  struct DataMapArray *ptr;

  if (name==NULL) return NULL;
  if (dim==0) return NULL;

  ptr=malloc(sizeof(struct DataMapArray));
  if (ptr==NULL) return ptr;
  ptr->name=malloc(strlen(name)+1);
  if (ptr->name==NULL) {
    free(ptr);
    return NULL;
  }
  strcpy(ptr->name,name);
  ptr->mode=mode;
  ptr->type=type;
  ptr->dim=dim;
  ptr->rng=rng;
  ptr->data.vptr=data;
  return ptr;
}

void DataMapFreeArray(struct DataMapArray *ptr) {
  if (ptr==NULL) return; 
  if (ptr->name !=NULL) free(ptr->name);
  if ((ptr->mode & 0x01) && (ptr->rng !=NULL)) free(ptr->rng);
  if ((ptr->mode & 0x02) && (ptr->data.vptr !=NULL)) free(ptr->data.vptr);
  free(ptr);
  return;
}


int DataMapAddArray(struct DataMap *ptr,
                        char *name,int type,int dim,
                        int32 *rng,void *data) {
  struct DataMapArray *a;
  if (ptr==NULL) return -1;
  a=DataMapMakeArray(name,0,type,dim,rng,data);
  if (a==NULL) return -1;

  if (ptr->arr==NULL) ptr->arr=malloc(sizeof(struct DataMapArray *));
  else {
    struct DataMapArray **tmp;
    tmp=realloc(ptr->arr,(ptr->anum+1)*sizeof(struct DataMapArray *));
    if (tmp==NULL) {
      DataMapFreeArray(a);
      return -1;
    }
    ptr->arr=tmp;
  }
  ptr->arr[ptr->anum]=a;
  ptr->anum++;
  return 0;
}

void *DataMapStoreArray(struct DataMap *ptr,
                     char *name,int type,int dim,
		      int32 *rng,void *data) {

  struct DataMapArray *a;
  int n=1,x=0;
  void *tmp=NULL;
  int32 *rngbuf=NULL;
  if (ptr==NULL) return NULL;
  if (dim==0) return NULL;
  if (rng==NULL) return NULL;

  
  rngbuf=malloc(dim*sizeof(int32));
  if (rngbuf==NULL) return NULL;
 
  n=1;
  for (x=0;x<dim;x++) {
    n=rng[x]*n;
    rngbuf[x]=rng[x];
  }
  if (data==NULL) {
    switch (type) {
    case DATACHAR:
      tmp=malloc(sizeof(char)*n);
      break;
    case DATASHORT:
      tmp=malloc(sizeof(int16)*n);
      break;
    case DATAINT:
      tmp=malloc(sizeof(int32)*n);
      break;
    case DATAFLOAT:
      tmp=malloc(sizeof(float)*n);
      break;
    case DATADOUBLE:
      tmp=malloc(sizeof(double)*n);
      break;
    default:
      tmp=malloc(sizeof(char *)*n);
      break;
    }
    if (tmp==NULL) {
      if (rngbuf !=NULL) free(rngbuf);
      return NULL;
    }
  }

  if (data !=NULL)
    a=DataMapMakeArray(name,1,type,dim,rngbuf,data);
  else 
    a=DataMapMakeArray(name,3,type,dim,rngbuf,tmp);
  if (a==NULL) {
    free(rngbuf);
    if (tmp !=NULL) free(tmp);
    return NULL;
  }
  if (ptr->arr==NULL) ptr->arr=malloc(sizeof(struct DataMapArray *));
  else {
    tmp=realloc(ptr->arr,(ptr->anum+1)*sizeof(struct DataMapArray *));
    if (tmp==NULL) {
      DataMapFreeArray(a);
      return NULL;
    }
    ptr->arr=tmp;
  }
  ptr->arr[ptr->anum]=a;
  ptr->anum++;
  if (data !=NULL) return data;
  return a->data.vptr;
}

int DataMapSize(struct DataMap *ptr) {
  int c,x,m,n;
  char **tmp;
  struct DataMapScalar *s=NULL;
  struct DataMapArray *a=NULL;
  int sze=0;

  sze+=sizeof(int32)*4;
  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];
    n=0;
    while (s->name[n] !=0) n++;
    sze+=n+1+1;
    switch (s->type) {
    case DATACHAR:
      sze++;
      break;
    case DATASHORT:
      sze+=sizeof(int16);
      break;
    case DATAINT:
      sze+=sizeof(int32);
      break;
    case DATAFLOAT:
      sze+=sizeof(float);
      break;
    case DATADOUBLE:
      sze+=sizeof(double);;
      break;
    default:
      tmp=(char **) s->data.vptr;
      if (*tmp !=NULL) {
        n=0;
        while((*tmp)[n] !=0) n++;
        sze+=n+1;  
      }
      break;
    }
  }

  for (c=0;c<ptr->anum;c++) {
    a=ptr->arr[c];
    n=0;
    while (a->name[n] !=0) n++;
    sze+=n+1+1+4+4*a->dim;
    n=1;
    for (x=0;x<a->dim;x++) n=a->rng[x]*n;
    switch (a->type) {
    case DATACHAR:
      sze+=n;
      break;
    case DATASHORT:
      sze+=sizeof(int16)*n;
      break;
    case DATAINT:
      sze+=sizeof(int32)*n;
      break;
    case DATAFLOAT:
      sze+=sizeof(float)*n;
      break;
    case DATADOUBLE:
      sze+=sizeof(double)*n;
      break;
    default:
      tmp=(char **) a->data.vptr;
      for (x=0;x<n;x++) {
        if (tmp[x] !=NULL) {
          m=0;
          while( (tmp[x])[m] !=0) m++;
          sze+=m+1;       
	}
      }
      break;
   }    
  }
  return sze;
}




unsigned char *DataMapEncodeBuffer(struct DataMap *ptr,int *size) {
  int c,x,m,n;
  char **tmp;
  struct DataMapScalar *s=NULL;
  struct DataMapArray *a=NULL;
  unsigned char *buf=NULL;
  int off=0;
  int sze=0;

  sze=DataMapSize(ptr);

  if (size !=NULL) *size=sze;
  buf=malloc(sze);
  if (buf==NULL) return NULL;

  ConvertFromInt(DATACODE,buf+off);
  off+=sizeof(int32);
  ConvertFromInt(sze,buf+off);
  off+=sizeof(int32);
  ConvertFromInt(ptr->snum,buf+off);
  off+=sizeof(int32);
  ConvertFromInt(ptr->anum,buf+off);
  off+=sizeof(int32);
  
  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];
    n=0;
    while (s->name[n] !=0) n++;
    memcpy(buf+off,s->name,n+1);
    off+=n+1;
    buf[off]=s->type;
    off++;
    switch (s->type) {
    case DATACHAR:
      buf[off]=s->data.cptr[0];
      off++;
      break;
    case DATASHORT:
      ConvertFromShort(*(s->data.sptr),buf+off);
      off+=sizeof(int16);
      break;
    case DATAINT:
      ConvertFromInt(*(s->data.iptr),buf+off);
      off+=sizeof(int32);
      break;
    case DATAFLOAT:
      ConvertFromFloat(*(s->data.fptr),buf+off);
      off+=sizeof(float);
      break;
    case DATADOUBLE:
      ConvertFromDouble(*(s->data.dptr),buf+off);
      off+=sizeof(double);
      break;
    default:
      tmp=(char **) s->data.vptr;
      if (*tmp !=NULL) {
        n=0;
        while((*tmp)[n] !=0) n++;
        memcpy(buf+off,*tmp,n+1);
        off+=n+1;
      }
      break;
    }
  }
 
  for (c=0;c<ptr->anum;c++) {
    a=ptr->arr[c];
    n=0;
    while (a->name[n] !=0) n++;
    memcpy(buf+off,a->name,n+1);
    off+=n+1;
    buf[off]=a->type;
    off++;
    ConvertFromInt(a->dim,buf+off);
    off+=sizeof(int32);
    for (x=0;x<a->dim;x++)  {
      ConvertFromInt(a->rng[x],buf+off);
      off+=sizeof(int32);
    }
    n=1;
    for (x=0;x<a->dim;x++) n=a->rng[x]*n;
    switch (a->type) {
    case DATACHAR:
      memcpy(buf+off,a->data.cptr,n);
      off+=n;
      break;
    case DATASHORT:
      for (x=0;x<n;x++) {
        ConvertFromShort(a->data.sptr[x],buf+off);
        off+=sizeof(int16);
      }
      break;
    case DATAINT:
      for (x=0;x<n;x++) {
        ConvertFromInt(a->data.iptr[x],buf+off);
        off+=sizeof(int32);
      }
      break;
    case DATAFLOAT:
      for (x=0;x<n;x++) {
        ConvertFromFloat(a->data.fptr[x],buf+off);
        off+=sizeof(float);
      }
      break;
    case DATADOUBLE:
      for (x=0;x<n;x++) {
        ConvertFromDouble(a->data.dptr[x],buf+off);
        off+=sizeof(double);;
      }
      break;
    default:
      tmp=(char **) a->data.vptr;
      for (x=0;x<n;x++) {
        if (tmp[x] !=NULL) {
          m=0;
          while( (tmp[x])[m] !=0) m++;
          memcpy(buf+off,tmp[x],m+1);
          off+=m+1;
	}
      }
      break;
   }    
  }
  return buf;
}


int DataMapWrite(int fid,struct DataMap *ptr) {
  unsigned char *buf=NULL;
  int sze=0,st=0,cnt=0;
  buf=DataMapEncodeBuffer(ptr,&sze);
  if (buf==NULL) return -1;
  
  while (cnt<sze) {
    st=write(fid,buf+cnt,sze-cnt);
    if (st<=0) break;
    cnt+=st;
  } 
  if (cnt<sze) return st;
  free(buf);
  return sze;
}

int DataMapFwrite(FILE *fp,struct DataMap *ptr) {
  return DataMapWrite(fileno(fp),ptr);
}

void DataMapFree(struct DataMap *ptr) {
  int s,a;
  if (ptr==NULL) return;
  if (ptr->scl !=NULL) {  
    for (s=0;s<ptr->snum;s++) {
      DataMapFreeScalar(ptr->scl[s]);
    }
    free(ptr->scl);
  }
  if (ptr->arr !=NULL) {
    for (a=0;a<ptr->anum;a++) {
      DataMapFreeArray(ptr->arr[a]);
    }
    free(ptr->arr);
  }
  if (ptr->buf !=NULL) free(ptr->buf);
  free(ptr);
}

char *DataMapReadName(int fid) {
  char *ptr,*tmp;
  char t;
  int st=0;
  int blk=256;
  int stp=256;
  int sze=0;

  ptr=malloc(sizeof(char)*blk);
  while (((st=read(fid,&t,1))==1) && (t !=0)) {
    ptr[sze]=t;
    sze++;
    if (sze>=blk) {
      blk+=stp;
      tmp=realloc(ptr,blk);
      if (tmp==NULL) free(ptr);
      ptr=tmp;
    }
    if (ptr==NULL) return NULL;
  }
  if (st != 1) {
    free(ptr);
    return NULL;
  } 
  ptr[sze]=0;
  
  sze++;
  tmp=realloc(ptr,sze);
  if (tmp==NULL) free(ptr);
  ptr=tmp;
  return ptr;
}

char *DataMapReadString(int fid) {
  char *ptr,*tmp;
  char t;
  int st=0;
  int blk=256;
  int stp=256;
  int sze=0;

  ptr=malloc(sizeof(char)*blk);
  while (((st=read(fid,&t,1))==1) && (t !=0)) {
    ptr[sze]=t;
    sze++;
    if (sze>=blk) {
      blk+=stp;
      tmp=realloc(ptr,blk);
      if (tmp==NULL) free(ptr);
      ptr=tmp;
    }
    if (ptr==NULL) return NULL;
  }
  if (st != 1) {
    free(ptr);
    return NULL;
  } 
  ptr[sze]=0;
  
  sze++;
  tmp=realloc(ptr,sze);
  if (tmp==NULL) free(ptr);
  ptr=tmp;
  return ptr;
}

struct DataMap *DataMapDecodeBuffer(unsigned char *buf,int size) {
  int c,x,n,i;
  int32 sn,an;
  int32 code,sze;
  char *name;
  unsigned char *tmp;
  char type;
  unsigned int off=0;  

  struct DataMap *ptr;
  struct DataMapScalar *s;
  struct DataMapArray *a;
  
  ptr=DataMapMake();
  if (ptr==NULL) return NULL;
  ConvertToInt(buf+off,&code);
  off+=sizeof(int32);
  ConvertToInt(buf+off,&sze);
  off+=sizeof(int32);
  ConvertToInt(buf+off,&sn);
  off+=sizeof(int32);
  ConvertToInt(buf+off,&an);
  off+=sizeof(int32);
  if (sn>0) {
    ptr->snum=sn;
    ptr->scl=malloc(sizeof(struct DataMapScalar *)*sn); 
    if (ptr->scl==NULL) {
      DataMapFree(ptr);
      return NULL;
    }
    for (c=0;c<sn;c++) ptr->scl[c]=NULL;
  }

  if (an>0) {
    ptr->anum=an;
    ptr->arr=malloc(sizeof(struct DataMapArray *)*an);
    if (ptr->arr==NULL) {
      DataMapFree(ptr);
      return NULL;
    }
    for (c=0;c<an;c++) ptr->arr[c]=NULL;
  }

  for (c=0;c<sn;c++) {
    n=0;
    while ((buf[off+n] !=0) && (off+n<size)) n++;
    if (off+n>=size) break;
    name=malloc(n+1);
    if (name==NULL) break;
    memcpy(name,buf+off,n+1);
    off+=n+1;
    type=buf[off];
    off++;
    s=malloc(sizeof(struct DataMapScalar));
    if (s==NULL) {
      free(name);
      break;
    }

    s->name=name;
    s->mode=1;
    s->type=type;
    ptr->scl[c]=s;   

    switch (s->type) {
    case DATACHAR:
      s->data.vptr=malloc(sizeof(char));
      if (s->data.vptr==NULL) break;
      s->data.cptr[0]=buf[off];
      off++;
      break;
    case DATASHORT:
      s->data.vptr=malloc(sizeof(int16));
      if (s->data.vptr==NULL) break;
      ConvertToShort(buf+off,s->data.sptr);
      off+=sizeof(int16);
      break;
    case DATAINT:
      s->data.vptr=malloc(sizeof(int32));
      if (s->data.vptr==NULL) break;
      ConvertToInt(buf+off,s->data.iptr);
      off+=sizeof(int32); 
      break;
    case DATAFLOAT:
      s->data.vptr=malloc(sizeof(float));
      if (s->data.vptr==NULL) break;
      ConvertToFloat(buf+off,s->data.fptr);
      off+=sizeof(float);
      break;
    case DATADOUBLE:
      s->data.vptr=malloc(sizeof(double));
      if (s->data.vptr==NULL) break;
      ConvertToDouble(buf+off,s->data.dptr);
      off+=sizeof(double);
      break;
    default:
      n=0;
      while ((buf[off+n] !=0) && (off+n<size)) n++;
      if (off+n>=size) break;
      s->data.vptr=malloc(sizeof(char *));
      if (s->data.vptr==NULL) break;
      tmp=realloc(ptr->buf,ptr->sze+n+1);
      if (tmp==NULL) break;
      ptr->buf=tmp;
      memcpy(ptr->buf+ptr->sze,buf+off,n+1);
      off+=n+1;
      *(s->data.optr)=ptr->sze;
      ptr->sze+=n+1;
      break;
    }
  }

  if (c !=sn) {
    DataMapFree(ptr);
    return NULL;
  }
  for (c=0;c<an;c++) {
    n=0;
    while ((buf[off+n] !=0) && (off+n<size)) n++;
    if (off+n>=size) break;
    name=malloc(n+1);
    if (name==NULL) break;
    memcpy(name,buf+off,n+1);
    off+=n+1;
    type=buf[off];
    off++;
    a=malloc(sizeof(struct DataMapArray));
    if (a==NULL) {
      free(name);
      break;
    }
    a->name=name;
    a->mode=3;
    a->type=type;
    ptr->arr[c]=a;   
    ConvertToInt(buf+off,(int32 *) &(a->dim));
    off+=sizeof(int32);
    a->rng=malloc(a->dim*sizeof(int32));
    if (a->rng==NULL) break;
    for (x=0;x<a->dim;x++)  {
       ConvertToInt(buf+off,&a->rng[x]);
       off+=sizeof(int32);
    }
    if (x!=a->dim) break;
    n=1;
    for (x=0;x<a->dim;x++) n=a->rng[x]*n;
    switch (a->type) {
    case DATACHAR:
      a->data.vptr=malloc(sizeof(char)*n);
      if (a->data.vptr==NULL) break;
      memcpy(a->data.cptr,buf+off,sizeof(char)*n);
      off+=sizeof(char)*n;
      break;
    case DATASHORT:
      a->data.vptr=malloc(sizeof(int16)*n);
      if (a->data.vptr==NULL) break;
      for (x=0;x<n;x++) {
        ConvertToShort(buf+off,&a->data.sptr[x]);
        off+=sizeof(int16);
      }
      break;
    case DATAINT:
      a->data.vptr=malloc(sizeof(int32)*n);
      if (a->data.vptr==NULL) break;
      for (x=0;x<n;x++) {
        ConvertToInt(buf+off,&a->data.iptr[x]);
        off+=sizeof(int32);
      }
      break;
    case DATAFLOAT:
      a->data.vptr=malloc(sizeof(float)*n);
      if (a->data.vptr==NULL) break;
      for (x=0;x<n;x++) {
        ConvertToFloat(buf+off,&a->data.fptr[x]);
        off+=sizeof(float);
      }
      break;
    case DATADOUBLE:
      a->data.vptr=malloc(sizeof(double)*n);
      if (a->data.vptr==NULL) break;
      for (x=0;x<n;x++) {
	ConvertToDouble(buf+off,&a->data.dptr[x]);
	off+=sizeof(double);
      }
      break;
    default:
      a->data.vptr=malloc(sizeof(char *)*n);
      if (a->data.vptr==NULL) break;
          
      for (x=0;x<n;x++) {
         i=0;
          
         while ((buf[off+i] !=0) && (off+i<size)) i++;
         if (off+i>=size) break;
         tmp=realloc(ptr->buf,ptr->sze+i+1);
         if (tmp==NULL) break;
         ptr->buf=tmp;
         memcpy(ptr->buf+ptr->sze,buf+off,i+1); 
         a->data.optr[x]=ptr->sze;
         ptr->sze+=i+1;
         off+=i+1;
      }
      if (x !=n) break;
    }
  }

  if (c !=an) {
    DataMapFree(ptr);
    return NULL;
  }
  

  for (c=0;c<ptr->snum;c++) {
    if (ptr->scl[c]==NULL) continue;
    s=ptr->scl[c];
    if (s->type==DATASTRING) 
      *((char **) s->data.vptr)=(char *) (ptr->buf+*(s->data.optr));



  }
  
  for (c=0;c<ptr->anum;c++) {
    if (ptr->arr[c]==NULL) continue;
    a=ptr->arr[c];
    if (a->type==DATASTRING) {
      n=1;
      for (x=0;x<a->dim;x++) n=a->rng[x]*n;
      for (x=0;x<n;x++) 
	( (char **) a->data.vptr)[x]=(char *) (ptr->buf+a->data.optr[x]);
    }
  }
  return ptr;
}


struct DataMap *DataMapReadBlock(int fid,int *s) {
  unsigned char *buf=NULL;
  struct DataMap *ptr;
  int32 code,sze,*iptr;
  int size=0,cnt=0,num=0,st=0;   
  st=ConvertReadInt(fid,&code);
  if (st==-1) return NULL;
  st=ConvertReadInt(fid,&sze);
  if (st==-1) return NULL;
  if (sze==0) return NULL;
  size=sze;
  buf=malloc(size);
  if (buf==NULL) return NULL;
  iptr=(int32 *) buf;
  iptr[0]=code;
  iptr[1]=sze;
    
  cnt=0;
  num=size-2*sizeof(int32);
  while (cnt<num) {
    st=read(fid,buf+2*sizeof(int32)+cnt,num-cnt);
    if (st<=0) break;
    cnt+=st;
  }
  if (cnt<num) {
    free(buf);
    return NULL;
  }
  ptr=DataMapDecodeBuffer(buf,size);
  free(buf);
  if (s !=NULL) *s=size+2*sizeof(int32);
  return ptr;
}

struct DataMap *DataMapFreadBlock(FILE *fp,int *s) {
  return DataMapReadBlock(fileno(fp),s);
}

struct DataMap *DataMapRead(int fid) {
  return DataMapReadBlock(fid,NULL);
}

struct DataMap *DataMapFread(FILE *fp) {
  return DataMapRead(fileno(fp));
}


struct DataMap *DataMapReadBlockZ(gzFile file,int *s) {
  unsigned char *buf=NULL;
  struct DataMap *ptr;
  int32 code,sze,*iptr;
  int size=0,cnt=0,num=0,st=0;   
  st=ConvertReadIntZ(file,&code);
  if (st==-1) return NULL;
  st=ConvertReadIntZ(file,&sze);
  if (st==-1) return NULL;
  if (sze==0) return NULL;
  size=sze;
  buf=malloc(size);
  if (buf==NULL) return NULL;
  iptr=(int32 *) buf;
  iptr[0]=code;
  iptr[1]=sze;
    
  cnt=0;
  num=size-2*sizeof(int32);
  while (cnt<num) {
    st=gzread(file,buf+2*sizeof(int32)+cnt,num-cnt);
    if (st<=0) break;
    cnt+=st;
  }
  if (cnt<num) {
    free(buf);
    return NULL;
  }

  ptr=DataMapDecodeBuffer(buf,size);
  free(buf);
  
  if (s !=NULL) *s=size+2*sizeof(int32);
  return ptr;
}

struct DataMap *DataMapReadZ(gzFile file) {
  return DataMapReadBlockZ(file,NULL);
}


int DataMapWriteZ(gzFile file,struct DataMap *ptr) {
  unsigned char *buf=NULL;
  int sze=0,st=0,cnt=0;
  buf=DataMapEncodeBuffer(ptr,&sze);
  if (buf==NULL) return -1;
  
  while (cnt<sze) {
    st=gzwrite(file,buf+cnt,sze-cnt);
    if (st<=0) break;
    cnt+=st;
  } 
  if (cnt<sze) return st;
  free(buf);
  return sze;
}

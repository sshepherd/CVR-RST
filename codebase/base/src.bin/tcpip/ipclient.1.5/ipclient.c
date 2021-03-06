/* ipclient.c
   ==========
   Author: R.J.Barnes
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "option.h"
#include "connex.h"
#include "errstr.h"
#include "hlpstr.h"



struct OptionData opt;

int main(int argc,char *argv[]) {
  int arg;
  int sock;
  int remote_port=0;
  char *host;
  int flag,status,size=0;
  unsigned char help=0;
  unsigned char option=0;

  struct timeval tmout;

  int timeout=120;
  int tflg=0;
  int cflg=0;

  unsigned char *buffer=NULL;
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"connect",'x',&cflg);
  OptionAdd(&opt,"tmout",'i',&timeout);
  OptionAdd(&opt,"test",'x',&tflg);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (argc-arg<2) { 
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  host=argv[argc-2];
  remote_port=atoi(argv[argc-1]);

  tmout.tv_sec=timeout;
  tmout.tv_usec=0;

  fprintf(stderr,"opening connection %s %d\n",host,remote_port); 
  sock=ConnexOpen(host,remote_port,&tmout); 
  if (sock<0) {
    if (tflg) fprintf(stdout,"0\n");
    else fprintf(stderr,"Could not connect to host.\n");
    exit(255);
  }

  if (cflg) {
    fprintf(stdout,"0\n");
    exit(0);
  }

  do {

   status=ConnexRead(1,&sock,&buffer,&size,&flag,&tmout);

   if (status==-1) break;
   if (flag !=-1) {
     if (tflg) break;
     else { 
       fprintf(stderr,"Message size:%d\n",size);
       fprintf(stderr,"Message content:%s\n",buffer);
     }
   }           
  } while(tflg==0);
  if (tflg) {
    fprintf(stdout,"%d\n",size);
    if (status==-1) return 255;
    else if (status==0) return 0;
  }  else fprintf(stderr,"Connection failed.\n");
  return 1;

}
   

 











/* socket.h
   ========
   Author: R.J.Barnes
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/



#define CLIENT_MAX 16
#define BUF_SIZE 10*1024

struct client {
  int sock;
  int in_sze;
  int out_sze;
  char host[256];
  char in_buf[BUF_SIZE]; /* data from client */
  char out_buf[BUF_SIZE]; /* data to client */
};


int pollsock(int sock,struct timeval *tv,fd_set *fdset);
int processsocket(int sock,int inpipe);
int createsocket(int *port);

/* normalscan.c
   ============
   Author: R.J.Barnes
   with mods from SGS

 Basic Parameters:
   Scan from start_beam to end_beam in sequence.

 Details:
   - 2-min scan is historical, setting the -fast option is standard for 1-min

 v1.7 - SGS
   - added fixed frequency capability (basically over-ride whatever the
      clear frequency search returns.) CAREFUL you don't transmit on a
      forbidden frequency...

   - allow for beam scanning in either direction; simply specify sb > eb for
      a backward directed scan, otherwise scans in the forward direction.

*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "dmap.h"
#include "limit.h"
#include "radar.h"
#include "rprm.h"
#include "iq.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "fitacf.h"
#include "errlog.h"
#include "freq.h"
#include "tcpipmsg.h"
#include "rmsg.h"
#include "rmsgsnd.h"
#include "radarshell.h"
#include "build.h"
#include "global.h"
#include "reopen.h"
#include "setup.h"
#include "sync.h"
#include "site.h"
#include "sitebuild.h"
#include "siteglobal.h"
#include "rosmsg.h"
#include "tsg.h"

char *ststr=NULL;
char *dfststr="tst";

void *tmpbuf;
size_t tmpsze;

char progid[80]={"normalscan"};
char progname[256];

int arg=0;
struct OptionData opt;

int baseport=44100;

struct TCPIPMsgHost errlog={"127.0.0.1",44100,-1};
struct TCPIPMsgHost shell={"127.0.0.1",44101,-1};

int tnum=4;      
struct TCPIPMsgHost task[4]={
  {"127.0.0.1",1,-1}, /* iqwrite */
  {"127.0.0.1",2,-1}, /* rawacfwrite */
  {"127.0.0.1",3,-1}, /* fitacfwrite */
  {"127.0.0.1",4,-1}  /* rtserver */
};

void usage(void);
int main(int argc,char *argv[]) {

  /*
   * commentary here: SGS
   * It seems that the mode should be decoupled from the pulse sequence.
   * The pulse table and lag table should be externally defined with some
   * way of determining the time it takes for a given sequence.
   */

  int ptab[8] = {0,14,22,24,27,31,42,43};
  int lags[LAG_SIZE][2] = {
    { 0, 0},		/*  0 */
    {42,43},		/*  1 */
    {22,24},		/*  2 */
    {24,27},		/*  3 */
    {27,31},		/*  4 */
    {22,27},		/*  5 */

    {24,31},		/*  7 */
    {14,22},		/*  8 */
    {22,31},		/*  9 */
    {14,24},		/* 10 */
    {31,42},		/* 11 */
    {31,43},		/* 12 */
    {14,27},		/* 13 */
    { 0,14},		/* 14 */
    {27,42},		/* 15 */
    {27,43},		/* 16 */
    {14,31},		/* 17 */
    {24,42},		/* 18 */
    {24,43},		/* 19 */
    {22,42},		/* 20 */
    {22,43},		/* 21 */
    { 0,22},		/* 22 */

    { 0,24},		/* 24 */

    {43,43}};		/* alternate lag-0  */

  char logtxt[1024];

  int exitpoll=0;
  int scannowait=0;
 
  int scnsc=120;			/* total scan period in seconds */
  int scnus=0;
  int skip;
  int cnt=0;

  unsigned char fast=0;
  unsigned char discretion=0;
  int fixfrq=0;

	int n;
  int status=0;

  int beams=0;
  int total_scan_usecs=0;
  int total_integration_usecs=0;
	int debug=0;

	int bufsc=0;    /* a buffer at the end of scan; historically this has   */
	int bufus=0;    /*   been set to 3.0s to account for what???            */
	unsigned char hlp=0;

	if (debug) {
		printf("Size of int %ld\n",sizeof(int));
		printf("Size of long %ld\n",sizeof(long));
		printf("Size of long long %ld\n",sizeof(long long));
		printf("Size of struct TRTimes %ld\n",sizeof(struct TRTimes));
		printf("Size of struct SeqPRM %ld\n",sizeof(struct SeqPRM));
		printf("Size of struct RosData %ld\n",sizeof(struct RosData));
		printf("Size of struct DataPRM %ld\n",sizeof(struct DataPRM));
		printf("Size of Struct ControlPRM  %ld\n",sizeof(struct ControlPRM));
		printf("Size of Struct RadarPRM  %ld\n",sizeof(struct RadarPRM));
		printf("Size of Struct ROSMsg  %ld\n",sizeof(struct ROSMsg));
		printf("Size of Struct CLRFreq  %ld\n",sizeof(struct CLRFreqPRM));
		printf("Size of Struct TSGprm  %ld\n",sizeof(struct TSGprm));
		printf("Size of Struct SiteSettings  %ld\n",sizeof(struct SiteSettings));
	}

  cp     = 150;		/* CPID */
  intsc  = 7;			/* integration period; recomputed below ... */
  intus  = 0;
  mppul  = 8;			/* number of pulses; tied to array above ... */
  mplgs  = 23;		/* same here for the number of lags */
  mpinc  = 1500;	/* multi-pulse increment [us] */
  nrang  = 100;		/* the number of ranges gets set in SiteXXXStart() */
  rsep   = 45;		/* same for the range separation */
  txpl   = 300;		/* pulse length [us]; gets redefined below... */

  dmpinc = nmpinc = mpinc;	/* set day and night to the same,
																but could change */

  /* ========= PROCESS COMMAND LINE ARGUMENTS ============= */

  OptionAdd(&opt, "di",     'x', &discretion);
  OptionAdd(&opt, "frang",  'i', &frang);
  OptionAdd(&opt, "rsep",   'i', &rsep);
  OptionAdd(&opt, "dt",     'i', &day);
  OptionAdd(&opt, "nt",     'i', &night);
  OptionAdd(&opt, "df",     'i', &dfrq);
  OptionAdd(&opt, "nf",     'i', &nfrq);
  OptionAdd(&opt, "xcf",    'x', &xcnt);
  OptionAdd(&opt, "ep",     'i', &errlog.port);
  OptionAdd(&opt, "sp",     'i', &shell.port); 
  OptionAdd(&opt, "bp",     'i', &baseport); 
  OptionAdd(&opt, "stid",   't', &ststr); 
  OptionAdd(&opt, "fast",   'x', &fast);
  OptionAdd(&opt, "nowait", 'x', &scannowait);
  OptionAdd(&opt, "sb",     'i', &sbm);
  OptionAdd(&opt, "eb",     'i', &ebm);
	OptionAdd(&opt, "fixfrq", 'i', &fixfrq);   /* fix the transmit frequency */
	OptionAdd(&opt, "-help",  'x', &hlp);      /* just dump some parameters */

	/* process the commandline; need this for setting errlog port */
  arg=OptionProcess(1,argc,argv,&opt,NULL);  
	backward = (sbm > ebm) ? 1 : 0;   /* allow for non-standard scan dir. */

  if (hlp) {
    usage();

    printf("  start beam: %2d\n", sbm);
    printf("  end   beam: %2d\n", ebm);
    if (backward) printf("  backward\n");
		else printf("  forward\n");

    return (-1);
  }

  if (ststr==NULL) ststr=dfststr;

  if ((errlog.sock=TCPIPMsgOpen(errlog.host,errlog.port))==-1) {    
    fprintf(stderr,"Error connecting to error log.\n");
  }
  if ((shell.sock=TCPIPMsgOpen(shell.host,shell.port))==-1) {    
    fprintf(stderr,"Error connecting to shell.\n");
  }

  for (n=0;n<tnum;n++) task[n].port+=baseport;

  OpsStart(ststr);

  status=SiteBuild(stid);
  if (status==-1) {
    fprintf(stderr,"Could not identify station.\n");
    exit(1);
  }

  SiteStart();

	/* reprocess the commandline since some things are reset by SiteStart */
  arg=OptionProcess(1,argc,argv,&opt,NULL);  
	backward = (sbm > ebm) ? 1 : 0;   /* this almost certainly got reset */

  printf("Station ID: %s  %d\n",ststr,stid);
  strncpy(combf,progid,80);   
 
  OpsSetupCommand(argc,argv);
  OpsSetupShell();
   
  RadarShellParse(&rstable,"sbm l ebm l dfrq l nfrq l dfrang l nfrang l"
					" dmpinc l nmpinc l frqrng l xcnt l", &sbm,&ebm, &dfrq,&nfrq,
					&dfrang,&nfrang, &dmpinc,&nmpinc, &frqrng,&xcnt);      
 
  status=SiteSetupRadar();
  printf("Initial Setup Complete: Station ID: %s  %d\n",ststr,stid);
  
  if (status !=0) {
    ErrLog(errlog.sock,progname,"Error locating hardware.");
    exit (1);
  }

  beams=abs(ebm-sbm)+1;
  if (fast) {
    cp    = 151;
    scnsc = 60;
    scnus = 0;
  } else {
    scnsc = 120;
    scnus = 0;
  }
	/* recomputing the integration period for each beam */
	/* Note that I have added a buffer here to account for things at the end
     of the scan. Traditionally this has been set to 3s, but I cannot find
     any justification of the need for it. -SGS */
  total_scan_usecs = scnsc*1e6 + scnus - (bufsc*1e6 + bufus);
/*  total_scan_usecs = (scnsc-3)*1E6+scnus; */
  total_integration_usecs = total_scan_usecs/beams;
  intsc = total_integration_usecs/1e6;
  intus = total_integration_usecs -(intsc*1e6);

  if (discretion) cp = -cp;

  txpl=(rsep*20)/3;

  if (fast) sprintf(progname,"normalscan (fast)");
  else sprintf(progname,"normalscan");

  OpsLogStart(errlog.sock,progname,argc,argv);  
  OpsSetupTask(tnum,task,errlog.sock,progname);

  for (n=0;n<tnum;n++) {
    RMsgSndReset(task[n].sock);
    RMsgSndOpen(task[n].sock,strlen((char *)command),command);     
  }

  printf("Preparing OpsFitACFStart Station ID: %s  %d\n",ststr,stid);
  OpsFitACFStart();
  
  printf("Preparing SiteTimeSeq Station ID: %s  %d\n",ststr,stid);
  tsgid=SiteTimeSeq(ptab);

  printf("Entering Scan loop Station ID: %s  %d\n",ststr,stid);
  do {

    printf("Entering Site Start Scan Station ID: %s  %d\n",ststr,stid);
    if (SiteStartScan() !=0) continue;
    
    if (OpsReOpen(2,0,0) !=0) {
      ErrLog(errlog.sock,progname,"Opening new files.");
      for (n=0;n<tnum;n++) {
        RMsgSndClose(task[n].sock);
        RMsgSndOpen(task[n].sock,strlen( (char *) command),command);     
      }
    }

    scan = 1;	/* scan flagg */
    
    ErrLog(errlog.sock,progname,"Starting scan.");
    if (xcnt>0) {
      cnt++;
      if (cnt==xcnt) {
        xcf=1;
        cnt=0;
      } else xcf=0;
    } else xcf=0;

    skip=OpsFindSkip(scnsc,scnus);
    
    if (backward) {
      bmnum=sbm-skip;
      if (bmnum<ebm) bmnum=sbm;
    } else {
      bmnum=sbm+skip;
      if (bmnum>ebm) bmnum=sbm;
    }

    do {

      TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
      
      if (OpsDayNight()==1) {
        stfrq=dfrq;
        mpinc=dmpinc;
        frang=dfrang;
      } else {
        stfrq=nfrq;
        mpinc=nmpinc;
        frang=nfrang;
      }        

      sprintf(logtxt,"Integrating beam:%d intt:%ds.%dus (%d:%d:%d:%d)",
											bmnum,intsc,intus,hr,mt,sc,us);
      ErrLog(errlog.sock,progname,logtxt);

      ErrLog(errlog.sock,progname,"Starting Integration.");
			printf("Entering Site Start Intt Station ID: %s  %d\n",ststr,stid);
      SiteStartIntt(intsc,intus);

			/* clear frequency search business */
      ErrLog(errlog.sock,progname,"Doing clear frequency search."); 
      sprintf(logtxt, "FRQ: %d %d", stfrq, frqrng);
      ErrLog(errlog.sock,progname, logtxt);
      tfreq=SiteFCLR(stfrq,stfrq+frqrng);

      if ( (fixfrq > 8000) && (fixfrq < 25000) ) tfreq = fixfrq;
 
      sprintf(logtxt,"Transmitting on: %d (Noise=%g)",tfreq,noise);
      ErrLog(errlog.sock,progname,logtxt);
    
      nave=SiteIntegrate(lags);   
      if (nave<0) {
        sprintf(logtxt,"Integration error:%d",nave);
        ErrLog(errlog.sock,progname,logtxt); 
        continue;
      }
      sprintf(logtxt,"Number of sequences: %d",nave);
      ErrLog(errlog.sock,progname,logtxt);

      OpsBuildPrm(prm,ptab,lags);
      OpsBuildIQ(iq,&badtr);
      OpsBuildRaw(raw);

      FitACF(prm,raw,fblk,fit);

      msg.num=0;
      msg.tsize=0;

      tmpbuf=RadarParmFlatten(prm,&tmpsze);
      RMsgSndAdd(&msg,tmpsze,tmpbuf, PRM_TYPE,0); 

      tmpbuf=IQFlatten(iq,prm->nave,&tmpsze);
      RMsgSndAdd(&msg,tmpsze,tmpbuf,IQ_TYPE,0);

      RMsgSndAdd(&msg,sizeof(unsigned int)*2*iq->tbadtr,
                 (unsigned char *) badtr,BADTR_TYPE,0);
		 
			RMsgSndAdd(&msg,strlen(sharedmemory)+1,(unsigned char *)sharedmemory,
									IQS_TYPE,0);

      tmpbuf=RawFlatten(raw,prm->nrang,prm->mplgs,&tmpsze);
      RMsgSndAdd(&msg,tmpsze,tmpbuf,RAW_TYPE,0); 
 
      tmpbuf=FitFlatten(fit,prm->nrang,&tmpsze);
      RMsgSndAdd(&msg,tmpsze,tmpbuf,FIT_TYPE,0); 

			RMsgSndAdd(&msg,strlen(progname)+1,(unsigned char *)progname,
									NME_TYPE,0);   

      for (n=0;n<tnum;n++) RMsgSndSend(task[n].sock,&msg); 

      for (n=0;n<msg.num;n++) {
        if (msg.data[n].type==PRM_TYPE) free(msg.ptr[n]);
        if (msg.data[n].type==IQ_TYPE) free(msg.ptr[n]);
        if (msg.data[n].type==RAW_TYPE) free(msg.ptr[n]);
        if (msg.data[n].type==FIT_TYPE) free(msg.ptr[n]); 
      }          

      RadarShell(shell.sock,&rstable);

      if (exitpoll !=0) break;
      scan=0;
      if (bmnum==ebm) break;
      if (backward) bmnum--;
      else bmnum++;

    } while (1);

    ErrLog(errlog.sock,progname,"Waiting for scan boundary."); 
    if ((exitpoll==0) && (scannowait==0)) SiteEndScan(scnsc,scnus);
  } while (exitpoll==0);
  
  for (n=0;n<tnum;n++) RMsgSndClose(task[n].sock);

  ErrLog(errlog.sock,progname,"Ending program.");

  SiteExit(0);

  return 0;   
} 
 
void usage(void)
{
	printf("\nnormalscan [command-line options]\n\n");
	printf("command-line options:\n");
	printf("    -di     : indicates running during discretionary time\n");
	printf(" -frang int : delay to first range (km) [180]\n");
	printf("  -rsep int : range separation (km) [45]\n");
	printf("    -dt int : hour when day freq. is used [site.c]\n");
	printf("    -nt int : hour when night freq. is used [site.c]\n");
	printf("    -df int : daytime frequency (kHz) [site.c]\n");
	printf("    -nf int : nighttime frequency (kHz) [site.c]\n");
	printf("   -xcf     : set for computing XCFs [global.c]\n");
	printf("    -sb int : starting beam [site.c]\n");
	printf("    -eb int : ending beam [site.c]\n");
	printf("    -ep int : error log port (must be set here for dual radars)\n");
	printf("    -sp int : shell port (must be set here for dual radars)\n");
	printf("    -bp int : base port (must be set here for dual radars)\n");
	printf("  -stid char: radar string (must be set here for dual radars)\n");
	printf("-fixfrq int : transmit on fixed frequency (kHz)\n");
	printf("-nowait     : do not wait at end of scan boundary.\n");
	printf(" --help     : print this message and quit.\n");
	printf("\n");
}


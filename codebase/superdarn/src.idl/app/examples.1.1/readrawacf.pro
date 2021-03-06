; readrawacf.pro
; ==============
; Author: R.J.Barnes
; 
; $License$
;
; A Simply demonstration program for reading rawacf files


pro readrawacf

; Open the raw index file and load it into the inx structure 

  openr,inp,'test.rawinx',/GET_LUN,/STDIO,/SWAP_IF_BIG_ENDIAN
  s=RawLoadInx(inp,inx)
  free_lun,inp
 
; Open the raw file for read only 
 
  inp=RawOpen('test.rawacf',/read)
 

;  Search for a specific time in the file

  s=RawSeek(inp,2002,12,19,0,30,10,inx,atme=atme)
 
  while RawRead(inp,prm,raw) ne -1 do begin
     print, prm.time.hr,prm.time.mt,prm.time.sc
     plot,raw.pwr0[0:prm.nrang-1]
     stop
  endwhile
  free_lun,inp

end



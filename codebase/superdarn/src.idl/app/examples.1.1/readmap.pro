; readmap.pro
; ==============
; Author: R.J.Barnes
; 
; $License$
;
; A Simply demonstration program for reading map files


pro readmap

; Open the raw index file and load it into the inx structure 

  openr,inp,'test.inx',/GET_LUN,/STDIO,/SWAP_IF_BIG_ENDIAN
  s=OldCnvMapLoadInx(inp,inx)
  free_lun,inp
 
; Open the map file for read only 
 
  inp=OldCnvMapOpen('test.map',/read)
 

;  Search for a specific time in the file

  s=OldCnvMapSeek(inp,2002,12,19,0,30,10,inx,atme=atme)
 
  while OldCnvMapRead(inp,prm,stvec,gvec,mvec,coef,bvec) ne -1 do begin
     print, prm.stme.hr,prm.stme.mt,prm.stme.sc
     stop
  endwhile
  free_lun,inp

end



; mlt.pro
; =======
; Author: R.J.Barnes
;
; $License$
;
; Public Functions:
; -----------------
;
; MLTConvert
;
; Private Functions:
; ------------------
;
; MLTDefaultMLT
; MLTAst
;
; ---------------------------------------------------------------

; Common blocks for the MLT algorithm

common MLTCom,sol_dec_old,told,mslon1,mslon2

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       MLTDefault
;
; PURPOSE:
;       Initialized the common block variables for the MLT routines
;       
;
; CALLING SEQUENCE:
;       MLTDefault
;
;       This procedure initializes the common block variables for
;       the MLT routine. It is automatically called the first
;       time the MLT routines are used.
;
;     
;
;-----------------------------------------------------------------
;


pro MLTDefault
  common MLTCom,sol_dec_old,told,mslon1,mslon2
  sol_dec_old=0
  told=1e12
  mslon1=0
  mslon2=0
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       MLTAst1
;
; PURPOSE:
;       Calculate Magnetic Local Time
;       
;
; CALLING SEQUENCE:
;       MLTAst1,t0,solar_dec,mlon,mlt,mslon
;
;
;       This procedure calculates magnetic local time.
;
;     
;-----------------------------------------------------------------
;



pro MLTAst1,t0,solar_dec,mlon,mlt,mslon
  common MLTCom,sol_dec_old,told,mslon1,mslon2
  if (n_elements(sol_dec_old) eq 0) then MLTDefault
  if ((abs(solar_dec-sol_dec_old) gt 0.1) or (sol_dec_old eq 0)) then told=1e12
  if (abs(mslon2-mslon1) gt 10) then told=1e12;
    
  if ((t0 ge told) and (t0 lt (told+600))) then $
    mslon=mslon1+(t0-told)*(mslon2-mslon1)/600.0 $
  else begin
    told=t0
    sol_dec_old=solar_dec

    slon1 = (43200.0-t0)*15.0/3600.0
    slon2 = (43200.0-t0-600)*15.0/3600.0

    height = 450
    AACGMConvertGeoCoord,solar_dec,slon1,height,mslat1,mslon1,4,err
    AACGMConvertGeoCoord,solar_dec,slon2,height,mslat2,mslon2,4,err
    mslon=mslon1
  end
  

  mlt = (mlon - mslon) /15.0 + 12.0
  if (mlt ge 24) then mlt=mlt-24;
  if (mlt lt 0) then mlt=mlt+24;  

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       MLTAst
;
; PURPOSE:
;       Calculate Magnetic Local Time
;       
;
; CALLING SEQUENCE:
;       MLTAst,yr,mo,dy,hr,mt,sc,mlon,mlt,mslon
;
;
;       This function calculates magnetic local time.
;
;     
;-----------------------------------------------------------------
;

pro MLTAst,yr,mo,dy,hr,mt,sc,mlon,mlt,mslon

   dd=AstAlg_dday(dy-1,hr,mt,sc)
   jd=AstAlg_jde(yr,mo,dd)
   eqt=AstAlg_equation_of_time(jd);
   dec=AstAlg_solar_declination(jd);

   ut=(hr*3600.0D) + (mt*60.0D)+sc;
   at = ut + (eqt*60.0D);
   
   MLTAst1,at, dec, mlon, mlt,mslon
 
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       MLTConvertYMDHMS
;
; PURPOSE:
;       Calculate Magnetic Local Time
;       
;
; CALLING SEQUENCE:
;       mlt=MLTConvertYMDHMS(yr,mo,dy,hr,mt,sc,mlon)
;
;
;       This function calculates magnetic local time.
;
;     
;-----------------------------------------------------------------
;

  
function MLTConvertYMDHMS,yr,mo,dy,hr,mt,sc,mlon
   mslon=0.0D
   mlt=0.0D
   MLTAst,yr,mo,dy,hr,mt,sc,mlon,mlt,mslon
   return, mlt
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       MLTConvertYMDHMS
;
; PURPOSE:
;       Calculate Magnetic Local Time
;       
;
; CALLING SEQUENCE:
;       mlt=MLTConvertYrsec(yr,yrsec,mlon)
;
;
;       This function calculates magnetic local time.
;
;     
;-----------------------------------------------------------------
;

  
function MLTConvertYrsec,yr,yrsec,mlon
   mslon=0.0D
   mlt=0.0D
   s=TimeYrsecToYMDHMS(yr,mo,dy,hr,mt,sc,yrsec)
   MLTAst,yr,mo,dy,hr,mt,sc,mlon,mlt,mslon
   return, mlt
end

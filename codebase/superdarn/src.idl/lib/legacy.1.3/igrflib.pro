; igrflib.pro
; ===========
; Author: K.Baker & R.J.Barnes
; 
; $License$
;
;
; PublicFunctions
; ---------------
; magcmp
; igrfcall
;
; ---------------------------------------------------------------




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;           magcmp
;
; PURPOSE:  
;
;           use IGRF magnetic field models to return magnetic field
;	    components.
;
; CALLING SEQUENCE:
;
;	B = magcmp(lat,lon,elev,date)
;	     where date is given in decimal years
;	     lat, lon are the geographic latitude and longitude in degrees
;	     elev is the elevation.
;
;-----------------------------------------------------------------------
;

function magcmp,lat,lon,elev,date,ifrst
  bvec=IGRFModelMagCmp(date,elev,lat,lon)
  return,bvec
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME: 
;
;           igrfcall
;
; PURPOSE:  
;
;           use IGRF magnetic field models to return magnetic field
;	    components.
;
; CALLING SEQUENCE:
;
;	B = igrfcall(date,glat,glong,alt)
;	     where date is given in decimal years
;	     glat, glong are the geographic latitude and longitude in degrees
;	     alt is the altitude in km.
;
;-----------------------------------------------------------------------
;

function igrfcall,date,glat,glong,alt
  if (n_params() NE 4) then begin
    print,"Invalid number of parameters in call to IGRFCALL"
    print,"USE: B=igrfcall(date,glat,glong,altitude)"
    return,[0,0,0]
  endif
	
  temp_date = date

  if (temp_date LT 1900) then temp_date = temp_date + 1900   
  igrf_year = double(temp_date)
  flat = double(glat)
  flong = double(glong)
  height = double(alt)
 
  b=IGRFModelCall(igrf_year,flat,flong,height)

  return,B
end

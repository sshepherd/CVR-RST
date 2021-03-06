; aacgmtest.pro
; =============
; Author: R.J.Barnes
; 
; $License$
;
; A Simply demonstration program for testing the AACGM library


pro aacgmtest


; Open a set of coefficients and load them.
; 
; NOTE: This step can be left out as the AACGM routines will default
; to the year 2000 coefficients

;  openr,unit,'2000.dat',/get_lun,/stdio
;  c=AACGMLoadCoef(unit)
;  free_lun,unit

  glat=85.5
  glon=70.5
  hgt=300.0

  mlat=0.0
  mlon=0.0
  r=0.0

  s=AACGMConvert(glat,glon,hgt,mlat,mlon,r)
  print, 'Geomagnetic Coordinates=',mlat,mlon

  s=AACGMConvert(mlat,mlon,hgt,glat,glon,r,/geo)
  print, 'Inverse Geographic Coordinates=',glat,glon

  yrsec=TimeYMDHMSToYrSec(2002,08,30,10,30,0)

  mlt=AACGMConvertMLT(2002,yrsec,0.0)

  print, 'MLT=',mlt

end



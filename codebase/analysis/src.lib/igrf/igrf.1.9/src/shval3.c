/* shval3.c
   ========
   Author: R.J.Barnes
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/



#include <stdio.h>
#include <math.h>
#include "rmath.h"

int shval3(int igdgc,double flat,double flon, double elev, 
            double erad, double a2, double b2, int nmax, double *gh,
            int iext,double *ext,double *x,double *y,double *z) { 

 
  
    double  d1;

    double clat, slat;
    int i, j, k, l, n, m;
    double p[119], q[119], r, ratio, aa, bb, cc, cd, dd, cl[14], 
	    fm, fn=0, sd, sl[14], rr=0;
    int npq;

    --ext;
    --gh;
  
    r = elev;
    slat = sin(flat * .01745329);
    if ( (90.0 - flat) < 0.001) {
	aa = 89.999;

/* 300 ft from N. pole */

    } else if (flat + 90.0 < 0.001) {
	aa = -89.999;
/* 300 ft from S. pole */
    } else {
	aa = flat;
    }
    clat = cos(aa * .01745329);
    sl[0] = sin(flon * .01745329);
    cl[0] = cos(flon * .01745329);
    *x =  0.0;
    *y =  0.0;
    *z =  0.0;
    sd =  0.0;
    cd =  1.0;
    n = 0;
    l = 1;
    m = 1;
    npq = (nmax * (nmax + 3)) / 2;
    if (igdgc == 1) {
	aa = a2 * clat * clat;
	bb = b2 * slat * slat;
	cc = aa + bb;
	dd = sqrt(cc);
	r = sqrt(elev * (elev + dd * 2.0) + (a2 * aa + b2 * bb) / 
		cc);
	cd = (elev + dd) / r;
	sd = (a2 - b2) / dd * slat * clat / r;
	aa = slat;
	slat = slat * cd - clat * sd;
	clat = clat * cd + aa * sd;
    }
    ratio = erad / r;
    aa = sqrt(3.0);
    p[0] = slat * 2.0;
    p[1] = clat * 2.0;
    p[2] = slat * 4.5 * slat - 1.5;
    p[3] = aa *  3.0 * clat * slat;
    q[0] = -clat;
    q[1] = slat;
    q[2] = clat * -3.0 * slat;
    q[3] = aa * (slat * slat - clat * clat);
    
    for (k = 1; k <= npq; ++k) {
	if (n < m) {
	    m = 0;
	    ++n;
	 
	    rr = pow(ratio, n+2);
	    fn =  n;
	}
	fm =  m;
	if (k >= 5) {
	    if (m == n) {
		aa = sqrt(1.0 - (0.5 / fm));
		j = k - n - 1;
		p[k - 1] = (1.0 / fm + 1.0) * aa * clat * p[j - 1]
			;
		q[k - 1] = aa * (clat * q[j - 1] + slat / fm * p[j - 1]);
		sl[m - 1] = sl[m - 2] * cl[0] + cl[m - 2] * sl[0];
		cl[m - 1] = cl[m - 2] * cl[0] - sl[m - 2] * sl[0];
	    } else {
		aa = sqrt(fn * fn - fm * fm);

		d1 = fn - 1.0;
		bb = sqrt(d1 * d1 - fm * fm) / aa;
		cc = (fn * 2.0 - 1.0) / aa;
		i = k - n;
		j = k - 2*n  + 1;
		p[k - 1] = (fn + 1.0) * (cc * slat / fn * p[i - 1] - 
			bb / (fn - 1.0) * p[j - 1]);
		q[k - 1] = cc * (slat * q[i - 1] - clat / fn * p[i - 1]) 
			- bb * q[j - 1];
	    }
	}
	aa = rr * gh[l];
	if (m == 0) {
	    *x += aa * q[k - 1];
	    *z -= aa * p[k - 1];
	    ++l;
	} else {
	    bb = rr * gh[l + 1];
	    cc = aa * cl[m - 1] + bb * sl[m - 1];
	    *x += cc * q[k - 1];
	    *z -= cc * p[k - 1];
	    if (clat > 0.0) {
		*y += (aa * sl[m - 1] - bb * cl[m - 1]) * fm * p[k - 1] / ((
			fn + 1.0) * clat);
	    } else {
		*y += (aa * sl[m - 1] - bb * cl[m - 1]) * q[k - 1] * slat;
	    }
	    l += 2;
	}
	++m;
    }
    if (iext != 0) {
	aa = ext[2] * cl[0] + ext[3] * sl[0];
	*x = *x - ext[1] * clat + aa * slat;
	*y = *y + ext[2] * sl[0] - ext[3] * cl[0];
	*z = *z + ext[1] * slat + aa * clat;
    }
    aa = *x;
    *x = *x * cd + *z * sd;
    *z = *z * cd - aa * sd;
    
    return 0;
} 


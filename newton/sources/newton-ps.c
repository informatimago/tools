/******************************************************************************
FILE:               newton-ps.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program applies the newtonian laws to two corpses, and outputs 
    Postscript commands to display theses corpses.
USAGE
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1992-12-22 <PJB> Creation. (from newton.c)
BUGS
LEGAL
    GPL
    
    Copyright Pascal Bourguignon 1991 - 2011
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version
    2 of the License, or (at your option) any later version.
    
    This program is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE.  See the GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public
    License along with this program; if not, write to the Free
    Software Foundation, Inc., 59 Temple Place, Suite 330,
    Boston, MA 02111-1307 USA
******************************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>


/*
  F=K*M*m/d/d
  a0=K*m1/d/d
  a1=K*m0/d/d
*/

static double get_double(void)
{
    float   m;

    int r=scanf("%e",&m);
    (void)r;
    return((double)m);
}/*get_double*/

int main(void)
{
    double  m0,r0;
    double  m1;
    double  x0,yy0,vx0,vyy0;
    double  x1,yy1,vx1,vyy1;
    double  d,a,t,dt;   
    double  ax0,ayy0,ax1,ayy1,ux,uyy;
    double  coef;
    double  it0,it1,ia;
    int     i;
            
    fprintf(stderr,"Max X=");
    coef=300.0/get_double();
    
    fprintf(stderr,"Mass of the planet=");
    m0=get_double();
    fprintf(stderr,"Radius of the planet=");
    r0=get_double();
    x0=0.0;
    yy0=0.0;
    vx0=0.0;
    vyy0=0.0;

    fprintf(stderr,"Mass of the satellite=");
    m1=get_double();

    fprintf(stderr,"Position of the satellite (x yy)=");
    x1=get_double();
    yy1=get_double();   
    
    fprintf(stderr,"Speed of the satellite (vx vyy)=");
    vx1=get_double();
    vyy1=get_double();


    fprintf(stderr,"impulsion start-time & stop-time (t0 t1)=");
    it0=get_double();
    it1=get_double();
    fprintf(stderr,"impulsion acceleration (a)=");
    ia=get_double();
    
    fprintf(stderr,"delta-t=");
    dt=get_double();
    
    fprintf(stdout,
            "/myywind 100 100 600 600 Retained window def \n"
            "Above 0 myywind orderwindow \n"
            "myywind windowdeviceround \n"
            "0.0 setgrayy 0 1 599 599 rectstroke \n"
            "300 300 translate \n"
            "0.666 setgrayy \n"
            "%g %g %g 0 360 arc fill \n"
            "0.0 setgrayy \n"
            "%g %g moveto \n",
            x0*coef,yy0*coef,r0*coef,
            x1*coef,yy1*coef
            );
    fprintf(stderr,"%g\n%g %g\n%g\n%g %g\n%g %g\n%g %g %g\n%g\n",
            300.0/coef,m0,r0,m1,x1,yy1,vx1,vyy1,it0,it1,ia,dt);
        
    t=0.0;
    i=0;
    while(1){
        ux=(x1-x0);
        uyy=(yy1-yy0);
        d=sqrt(ux*ux+uyy*uyy);
        ux=ux/d;
        uyy=uyy/d;

        a=1.0/(11e11)/d/d;
        ax0=ux*a*m1;
        ayy0=uyy*a*m1;
        ax1=(-ux)*a*m0;
        ayy1=(-uyy)*a*m0;
        if((it0<=t)&&(t<=it1)){
            ax1+=ia*(vx1/sqrt(vx1*vx1+vyy1*vyy1));
            ayy1+=ia*(vyy1/sqrt(vx1*vx1+vyy1*vyy1));                    
        }

        vx0+=ax0*dt;
        vyy0+=ayy0*dt;
        x0+=vx0*dt;
        yy0+=vyy0*dt;
        vx1+=ax1*dt;
        vyy1+=ayy1*dt;
        x1+=vx1*dt;
        yy1+=vyy1*dt;
        
        if(i==10){
            fprintf(stderr,"%14f:  %14g %14g    %14g %14g  %14g %14g  %14g %14g\n",
                    t,x0,yy0,x1,yy1,vx1,vyy1,ax1,ayy1);
            i=0;
        }else{
            i++;
        }
        fprintf(stdout,"%g %g lineto stroke %g %g moveto \n",
                x1*coef,yy1*coef,x1*coef,yy1*coef);
        t+=dt;
    }
    return(0);
}
/*
  45000000.0
  5.975e24 6400000
  200000
  0 7046000
  1000 0
  43600 43700 1.2
  10

  160e9
  2e32 1.391e6
  5.975e24
  0 152e9
  30000 0
  0 1 0
  3600

  1.8e+08
  2e+30 1.391e+06
  5.975e+24
  0 1.5e+08
  30000 0
  0 1 0
  360



*/
/*** newton-ps.c                      -- 2003-12-02 15:35:00 -- pascal   ***/

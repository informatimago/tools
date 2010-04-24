/******************************************************************************
FILE:               newton.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program applies the newtonian laws to two corpses.
USAGE
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1991-05-12 <PJB> Creation.
BUGS
LEGAL
    GPL
    
    Copyright Pascal Bourguignon 1991 - 1991
    
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
#include <math.h>

/*
    F=K*M*m/d/d
    a0=K*m1/d/d
    a1=K*m0/d/d
*/

static double get_double(void)
{
    float m;

  scanf("%e",&m);
  return((double)m);
}/*get_double*/

int main(void)
{
    double m0;
    double m1;
    double x0,yy0,vx0,vyy0;
    double x1,yy1,vx1,vyy1;
    double d,a,t,dt; 
    double ax0,ayy0,ax1,ayy1,ux,uy;

  printf("Mass of the planet=");
  m0=get_double();
  x0=0.0;
  yy0=0.0;
  vx0=0.0;
  vyy0=0.0;

  printf("Mass of the satellite=");
  m1=get_double();

  printf("Position of the satellite (x y)=");
  x1=get_double();
  yy1=get_double(); 
  
  printf("Speed of the satellite (vx vy)=");
  vx1=get_double();
  vyy1=get_double();

  printf("delta-t=");
  dt=get_double();
  t=0.0;
  while(1){
    ux=(x1-x0);
    uy=(yy1-yy0);
    d=sqrt(ux*ux+uy*uy);
    ux=ux/d;
    uy=uy/d;

    a=1.0/(11e11)/d/d;
    ax0=ux*a*m1;
    ayy0=uy*a*m1;
    ax1=(-ux)*a*m0;
    ayy1=(-uy)*a*m0;
 
    vx0+=ax0*dt;
    vyy0+=ayy0*dt;
    x0+=vx0*dt;
    yy0+=vyy0*dt;
    vx1+=ax1*dt;
    vyy1+=ayy1*dt;
    x1+=vx1*dt;
    yy1+=vyy1*dt;

    printf("%3f:  %g %g    %g %g  %g %g  %g %g\n",t,x0,yy0,x1,yy1,vx1,vyy1,ax1,ayy1);
    t+=dt;
  }
  return(0);
}

/*** newton.c                         -- 2003-12-02 15:34:45 -- pascal   ***/

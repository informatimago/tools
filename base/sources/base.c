/*****************************************************************************
FILE:               base.c
LANGUAGE:           c
SYSTEM:             POSIX
USER-INTERFACE:     NONE
DESCRIPTION
    
    Converts a number passed as first argument (read with atoi)
    into the base passed as second argument.
    
AUTHORS
    <PJB> Pascal Bourguignon <pjb@informatimago.com>
MODIFICATIONS
    2003-11-23 <PJB> Added this header comment.
BUGS
LEGAL
    GPL
    
    Copyright Pascal Bourguignon 2003 - 2003
    
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
*****************************************************************************/
#include <string.h>    
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


    static void int_to_base(int n,int /*card8*/ base,char* tampon)
        /*
            PRE: tampon pointe vers une zone suffisament longue pour
            ecrire le nombre n en base `base'.
        */  
    {
        static const char digits[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        int k,l;
        char t;
        assert((1<base)&&(base<=(int)strlen(digits)));
        l=0;

        if(n<0){
            tampon[l++]='-';
            n=-n;
    
            /* Dans le cas habituel, les entiers sont stockes en complement a 2, */
            /* et alors, lorsque n=-2^(P-1), (P=32 ici), -n==n ! */
            /* Note: Il y a probablement un meilleur moyen de traiter ce cas. */
            if(n<0){
                /* -2147483648 exprime dans ces differentes bases: */
                static const char* special_values[]={
                    "INVALID BASE 0",
                    "INVALID BASE 1",
                    /* base  2 */"-10000000000000000000000000000000",
                    /* base  3 */"-12112122212110202102",
                    /* base  4 */"-2000000000000000",
                    /* base  5 */"-13344223434043",
                    /* base  6 */"-553032005532",
                    /* base  7 */"-104134211162",
                    /* base  8 */"-20000000000",
                    /* base  9 */"-5478773672",
                    /* base 10 */"-2147483648",
                    /* base 11 */"-A02220282",
                    /* base 12 */"-4BB2308A8",
                    /* base 13 */"-282BA4AAB",
                    /* base 14 */"-1652CA932",
                    /* base 15 */"-C87E66B8",
                    /* base 16 */"-80000000",
                    /* base 17 */"-53G7F549",
                    /* base 18 */"-3928G3H2",
                    /* base 19 */"-27C57H33",
                    /* base 20 */"-1DB1F928",
                    /* base 21 */"-140H2D92",
                    /* base 22 */"-IKF5BF2",
                    /* base 23 */"-EBELF96",
                    /* base 24 */"-B5GGE58",
                    /* base 25 */"-8JMDNKN",
                    /* base 26 */"-6OJ8IOO",
                    /* base 27 */"-5EHNCKB",
                    /* base 28 */"-4CLM98G",
                    /* base 29 */"-3HK7988",
                    /* base 30 */"-2SB6CS8",
                    /* base 31 */"-2D09UC2",
                    /* base 32 */"-1VVVVVW",
                    /* base 33 */"-1LSQTL2",
                    /* base 34 */"-1D8XQRQ",
                    /* base 35 */"-15V22UN",
                    /* base 36 */"-ZIK0ZK"
                };
                strcpy(tampon,special_values[base]);
                return;
            }

        }
        k=l;

        while(n>0){
            tampon[l++]=digits[n%base];
            n/=base;
        }
        tampon[l--]='\0';

        while(k<l){
            t=tampon[k];
            tampon[k]=tampon[l];
            tampon[l]=t;
            k++;
            l--;
        }
    }/*int_to_base;*/



int main(int argc,const char**  argv)
{
  char tampon[34];
  int  n;
  int  base;

  if(argc!=3){
    fprintf(stderr,"Usage:\n"
        "\t%s number base\n",argv[0]);
    exit(1);
  }

  n=atoi(argv[1]);
  base=atoi(argv[2]);

  int_to_base(n,base,tampon);

  fprintf(stdout,"%s\n",tampon);
  exit(0);
  return(0);
}/*main.*/



/*** base.c                           -- 2003-12-02 10:47:41 -- pascal   ***/

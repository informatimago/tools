/******************************************************************************
FILE:               primes.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
USAGE
    primes to <integer> | factorize <integer> 
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1993-12-12 <PJB> Creation.
BUGS
LEGAL
    GPL
    
    Copyright Pascal Bourguignon 1993 - 1993
    
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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>

typedef long int    integer;
static const long int max_integer=0x7fffffff;

    static integer          debug=0;
    

#define round4k(s)  ((((s)+4095)/4096)*4096)
#define bits_per_char       (8)
#define bits_per_integer    (sizeof(integer)*bits_per_char)

    static integer* ComputePrimesTo(integer n)
    {
            integer         bitsmax;
            integer*        bits;       /* set of odd numbers.*/
            integer*        curbits;
            integer         i;
            integer         curprime;
            integer         primecount;
            integer         bit;
            integer         halfn;
            integer*        primes;
            integer*        curnum;


        n+=2;
        bitsmax=(n/2+bits_per_integer-1)/bits_per_integer;
        n=2*bitsmax*bits_per_integer;
        halfn=n/2;
        bits=malloc(round4k(bitsmax*sizeof(integer)));
            /* SEE: seems to be a bug for large allocation sizes ?*/
        
        /* set the bitset to full bits;*/
        curbits=bits;
        i=bitsmax-1;
        while(i-->=0){
            *curbits++=(~0);
        }
        /* reset the last bit as watchdog for searching next prime.*/
        bits[bitsmax-1]=~(1<<(bits_per_integer-1));
        /*bits[(halfn-1)/bits_per_integer]&=~(1<<((n-1)%bits_per_integer));*/
        
        primecount=2;
        curprime=3;
        while(curprime<n){
            bit=(curprime-3)/2+curprime;
            while(bit<halfn){
if(debug){printf("exclude bit %ld\n",bit);}
                bits[bit/bits_per_integer]&=~(1<<(bit%bits_per_integer));
                bit+=curprime;
            }
            bit=(curprime-3)/2+1;
            while(!(bits[bit/bits_per_integer]&(1<<(bit%bits_per_integer)))){
                bit++;
            }
            curprime=2*bit+3;
if(debug){printf("found prime %ld\n",curprime);}
            primecount++;
        }
if(debug){printf("primecount %ld\n",primecount);}
        
        primes=malloc(round4k((primecount+1)*sizeof(integer)));
if(debug){printf("primes=%p\n",(void*)primes);}
if(debug){printf("after last primes=%p\n",(void*)(primes+(primecount+1)));}
        curnum=primes;
        *curnum++=2;
        curprime=3;
        bit=0;
        while(curprime<n){
            bit++;
            while(!(bits[bit/bits_per_integer]&(1<<(bit%bits_per_integer)))){
if(debug){printf("skipped bit %ld\n",bit);}
                bit++;
            }
            curprime=2*bit+3;
if(debug){printf("found prime %ld\n",curprime);}
            *curnum++=curprime;
        }
        *curnum++=0;
if(debug){printf("curnum=%p\n",(void*)curnum);}
        free(bits);
        return(primes);
    }/*ComputePrimesTo;*/
    


    static integer* ComputeOnePrimeFrom(integer from)
    {
        integer         bitsmax;
        integer*        bits;       /* set of odd numbers.*/
        integer*        curbits;
        integer         i;
        integer         curprime;
        integer         primecount;
        integer         bit;
        integer         halfn;
        integer*        primes;
        integer         n=max_integer;

        if(from<n/4){
            integer four_from=from*4;
            while(n>four_from){
                n/=2;
            }
        }

        bitsmax=(n/2+bits_per_integer-1)/bits_per_integer;
        n=2*bitsmax*bits_per_integer;
        halfn=n/2;
        bits=malloc(round4k(bitsmax*sizeof(integer)));
            /* SEE: seems to be a bug for large allocation sizes ?*/
        
        /* set the bitset to full bits;*/
        curbits=bits;
        i=bitsmax-1;
        while(i-->=0){
            *curbits++=(~0);
        }
        /* reset the last bit as watchdog for searching next prime.*/
        bits[bitsmax-1]=~(1<<(bits_per_integer-1));
        /*bits[(halfn-1)/bits_per_integer]&=~(1<<((n-1)%bits_per_integer));*/
        
        primecount=2;
        curprime=3;
        while(curprime<=from){
            bit=(curprime-3)/2+curprime;
            while(bit<halfn){
if(debug){printf("exclude bit %ld\n",bit);}
                bits[bit/bits_per_integer]&=~(1<<(bit%bits_per_integer));
                bit+=curprime;
            }
            bit=(curprime-3)/2+1;
            while(!(bits[bit/bits_per_integer]&(1<<(bit%bits_per_integer)))){
                bit++;
            }
            curprime=2*bit+3;
if(debug){printf("found prime %ld\n",curprime);}
            primecount++;
        }
if(debug){printf("primecount %ld\n",primecount);}
        
        primes=malloc(2*sizeof(integer));
if(debug){printf("next prime=%ld\n",curprime);}
        primes[0]=curprime;
        primes[1]=0;
        free(bits);
        return(primes);
    }/*ComputeOnePrimeFrom;*/
    
    
    
    static integer* Factorize(integer n,integer* primes)
    {
            integer         primecount;
            integer*        exponents;
            integer         expo;
            integer         prime;
            integer*        curexponent;
            
        primecount=0;
        while(primes[primecount++]>0){
        }
        
        exponents=malloc(round4k(sizeof(integer)*(primecount+1)));
        curexponent=exponents+1;
        
        prime=2;
        while((primecount-->0)&&(n>1)){
            expo=0;
            while(n%prime==0){
                expo++;
                n/=prime;
            }
            *curexponent++=expo;
            prime=*primes++;
        }
        *curexponent++=0;
        *exponents=n;
        return(exponents);
    }/*Factorize;*/


    static void PrintNumbers(integer* list)
    {
        while(*list>0){
            printf("%ld\n",*list++);
        }
    }/*PrintNumbers;*/
    
    
    static void PrintFactorization(integer* exponent,integer* primes)
    {
            integer     remainder=*exponent++;
            
        while(*exponent>=0){
            if(*exponent>0){
                printf("%12ld ^ %ld *\n",*primes,*exponent);
            }
            primes++;
            exponent++;
        }
        printf("%12c   %ld\n",' ',remainder);
    }/*PrintFactorization;*/
    
    
    static void usage(const char* pname)
    {
        fprintf(stderr,"%s usage :\n"
            "%s --next-from | next-from  n  # prints one prime greater than n.\n"
            "%s --to        | to         n  # prints all primes less than n.\n"
            "%s --factorize | factorize  n  # prints the factorization of n.\n",
            pname,pname,pname,pname);
    }/*usage;*/


int main(int argc,char** argv)
{
        integer     n;
        integer*    primelist;
        integer*    exponentlist;
    
    debug=(0==strcmp(argv[0],"debug"));
    if(argc!=3){
        usage(basename(argv[0]));
        return(1);
    }
    n=atoi(argv[2]);
    if(n<2){
        fprintf(stderr,"%s: the integer argument must be greater than 2.\n",
            argv[0]);
        return(2);
    }
    if((strcmp(argv[1],"to")||(strcmp(argv[1],"--to")==0))==0){
        primelist=ComputePrimesTo(n);
        PrintNumbers(primelist);
        free(primelist);
    }else if((strcmp(argv[1],"next-from")==0)||(strcmp(argv[1],"--next-from")==0)){
        primelist=ComputeOnePrimeFrom(n);
        PrintNumbers(primelist);
        free(primelist);
    }else if((strcmp(argv[1],"factorize")==0)||(strcmp(argv[1],"--factorize")==0)){
        primelist=ComputePrimesTo(n/2);
        exponentlist=Factorize(n,primelist);
        PrintFactorization(exponentlist,primelist);
        free(primelist);
        free(exponentlist);
    }else{
        usage(basename(argv[0]));
        return(1);
    }
    return(0);
}/*main.*/


/*** primes.c                         --                     --          ***/

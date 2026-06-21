/******************************************************************************
FILE:               lmax.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This filter return a status depending on the length of the input lines:
        0   all input lines are less than the limit passed as argument,
        1   there is an input line greater than the limit, and some other
            smaller.
        2   all the input lines are greater thant the limit.
USAGE
    lmax_ < _input_ && echo 'all smaller' || echo 'some greater'
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1995/09/14 <PJB> Creation.
BUGS
    lines from input file may be no longer than 4*BUFSIZ.
LEGAL
    Copyright Pascal J. Bourguignon 1995 - 2011
    All rights reserved.
    This program or any part of it may not be included in any commercial 
    product without the author written permission. It may be used freely for 
    any non-commercial purpose, provided that this header is always included.
******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static void usage(char* name)
{
    printf("%s usage:\n\t%s  limit\n",name,name);
}/*usage;*/

#define linesize    (4*BUFSIZ)

static int lmax(FILE* file,int limit)
{
    char            buffer[linesize];
    int             seen_short=0;
    int             seen_long=0;
    size_t          len;

    /* Read until EOF.  fgets() returns NULL at EOF *and* on error; the old
       code fed that NULL to check_error() which perror()+exit(1)'d on every
       normal EOF.  Drive the loop on the fgets() return and only treat a real
       stream error (ferror) as failure. */
    while(fgets(buffer,linesize,file)!=NULL){
        len=strlen(buffer);
        if(len<=(size_t)limit){
            seen_short=1;
        }else{
            seen_long=1;
        }
    }
    if(ferror(file)){
        perror("got an error");
        exit(1);
    }
    if(seen_short&&seen_long){
        return(1);
    }else if(seen_long){
        return(2);
    }else{
        return(0);
    }
}/*lmax*/
    
    
int main(int argc,char** argv)
{
    int             i;
    int             limit=0;
    int             (*test)(FILE*,int);
        
    i=1;
    while(i<argc){
        if(limit<=0){
            sscanf(argv[i],"%d",&limit);
        }else{
            usage(argv[0]);
            return(127);
        }
        i++;
    }
    if(limit<=0){
        usage(argv[0]);
        return(126);
    }

    test=lmax;  /* for when we allow several tests (lmax,lmin,lzero,...) */

    return(test(stdin,limit));
}/*main*/




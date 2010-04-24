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
    Copyright Pascal J. Bourguignon 1995 - 1995
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
            int             result;
            int             len;
            
        result=0;
        while(!feof(file)){
            fgets(buffer,linesize,file);
            len=strlen(buffer);
            if(len<=limit){
                while((!feof(file))&&(len<=limit)){
                    fgets(buffer,linesize,file);
                    len=strlen(buffer);
                }
                if(feof(file)){
                    return(0);
                }else{
                    while(!feof(file)){
                        fgets(buffer,linesize,file);
                    }
                    return(1);
                }
            }else{
                while((!feof(file))&&(len>limit)){
                    fgets(buffer,linesize,file);
                    len=strlen(buffer);
                }
                if(feof(file)){
                    return(2);
                }else{
                    while(!feof(file)){
                        fgets(buffer,linesize,file);
                    }
                    return(1);
                }
            }
        }
        return(result);
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




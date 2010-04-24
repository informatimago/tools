/*****************************************************************************
FILE:               random.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program compute and print one random number.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1992/07/18 <PJB> Creation.
    1993/08/16 <PJB> Added modulo parameter.
    1993/09/08 <PJB> Used BcRandom instead of random.
LEGAL
    Copyright Pascal J. Bourguignon 1992 - 1993
    All rights reserved.
    This program may not be included in any commercial product without the 
    author written permission. It may be used freely for any non-commercial 
    purpose, provided that this header is always included.
******************************************************************************/
#include <BcRandom.h>
#include <BcImplementation.h>

    static void Usage(char* pname)
    {
        fprintf(stderr,"Usage:\n\t%s [ <module> ]\n",pname);
    }/*Usage*/
    
int main(int argc,char** argv)
{
    unsigned int n;
        
    switch(argc){
    case 1:
        printf("%lu\n",BcRandom_fromfile());
        break;
    case 2:
        if((1==sscanf(argv[1],"%u",&n))&&(n>0)){
            printf("%lu\n",BcRandom_fromfile()%n);
        }else{
            Usage(argv[0]);
            return(1);
        }
        break;
    default:
        Usage(argv[0]);
        return(1);
    }
    return(0);
}/*main*/


/*** random.c                         --                     --          ***/

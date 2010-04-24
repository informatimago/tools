/******************************************************************************
FILE:               binskip.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This filter copies stdin to stdout, skipping some number of bytes specified
    as argument 1.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1993/09/24 <PJB>    Creation.
    1993/10/03 <PJB>    For performance  it uses now a bigger buffer
                        than that provided by getchar/putchar.
    Revision 1.1  2003/12/04 03:46:02  pjbpjb
    Cleaned-up.
    Fix.

    Revision 1.1  2001/04/30 01:58:08  pascal
    Initial entry into CVS.
    
    Revision 1.1  2001/04/27 06:45:10  pascal
    Added to CVS.
    
    Revision 1.3  1994/07/13  12:11:43  pbo
    Transformed C++ comments into C comments.

    Revision 1.2  1994/02/15  05:21:07  pascal
    Added RCS keywords.

BUGS
    - Should allow skipping bytes at the end and in the middle of the file. 
LEGAL
    Copyright Pascal J. Bourguignon 1993 - 1993
    All rights reserved.
    This program or any part of it may not be included in any commercial 
    product without the author written permission. It may be used freely for 
    any non-commercial purpose, provided that this header is always included.
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <BcTypes.h>

    
    static INT32 copy(FILE* input,FILE* output)
    {
            char            buffer[16*1024];
            char*           p;
            size_t          rsize;
            size_t          wsize;
#define buffersize  (sizeof(buffer)/sizeof(char))
        errno=0;
        rsize=fread(buffer,sizeof(char),buffersize,input);
        while(rsize>0){
            wsize=0;
            p=buffer;
            do{
                p+=(wsize/sizeof(char));
                rsize-=wsize;
                wsize=fwrite(buffer,sizeof(char),rsize,output);
            }while(wsize<rsize);
            rsize=fread(buffer,sizeof(char),buffersize,input);
        }
        return(errno);
    }/*copy.*/
    
    
int main(int argc,char** argv)
{
        char        c;
        INT32       count;
        
    if(argc!=2){
        fprintf(stderr,"Usage:\n\t%s <skipcount>\n",argv[0]);
        return(1);
    }
    count=atoi(argv[1]);
    while(count>0){
        c=getchar();
        count--;
    }
    return(copy(stdin,stdout));
}/*main.*/



/*** binskip.c                        --                     --          ***/

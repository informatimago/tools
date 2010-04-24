/******************************************************************************
FILE:               whatisit.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program tries to determine wheither the files given in arguments 
    are binary, pure ASCII, Macintosh extended ASCII, or NeXT extended ASCII.
USAGE
    whatisit <file>...
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1993/09/10 <PJB> Creation.
    1993/09/26 <PJB> Updated.
    1993/10/21 <PJB> Rename fload to loadfile.
    Revision 1.2  2004/06/24 18:19:37  pjbpjb
    Updated. Added a concatenate function to paste back split strings for
    ISO-C limitations.

    Revision 1.1  2003/12/04 03:46:03  pjbpjb
    Cleaned-up.
    Fix.

    Revision 1.2  2003/11/22 22:30:53  pascal
    Added options to ascii.
    Corrected some compilation problems.
    
    Revision 1.1  2001/04/30 01:58:08  pascal
    Initial entry into CVS.
    
    Revision 1.1  2001/04/27 06:45:11  pascal
    Added to CVS.
    
    Revision 1.4  95/09/08  19:51:42  pascal
    no comment.
    
    Revision 1.3  1994/07/13  12:11:43  pbo
    Transformed C++ comments into C comments.

    Revision 1.2  1994/02/15  05:21:34  pascal
    Added RCS keywords.

LEGAL
    Copyright Pascal J. Bourguignon 1993 - 1993
    All rights reserved.
    This program or any part of it may not be included in any commercial 
    product without the author written permission. It may be used freely for 
    any non-commercial purpose, provided that this header is always included.
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined(linux)||defined(AIX)
#include <unistd.h>
#else
#include <libc.h>
#endif
#include <BcTypes.h>

    
    typedef enum {
        eBinary,eAscii,eNext,eMacintosh
    }           KindT;
    static const char* KindLabel[]={"Binary","Ascii","Next","Macintosh"};
    static unsigned char MacintoshSet[]="ÛÝÜæ·Þòî úºª'¦°« »  ¼ä";
    static unsigned char AmbiguousSet[]="×Õ";
    

    static INT32 position(/*CHAR*/int c,CHAR* string)
    {
        INT32       i=0;
        while((string[i]!=(CHAR)0)&&(string[i]!=c)){
            i++;
        }
        if(string[i]!=(CHAR)0){
            return(i);
        }else{
            return(-1);
        }
    }/*position;*/
    

    static INT32        binaryCount=0;
    static INT32        asciiCount=0;
    static INT32        nextCount=0;
    static INT32        macintoshCount=0;


    static KindT WhatIsIt(CHAR* buffer,INT32 size)
    {
/*
            INT32       binaryCount=0;
            INT32       asciiCount=0;
            INT32       nextCount=0;
            INT32       macintoshCount=0;
*/
            INT32       i=0;

        binaryCount=0;
        asciiCount=0;
        nextCount=0;
        macintoshCount=0;
        
        while(i<size){
            if((buffer[i]<' ')
            &&(buffer[i]!=(CHAR)9)
            &&(buffer[i]!=(CHAR)10)
            &&(buffer[i]!=(CHAR)13)){
                binaryCount++;
            }else if(buffer[i]<(CHAR)128){
                asciiCount++;
            }else if(position(buffer[i],MacintoshSet)>=0){
                macintoshCount++;
            }else if(position(buffer[i],AmbiguousSet)<0){
                nextCount++;
            }
            i++;
        }
/* 
printf("%8ld %8ld %8ld %8ld %10ld\n",
binaryCount,asciiCount,macintoshCount,nextCount,size);
*/
        if((binaryCount>(asciiCount+nextCount+macintoshCount)/10)
        ||(nextCount+macintoshCount>asciiCount/2)){
            return(eBinary);
        }else if(macintoshCount>nextCount){
            return(eMacintosh);
        }else if(nextCount==0){
            return(eAscii);
        }else{
            return(eNext);
        }
    }/*WhatIsIt;*/


    static CHAR* loadfile(FILE* file,INT32* size)
        /*
            POST:       size=-1 <= cannot seek to EOF;
                        size>=0 & return=NULL <= cannot seek or read the file;
                        size>=0 && return!=NULL <= all is ok. 
        */
    {
        CHAR*               buffer;
        
        (*size)=-1;
        if(fseek(file,0,SEEK_END)<0){
            return(NULL);
        }else{
            (*size)=ftell(file);
            if(fseek(file,0,SEEK_SET)<0){
                return(NULL);
            }else{
                buffer=(CHAR*)malloc((unsigned)(*size));
                if(fread(buffer,sizeof(CHAR),(unsigned)(*size),file)
                   !=(size_t)(*size)){
                    free(buffer);
                    return(NULL);
                }else{
                    return(buffer);
                }
            }
        }
    }/*loadfile;*/
    
    
int main(int argc,char** argv)
{
        INT32           i;
        FILE*           file;
        CHAR*           buffer;
        INT32           size;
        INT32           k;
        INT32           l;
        int             maxlen=0;   
        int             printCount=0;
        
    if(argc<=1){
        fprintf(stderr,"%s usage: %s file... # display what type file is.\n",
                argv[0],argv[0]);
        return(1);
    }
    
    i=1;
    while(i<argc){
        if(strcmp(argv[i],"-v")==0){
            printCount=1;
        }else{
            l=strlen(argv[i]);
            if(l>maxlen){
                maxlen=l;
            }
        }
        i++;
    }
    
    i=1;
    while(i<argc){
        if(strcmp(argv[i],"-v")!=0){
            file=fopen(argv[i],"r");
            if(file==NULL){
                fprintf(stdout,"%-*s : CANNOT OPEN.\n",maxlen,argv[i]);
            }else{
                buffer=loadfile(file,&size);
                fclose(file);
                if(buffer==NULL){
                    fprintf(stdout,
                            "%-*s : CANNOT READ FILE OR ALLOCATE BUFFER.\n",
                            maxlen,argv[i]);
                }else{
                    k=WhatIsIt(buffer,size);
                    free(buffer);
                    if(printCount){
                        fprintf(stdout,
                                "%-*s : %-12s %8ld %8ld %8ld %8ld %10ld\n",
                                maxlen,argv[i],KindLabel[k],
                                binaryCount,asciiCount,macintoshCount,
                                nextCount,size);
                    }else{
                        fprintf(stdout,
                                "%-*s : %s\n",
                                maxlen,argv[i],KindLabel[k]);
                    }
                }
            }
        }
        i++;
    }
    return(0);
}/*main;*/

    

/*** whatisit.c                       --                     --          ***/

/******************************************************************************
FILE:               hexbin.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This is a filter converting a stream of ascii hexadecimal data into
    a stream of binary data. The input stream may contain any ASCII character
    less or equal to space which are skipped, and hexadecimal characters 
    ([0-9A-Fa-f]*) which are taken two by two to make an output byte.
USAGE
    hexbin < hexfile > binfile
AUTHOR
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1993/07/24 <PJB> Creation.
    Revision 1.1  2003/12/04 03:46:03  pjbpjb
    Cleaned-up.
    Fix.

    Revision 1.1  2001/04/30 01:58:08  pascal
    Initial entry into CVS.
    
    Revision 1.1  2001/04/27 06:45:10  pascal
    Added to CVS.
    
    Revision 1.4  95/09/08  18:32:04  pascal
    Corrected a small bug making use only half of the buffer.
    
    Revision 1.3  94/07/13  12:11:43  pascal
    Transformed C++ comments into C comments.
    
    Revision 1.2  1994/02/15  05:21:15  pascal
    Added RCS keywords.

LEGAL
    Copyright Pascal J. Bourguignon 1993 - 2011
    All rights reserved.
    This program or any part of it may not be included in any commercial 
    product without the author written permission. It may be used freely for 
    any non-commercial purpose, provided that this header is always included.
******************************************************************************/
#include <stdio.h>
#include <BcTypes.h>

    static INT32 hexdigit(INT32 c)
    {
        if(('0'<=c)&&(c<='9')){
            return(c-'0'+0);
        }else if(('A'<=c)&&(c<='F')){
            return(c-'A'+10);
        }else if(('a'<=c)&&(c<='f')){
            return(c-'a'+10);
        }else{
            return(-1);
        }
    }/*hexdigit;*/
    

    static INT32 loadhexwords(FILE* input,CARD8* buffer,INT32 count)
    {
            INT32       i;
            INT32       c1,c2;
            INT32       c;
        i=0;
        while(i<count){
            c=fgetc(input);
            while((0<=c)&&(c<=' ')){
                c=fgetc(input);
            }
            c1=hexdigit(c);
            
            c=fgetc(input);
            while((0<=c)&&(c<=' ')){
                c=fgetc(input);
            }
            c2=hexdigit(c);
            
            if((c1>=0)&&(c2>=0)){
                buffer[i]=(CARD8)((c1<<4)|c2);
                i++;
            }else{
                return(i);
            }
        }
        return(i);
    }/*loadhexwords;*/
    
    
int main(int argc,char** argv)
{
        CARD8       buffer[16*1024];
        size_t      rsize;
        size_t      wsize;
        INT32       isize;
    
    isize=0;
    rsize=(size_t)loadhexwords(stdin,buffer,sizeof(buffer)/sizeof(CARD8));
    while(rsize>0){
        isize+=(INT32)rsize;
        wsize=fwrite(buffer,1,(unsigned)rsize,stdout);
        if(wsize!=rsize){
            fprintf(stderr,"%s error: write count (%ld) differ read "
                            "count (%ld).\n",argv[0],wsize,rsize);
            return(1);
        }
        rsize=(size_t)loadhexwords(stdin,buffer,sizeof(buffer)/sizeof(CARD8));
    }
    if(!feof(stdin)){
        fprintf(stderr,"%s error: invalid character in input stream\n"
                "\t%"FMT_INT32" bytes read.\n",argv[0],isize);
        return(1); 
    }
    return(0);
}/*main;*/

/*** hextobin.c                       --                     --          ***/

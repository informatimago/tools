/******************************************************************************
FILE:               bintohex.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This filter converts a binary file to hexadecimal.
USAGE
    hexbin < binfile > hexfile
    Future usage:
        hexbin [-wWidth] [-n] [-c] [file...] [<file] >hexfile
AUTHOR
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1993-07-24 <PJB> Creation.

    Revision 1.2  2003/01/07 19:55:35  pascal
    Orthogonalized options of ascii.c
    
    Revision 1.1  2001/04/30 01:58:08  pascal
    Initial entry into CVS.
    
    Revision 1.1  2001/04/27 06:45:10  pascal
    Added to CVS.
    
    Revision 1.2  95/09/08  20:10:33  pascal
    Added produit and consomme results.
    
    Revision 1.1  95/09/08  19:40:40  pascal
    Initial revision
    
    Revision 1.3  94/07/13  12:11:43  pbo
    Transformed C++ comments into C comments.
    
    Revision 1.2  1994/02/15  05:21:15  pascal
    Added RCS keywords.

LEGAL
    GPL
    
    Copyright Pascal Bourguignon 1993 - 2011
    
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
#include <unistd.h> /* import read */
#include <string.h>
#include <stdio.h>
#include <BcTypes.h>


    static const char lohex[]="0123456789abcdef";   
    static const char hihex[]="0123456789ABCDEF";   
    
    
    static void bintohex(CARD8* in,INT32 insize,
            char* out,INT32 bytesPerLine,INT32 spaces,int hicase,int newline,
            INT32* consomme,INT32* produit)
    {
            INT32       i,j;
            CARD8       h,l;
            const char* hex;
            INT32       last;
            INT32       beforespaces;
            
        last=insize>bytesPerLine?bytesPerLine:insize;
        hex=hicase?hihex:lohex;
        beforespaces=spaces;
        j=0;
        for(i=0;i<last;i++,in++){
            h=(*in)>>4;
            l=(*in)&0xF;
            (*out++)=hex[h];
            (*out++)=hex[l];
            j+=2;
            beforespaces--;
            if(beforespaces==0){
                beforespaces=spaces;
                (*out++)=' ';
                j++;
            }
        }
        (*out++)='\n';
        j++;
        (*out)='\0';
        *consomme=last;
        *produit=j;
    }/*bintohex*/


int main(int argc,char** argv)
{
    CARD8       buffer[16*1024];
    char        outbuf[128];
    size_t      rsize;
    size_t      wsize;
    INT32       isize;
    INT32       i;
    INT32       produit;
    INT32       consomme;

    if((argv[1]!=0)&&(strcmp("-c",argv[1])==0)){
        unsigned char byte;
        do{
            if(read(0,&byte,1)){
                ssize_t r;
                int h=byte>>4;
                int l=byte&0xF;
                outbuf[0]=hihex[h];
                outbuf[1]=hihex[l];
                outbuf[2]=' ';
                r=write(1,outbuf,3);
                (void)r;
            }
        }while(1);/* TODO: EOF! */
    }else{
        isize=0;
        rsize=fread(buffer,1,sizeof(buffer),stdin);
        while(rsize>0){
            isize=isize+(INT32)rsize;
            i=0;
            while(rsize>0){
                bintohex(buffer+i,(INT32)rsize,outbuf,16,0,1,1,&consomme,&produit);
                i+=consomme;
                rsize=rsize-(size_t)consomme;
                wsize=fwrite(outbuf,1,(unsigned)produit,stdout);
                if(wsize!=produit){
                    fprintf(stderr,
                            "%s error: error while writting output stream "
                            "(%"FMT_INT32"/%"FMT_INT64").\n",argv[0],produit,wsize);
                    return(1);
                }
            }
            rsize=fread(buffer,1,sizeof(buffer),stdin);
        }
        if(!feof(stdin)){
            fprintf(stderr,"%s error: while reading input stream\n"
                    "\t%"FMT_INT32" bytes read.\n",argv[0],isize);
            return(1); 
        }
    }
    return(0);
}/*main;*/

/*** bintohex.c                       --                     --          ***/

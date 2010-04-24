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

    GPL
    Copyright Pascal J. Bourguignon 1993 - 2002

    This file is part of PJB-Tools.

    This  program is  free software;  you can  redistribute  it and/or
    modify it  under the  terms of the  GNU General Public  License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    This program  is distributed in the  hope that it  will be useful,
    but  WITHOUT ANY WARRANTY;  without even  the implied  warranty of
    MERCHANTABILITY or FITNESS FOR  A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a  copy of the GNU General Public License
    along with  this program; see the  file COPYING; if  not, write to
    the Free  Software Foundation, Inc.,  59 Temple Place,  Suite 330,
    Boston, MA 02111-1307 USA
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

    
    static int copy(FILE* input,FILE* output)
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
        int         count;
        
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


/*** binskip.c                        -- 2003-12-02 13:21:04 -- pascal   ***/

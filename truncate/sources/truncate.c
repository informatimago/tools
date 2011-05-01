/******************************************************************************
FILE:               truncate.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    Cette commande unix tronque (ou aggrandi) les fichiers passés en 
    paramètre, aux tailles indiquées.
USAGE
    truncate [-size] {fichier} { -size {fichiers} }
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1994/12/06 <PJB> Creation.
BUGS
LEGAL

    GPL
    Copyright Pascal J. Bourguignon 1994 - 2011

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
#if defined(AIX) || defined(Linux) || defined(MACOSX)
#include <unistd.h> /* truncate */
#include <sys/types.h>
#endif
#ifdef NeXT
#include <libc.h>   /* truncate */
#endif



static const char* scan_format(int size){
    /* if(sizeof(off_t)==sizeof(long long)){ */
    /*     return("%lld"); */
    /* }else */ if(size==sizeof(long)){
        return("%ld");
    }else if(size==sizeof(short)){
        return("%hd");
    }else if(size==sizeof(char)){
        return("%hhd");
    }else{
        return("%d");
    }
}




int main(int argc,char** argv)
{
    int     r;
    int     i;
    off_t  size;
    const char* off_t_scan=scan_format(sizeof(size));
    const char* pname=argv[0];
    size=0;
    for(i=1;i<argc;i++){
        if(argv[i][0]=='-'){
            r=sscanf(argv[i]+1,off_t_scan,&size);
            if(r!=1){
                fprintf(stderr,"%s: invalid size '%s'.\n",pname,argv[i]+1);
                fflush(stderr);
            }
        }else{
            if(0!=truncate(argv[i],size)){
                    char    message[BUFSIZ];
                sprintf(message,"%s: truncating '%s'",pname,argv[i]);
                perror(message);
                fflush(stderr);
            }
        }
    }
    return(0);
}/*main*/

/*** truncate.c                       -- 2003-12-02 13:20:58 -- pascal   ***/

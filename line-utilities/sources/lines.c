/******************************************************************************
FILE:               lines.c
LANGUAGE:           ANSI-C
SYSTEM:             POSIX
USER-INTERFACE:     POSIX
DESCRIPTION
    Some routines to handle lines.
AUTHORS
    <PJB> Pascal J. Bourguignon <pjb@informatimago.com>
MODIFICATIONS
    2000-09-16 <PJB> Creation.
BUGS
    Please report them to <pjb@informatimago.com>
    - Implementation of write_string is shamefull.
LEGAL
    Copyright Pascal J. Bourguignon 2000 - 2011

    GPL
    This file is part of the lgetvalue utility.

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

    Commercial licenses are available. 
    Send your requests to <pjb@informatimago.com>.
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "lines.h"


    int write_string(const char* buffer,FILE* output)
    {
        size_t remains=strlen(buffer);
        while(remains>0){
            size_t wrote=fwrite(buffer,sizeof(char),remains,output);
            if(wrote>0){
                remains-=wrote;
                buffer+=wrote;
            }
        }
        return(0);
    }/*write_string*/


    void chop_nl(char* line)
    {
        size_t i=strlen(line);
        while((i>0)&&((line[i-1]=='\n')||(line[i-1]=='\r'))){
            i--;
        }
        line[i]='\0';
        i=0;
        while((line[i]=='\n')||(line[i]=='\r')){
            i++;
        }
        if(i>0){
            strcpy(line,line+i);
        }
    }/*chop_nl*/


/*** lines.c                          --                     --          ***/

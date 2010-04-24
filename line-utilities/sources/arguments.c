/******************************************************************************
FILE:               arguments.c
LANGUAGE:           ANSI-C
SYSTEM:             POSIX
USER-INTERFACE:     POSIX
DESCRIPTION
    Some routines to handle program arguments.
AUTHORS
    <PJB> Pascal J. Bourguignon <pjb@informatimago.com>
MODIFICATIONS
    2000-09-16 <PJB> Creation.
BUGS
    Please report them to <pjb@informatimago.com>
LEGAL
    Copyright Pascal J. Bourguignon 2000 - 2000

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
    Send your requests to <pjb@informatimago.com>
******************************************************************************/
#include <string.h>
#include <stdio.h>
#include "arguments.h"


    int get_separator(char* separator,const char* argument,
                      unsigned int position,const char* pname)
    {
        if(strlen(argument)!=position+1){
            fprintf(stderr,
                    "%s error: Only one-character separator are accepted.\n"
                    "  '%s' is not a valid separator.\n",
                    pname,argument+position);
            usage(pname);
            return(1);
        }
        *separator=argument[position];
        return(0);
    }/*get_separator*/


    int get_integer(int* number,const char* argument,
                    const char* pname,int/*short*/ positive)
    {
        if(1!=sscanf(argument,"%d",number)){
            fprintf(stderr,
                    "%s error: An integer value was expected.\n"
                    "  '%s' is not a valid integer.\n",
                    pname,argument);
            usage(pname);
            return(10);
        }
        switch(positive){
        case 1:
            if(*number<0){
                fprintf(stderr,
                        "%s error: A positive integer value was expected.\n"
                        "  '%s' is not a positive integer.\n",
                        pname,argument);
                usage(pname);
                return(11);
            }
            break;
        case 2:
            if(*number<0){
                fprintf(stderr,
                        "%s error: A stricktly positive integer "
                        "value was expected.\n"
                        "  '%s' is not a stricktly positive integer.\n",
                        pname,argument);
                usage(pname);
                return(12);
            }
            break;
        default:
            break;
        }
        return(0);
    }/*get_integer*/


/*** arguments.c                      --                     --          ***/

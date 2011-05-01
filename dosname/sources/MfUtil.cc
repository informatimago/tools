/******************************************************************************
FILE:               MfUtil.cc
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This module contains utility routines.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:40:31  pascal
   Initial revision
   
    Revision 1.3  95/04/18  03:08:58  pascal
    Freezing version WORKING_1.
    
    Revision 1.2  95/04/17  03:05:09  pascal
    Added ident keyword.
    
    Revision 1.1  95/02/25  22:22:51  pascal
    Initial revision
    
    1994-12-29 <PJB> Creation. 
LEGAL
    Copyright Pascal J. Bourguignon 1994 - 2011

    GPL

    This file is part of the dosname tool.

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
extern "C"{
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
}
#include <MfUtil.hh>
#include BpClass_hh
#include <BcImplementation.h>

    static const char rcsident[]="$Id: MfUtil.cc,v 1.1 2003/12/04 03:46:04 pjbpjb Exp $";

    PROCEDURE(perrorf,(const char* format,...),void)
    {
            va_list     args;
            char        buffer[BUFSIZ];
                
        va_start(args,format);
        vsprintf(buffer,format,args);
        va_end(args);
        perror(buffer);
    }//perrorf;


//END MfUtil.

/******************************************************************************
FILE:               Pressure.cc
LANGUAGE:           C++
SYSTEM:             UNIX
USER-INTERFACE:     ANSI
DESCRIPTION
    This file defines the Pressure type.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:45:01  pascal
   Initial revision
   
    1995-01-01 <PJB> Creation.
BUGS
LEGAL
    Copyright Pascal J. Bourguignon 1995 - 2001

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
#include <string.h>
}
#include "names.h"
#include Pressure_hh
#include BpClass_hh
#include BcImplementation_h


    static const char rcsid[]="$Id: Pressure.cc,v 1.1 2003/12/04 03:46:04 pjbpjb Exp $";
    
    
    PROCEDURE(newstr,(const char* str),char*)
    {
        char*       s;
        s=(char*)malloc(strlen(str)+1);
        strcpy(s,str);
        return(s);
    }//newstr;  
    
//END Pressure.

/*** Pressure.cc                      -- 2003-12-01 05:10:52 -- pascal   ***/

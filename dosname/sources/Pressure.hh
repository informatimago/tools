/******************************************************************************
FILE:               Pressure.hh
LANGUAGE:           C++
SYSTEM:             UNIX
USER-INTERFACE:     ANSI
DESCRIPTION
    This file defines the Pressure type.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:45:29  pascal
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
#ifndef __Pressure__
#define __Pressure__
#include "names.h"
#include BcInterface_h
#include BpString_hh

    typedef enum {Pressure_light,Pressure_medium,Pressure_heavy} PressureT;

    PROCEDURE(newstr,(const char* str),char*)

#endif // Pressure.

/******************************************************************************
FILE:               dosname.hh
LANGUAGE:           C++
SYSTEM:             UNIX
USER-INTERFACE:     ANSI
DESCRIPTION
    This file defines the classes implemented and used by the dosname program.
    This is a private interface.
    The dosname program copies a unix directory to a dos directory, and vice-
    versa, renaming the files according to the file system specific convention.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:37:52  pascal
   Initial revision
   
    1995/01/01
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
#ifndef __dosname__
#define __dosname__
extern "C"{
#include <stdio.h>
}
#include "names.h"
#include AttrString_hh
#include CapWord_hh
#include MfDirectory_hh
#include MfFile_hh
#include MfMode_hh
#include MfNode_hh
#include MfRootDirectory_hh
#include MfUtil_hh
#include NameSeq_hh
#include Renamer_hh
#include WordSeq_hh
#include dosname_hh
#include Pressure_hh
     

#endif //__dosname__


/******************************************************************************
FILE:               AttrString.hh
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is a subclass of BpString to which an attribute can be associated.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:44:56  pascal
   Initial revision
   
    1995-11-05 <PJB> Creation. 
LEGAL
    Copyright Pascal J. Bourguignon 1995 - 2011

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
#ifndef __AttrString__
#define __AttrString__
#include "names.h"
#include BcInterface_h
#include BcTypes_h
#include BpString_hh
#include MfMode_hh
#include Pressure_hh

#define AttrString_SUPER BpString
class AttrString:public AttrString_SUPER
    // Yes, I know: this is not gracious object programming, but it's easy.
{
public:
// birth and death:

    CONSTRUCTOR(AttrString)
    DESTRUCTOR(AttrString)

        
// override of BpObject methods:

    METHOD(AttrString,makeBrother,(void),BpObject*)
    METHOD(AttrString,printOnLevel,(FILE* file,CARD32 level),void)


// override of BpString methods:


// AttrString methods:

    METHOD(AttrString,setMode,(MfMode* nMode),void)
    METHOD(AttrString,mode,(void),MfMode*)

protected:
    MfMode*         fMode;
};

#endif // AttrString.

/******************************************************************************
FILE:               MfMode.hh
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This class represent UNIX access rights.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    $Id: MfMode.hh,v 1.1 2003/12/04 03:46:04 pjbpjb Exp $
   Revision 1.1  95/11/19  08:41:45  pascal
   Initial revision
   
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
#ifndef __MfMode__
#define __MfMode__
#include <BcInterface.h>
#include <BcTypes.h>
#include <BpObject.hh>
#include <BpString.hh>

#define MfMode_SUPER BpObject
class MfMode:public MfMode_SUPER
{
public:
    CONSTRUCTOR(MfMode)
    DESTRUCTOR(MfMode)
    
// override of BpObject methods:

    METHOD(MfMode,makeBrother,(void),BpObject*)
    METHOD(MfMode,printOnLevel,(FILE* file,CARD32 level),void)

// MfMode methods:
    
    METHOD(MfMode,mode,(void),CARD16)
    METHOD(MfMode,modeSet,(CARD16 nMode),void)

    METHOD(MfMode,stringValue,(void),BpString*)
    METHOD(MfMode,stringValueSet,(BpString* nModeStr),void)
    
protected:
    CARD16          fMode;
};
#endif //__MfMode__


/******************************************************************************
FILE:               AttrString.cc
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the implementation of the AttrString class.
    This class blah_blah_blah.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:32:57  pascal
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
extern "C"{
#include <stdlib.h>
}
#include "names.h"
#include AttrString_hh
#include BpClass_hh
#include BcImplementation_h

    static const char rcsid[]="$Id: AttrString.cc,v 1.1 2003/12/04 03:46:04 pjbpjb Exp $";
    
    
    CONSTRUCTOR(AttrString)
    {
        BpClass_PLUG(AttrString);
        fMode=NIL;
    }//AttrString;
    
    
    DESTRUCTOR(AttrString)
    {
        if(fMode!=NIL){
            fMode->release();
        }
    }//~AttrString;
    
    
    METHOD(AttrString,makeBrother,(void),BpObject*)
    {
        return(NEW(AttrString));
    }//makeBrother;
    
    
    METHOD(AttrString,printOnLevel,(FILE* file,CARD32 level),void)
    {
        AttrString_SUPER::printOnLevel(file,level);
        PRINTONLEVEL(file,level,"%p",fMode,(void*)fMode);
        if(fMode!=NIL){
            PRINTONLEVEL_OPEN(file,level,fMode);
            fMode->printOnLevel(file,level+1);
            PRINTONLEVEL_CLOSE(file,level);
        }
    }//printOnLevel;
    

    METHOD(AttrString,setMode,(MfMode* nMode),void)
    {
        if(fMode!=NIL){
            fMode->release();
        }
        fMode=nMode;
        fMode->retain();
    }//setMode;
    
    
    METHOD(AttrString,mode,(void), MfMode*)
    {
        return(fMode);
    }//mode;
    

//END AttrString.

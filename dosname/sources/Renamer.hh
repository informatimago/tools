/******************************************************************************
FILE:               Renamer.hh
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the definition of the Renamer class.
    The instances of this class can rename unix to dos or dos to unix files,
    using a conversion dictionnary and a heuristic based on the parsing of 
    the UNIX file name.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:44:41  pascal
   Initial revision
   
    1995-11-05 <PJB> Creation. 
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
#ifndef __Renamer__
#define __Renamer__
#include "names.h"
#include BcInterface_h
#include BcTypes_h
#include BpObject_hh
#include BpDict_hh
#include MfMode_hh
#include Pressure_hh

#define Renamer_SUPER BpObject
class Renamer:public Renamer_SUPER
{
public:
// birth and death:

    CONSTRUCTOR(Renamer)
    DESTRUCTOR(Renamer)

        
// override of BpObject methods:

    METHOD(Renamer,makeBrother,(void),BpObject*)
    METHOD(Renamer,printOnLevel,(FILE* file,CARD32 level),void)


// override of BpObject methods:


// Renamer methods:

    METHOD(Renamer,setUnixName,(const char* nUN,MfMode* nMode),void)
    METHOD(Renamer,dosName,(void),const char*)

    METHOD(Renamer,setDosName,(const char* nDN),void)
    METHOD(Renamer,unixName,(void),const char*)
    METHOD(Renamer,mode,(void), MfMode*)
    
    METHOD(Renamer,loadUnixCache,(FILE* fCache),void)
    METHOD(Renamer,saveUnixCache,(FILE* fCache),void)
    METHOD(Renamer,loadDosCache,(FILE* fCache),void)
    METHOD(Renamer,saveDosCache,(FILE* fCache),void)

protected:

    METHOD(Renamer,processDots,(char* processedUN),void)
    METHOD(Renamer,computeDosName,(void),void)
    METHOD(Renamer,computeUnixName,(void),void)
    METHOD(Renamer,loadCache,(FILE* fCache,BpDict* cache,BOOLEAN unox),void)
    METHOD(Renamer,saveCache,(FILE* fCache,BpDict* cache,BOOLEAN unox),void)

    BpDict*     dosToUnix;
    BpDict*     unixToDos;
    char*       curUnixName;
    MfMode*     curMode;
    char*       curDosName;
};

#endif // Renamer.

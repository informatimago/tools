/******************************************************************************
FILE:               MfLink.hh
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the definition of the MfLink class.
    This class reprensents UNIX symbolic links.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:45:50  pascal
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
#ifndef __MfLink__
#define __MfLink__
#include "names.h"
#include BcInterface_h
#include BcTypes_h
#include MfFile_hh

#define MfLink_SUPER MfFile
class MfLink:public MfLink_SUPER
{
public:
// birth and death:

    CONSTRUCTOR(MfLink)
    DESTRUCTOR(MfLink)

        
// override of BpObject methods:

    METHOD(MfLink,makeBrother,(void),BpObject*)
    METHOD(MfLink,printOnLevel,(FILE* file,CARD32 level),void)

// override of MfNode methods:

    METHOD(MfLink,exists,(void),BOOLEAN)
    METHOD(MfLink,create,(void),BOOLEAN)
    METHOD(MfLink,open,(const char* omode),BOOLEAN)
    METHOD(MfLink,read,(void* buffer,INT32 bufSize),INT32)
    METHOD(MfLink,write,(void* buffer,INT32 bufSize),INT32)
    METHOD(MfLink,seek,(INT32 position),INT32)
    METHOD(MfLink,eof,(void),BOOLEAN)
    METHOD(MfLink,close,(void),void)



    METHOD(MfLink,copyToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)

    METHOD(MfLink,copyToUnix,
           (MfNode* uDir,BpString* uName,MfMode* uMode,BOOLEAN printIt),void)

    METHOD(MfLink,symLinkToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)

    METHOD(MfLink,symLinkToUnix,
           (MfNode* uDir,BpString* uName,MfMode* uMode,BOOLEAN printIt),void)

    METHOD(MfLink,hardLinkToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)

    METHOD(MfLink,hardLinkToUnix,
           (MfNode* uDir,BpString* uName,MfMode* uMode,BOOLEAN printIt),void)



// override of MfFile methods:

    METHOD(MfLink,size,(void),CARD32)


// MfLink methods:


protected:
    INT32       state;
    INT32       position;
    INT32       fEof;
    char        linkbuf[BUFSIZ];
};

#endif // MfLink.

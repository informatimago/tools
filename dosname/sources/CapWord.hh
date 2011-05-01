/******************************************************************************
FILE:               CapWord.hh
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the definition of the CapWord class.
    This class represent the CapWord non terminal in the grammar used
    to analyse UNIX file names.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:42:32  pascal
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
#ifndef __CapWord__
#define __CapWord__
#include "names.h"
#include BcInterface_h
#include BcTypes_h
#include BpObject_hh
#include Pressure_hh

    typedef enum {
        ck_cap,ck_low,ck_dig,ck_ini,ck_end,ck_sep,ck_ign
    }           CharKindT;
    
    extern CharKindT charKind[256];
    
#define CapWord_SUPER BpObject
class CapWord:public CapWord_SUPER
{
public:
// birth and death:

    CONSTRUCTOR(CapWord)
    DESTRUCTOR(CapWord)

        
// override of BpObject methods:

    METHOD(CapWord,makeBrother,(void),BpObject*)
    METHOD(CapWord,printOnLevel,(FILE* file,CARD32 level),void)


// override of BpObject methods:


// CapWord methods:

    METHOD(CapWord,analyze,(const char* source,INT32* next),void)
    METHOD(CapWord,catenate,(char* buffer,INT32* bsize),void)
    METHOD(CapWord,reduceBy,(PressureT pressure,float ratio),void)
    METHOD(CapWord,reduceSoft,(INT32* remainder),void)
    METHOD(CapWord,print,(FILE* file),void)
    METHOD(CapWord,length,(PressureT pressure),INT32)
    METHOD(CapWord,compareLength,(CapWord* other),SignT)

protected:

    METHOD(CapWord,reduceSlice,(INT32 slice),INT32)
    METHOD(CapWord,analyzeRun,
                    (const char* source,INT32* next,INT32 kinds),char*)
    
    char*       initers;
    char*       letters;
    char*       trailers;
};

#endif // CapWord.

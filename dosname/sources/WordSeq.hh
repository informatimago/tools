/******************************************************************************
FILE:               WordSeq.hh
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the definition of the WordSeq class.
    This class represent the WordSeq non terminal in the grammar used
    to analyse UNIX file names.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:42:53  pascal
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
#ifndef __WordSeq__
#define __WordSeq__
#include "names.h"
#include BcInterface_h
#include BcTypes_h
#include BpObject_hh
#include BpList_hh
#include Pressure_hh


#define WordSeq_SUPER BpObject
class WordSeq:public WordSeq_SUPER
{
public:
// birth and death:

    CONSTRUCTOR(WordSeq)
    DESTRUCTOR(WordSeq)

        
// override of BpObject methods:

    METHOD(WordSeq,makeBrother,(void),BpObject*)
    METHOD(WordSeq,printOnLevel,(FILE* file,CARD32 level),void)


// override of BpObject methods:


// WordSeq methods:

    METHOD(WordSeq,append,(WordSeq* tail),void)
    METHOD(WordSeq,analyze,(const char* source,INT32* next),void)
    METHOD(WordSeq,catenate,(char* buffer,INT32* bsize),void)
    METHOD(WordSeq,reduceTo,(INT32 maxSize),void)
    METHOD(WordSeq,print,(FILE* file),void)
    METHOD(WordSeq,length,(PressureT pressure),INT32)

protected:
    BpList*     words;
};

#endif // WordSeq.

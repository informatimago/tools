/******************************************************************************
FILE:               NameSeq.hh
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the definition of the NameSeq class.
    This class represent the NameSeq non terminal in the grammar used
    to analyse UNIX file names.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:43:11  pascal
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
#ifndef __NameSeq__
#define __NameSeq__
#include "names.h"
#include BcInterface_h
#include BcTypes_h
#include BpObject_hh
#include BpList_hh
#include Pressure_hh

#define NameSeq_SUPER BpObject
class NameSeq:public NameSeq_SUPER
{
public:
// birth and death:

    CONSTRUCTOR(NameSeq)
    DESTRUCTOR(NameSeq)

        
// override of BpObject methods:

    METHOD(NameSeq,makeBrother,(void),BpObject*)
    METHOD(NameSeq,printOnLevel,(FILE* file,CARD32 level),void)


// override of BpObject methods:


// NameSeq methods:

    METHOD(NameSeq,analyze,(const char* source,INT32* next),void)
    METHOD(NameSeq,catenate,(char* buffer,INT32* bsize),void)
    METHOD(NameSeq,reduce,(void),void)
    METHOD(NameSeq,print,(FILE* file),void)

protected:

    METHOD(NameSeq,catenateHead,(char* buffer,INT32* bsize),void)
    METHOD(NameSeq,catenateTail,(char* buffer,INT32* bsize),void)

    BpList*     sequences;
};

#endif // NameSeq.

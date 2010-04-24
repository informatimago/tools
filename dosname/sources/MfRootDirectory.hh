/******************************************************************************
FILE:               MfRootDirectory.hh
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the definition of the MfRootDirectory class.
    This class blah_blah_blah.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    $Id: MfRootDirectory.hh,v 1.1 2003/12/04 03:46:04 pjbpjb Exp $
   Revision 1.1  95/11/19  08:40:28  pascal
   Initial revision
   
    Revision 1.3  95/04/18  03:08:56  pascal
    Freezing version WORKING_1.
    
    Revision 1.2  95/04/17  03:05:09  pascal
    Added ident keyword.
    
    Revision 1.1  95/02/25  22:22:45  pascal
    Initial revision
    
    1994-12-25 <PJB> Creation. 
LEGAL
    Copyright Pascal J. Bourguignon 1994 - 2001

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
#ifndef __MfRootDirectory__
#define __MfRootDirectory__
#include <BcInterface.h>
#include <BcTypes.h>
#include <BpList.hh>
#include <MfDirectory.hh>

#define MfRootDirectory_SUPER MfDirectory
class MfRootDirectory:public MfRootDirectory_SUPER
{
public:
// birth and death:

    CONSTRUCTOR(MfRootDirectory)
    DESTRUCTOR(MfRootDirectory)

        
// override of BpObject methods:

    METHOD(MfRootDirectory,makeBrother,(void),BpObject*)
    METHOD(MfRootDirectory,printOnLevel,(FILE* file,CARD32 level),void)


// MfRootDirectory methods:

    METHOD(MfRootDirectory,buildDeepFile,(void),void)
        /*
            POST:       deepFile built.
        */
    METHOD(MfRootDirectory,deepFileCard,(void),CARD32)
        /*
            PRE:        deepFile built;
        */
    METHOD(MfRootDirectory,deepFile,(CARD32 index),MfFile*)
        /*
            PRE:        deepFile built;
        */
    METHOD(MfDirectory,makeDeepFilesPerformWith,
                        (BpList_ActionWithPr action,BpObject* anObject),void)
        /*
            PRE:        deepFile built;
        */

protected:
    BpList*         fDeepFiles;
};

#endif // MfRootDirectory.

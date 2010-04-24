/******************************************************************************
FILE:               MfRootDirectory.cc
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the implementation of the MfRootDirectory class.
    This class blah_blah_blah.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:39:56  pascal
   Initial revision
   
    Revision 1.3  95/04/18  03:08:54  pascal
    Freezing version WORKING_1.
    
    Revision 1.2  95/04/17  03:05:08  pascal
    Added ident keyword.
    
    Revision 1.1  95/02/25  22:22:50  pascal
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
#include <MfRootDirectory.hh>
#include BpClass_hh
#include <BcImplementation.h>

    static const char rcsident[]="$Id: MfRootDirectory.cc,v 1.1 2003/12/04 03:46:04 pjbpjb Exp $";

// birth and death:

    CONSTRUCTOR(MfRootDirectory)
    {
        BpClass_PLUG(MfRootDirectory);
        fDeepFiles=NEW(BpList);
        fDeepFiles->retain();
    }//MfRootDirectory;
        
    
    DESTRUCTOR(MfRootDirectory)
    {
        fDeepFiles->release();
    }//~MfRootDirectory;


// override of BpObject methods:

    METHOD(MfRootDirectory,makeBrother,(void),BpObject*)
    {
        return(NEW(MfRootDirectory));
    }//makeBrother;


    METHOD(MfRootDirectory,printOnLevel,(FILE* pfile,CARD32 level),void)
    {
        MfRootDirectory_SUPER::printOnLevel(pfile,level);
        PRINTONLEVEL_OPEN(pfile,level,fDeepFiles);
        fDeepFiles->printOnLevel(pfile,level+4);
        PRINTONLEVEL_CLOSE(pfile,level);
    }//printOnLevel;

    
// override of MfDirectory methods:


// MfRootDirectory methods:

    PROCEDURE(BuildDeepFile,(BpObject* obj,BpObject* with),void)
    {
            MfNode*     node=(MfNode*)obj;
            BpList*     deepFiles=(BpList*)with;
        
        if(node->isDirectory()){
            node->makeContentsPerformWith(BuildDeepFile,with);
        }else{
            deepFiles->addObject(node);
        }
    }//BuildDeepFile;
    
    
    METHOD(MfRootDirectory,buildDeepFile,(void),void)
    {
        makeContentsPerformWith(BuildDeepFile,fDeepFiles);
    }//buildDeepFile;


    METHOD(MfRootDirectory,deepFileCard,(void),CARD32)
    {
        return(fDeepFiles->count());
    }//deepFileCard;


    METHOD(MfRootDirectory,deepFile,(CARD32 index),MfFile*)
    {
        return((MfFile*)(fDeepFiles->objectAt(index)));
    }//deepFile;


    METHOD(MfRootDirectory,makeDeepFilesPerformWith,
                        (BpList_ActionWithPr action,BpObject* anObject),void)
    {
        fDeepFiles->makeObjectsPerformWith(action,anObject);
    }//makeDeepFilesPerformWith;


    

//END MfRootDirectory.

/*** MfRootDirectory.cc               -- 2003-12-01 05:00:50 -- pascal   ***/

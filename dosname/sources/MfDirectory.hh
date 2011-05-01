/******************************************************************************
FILE:               MfDirectory.hh
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the definition of the MfDirectory class.
    This class represents a file system directory.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    $Id: MfDirectory.hh,v 1.1 2003/12/04 03:46:04 pjbpjb Exp $
   Revision 1.1  95/11/19  08:37:57  pascal
   Initial revision
   
    Revision 1.4  95/04/18  03:08:27  pascal
    Freezing version WORKING_1.
    
    Revision 1.3  95/04/17  03:05:35  pascal
    Version optimized for space. Added ident keyword.
    
    Revision 1.2  95/04/17  02:17:12  pascal
    Version with fPath cache in MfNode.
    
    Revision 1.1  95/02/25  22:22:41  pascal
    Initial revision
    
    1994-12-25 <PJB> Creation. 
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
#ifndef __MfDirectory__
#define __MfDirectory__
#include <BcInterface.h>
#include <BcTypes.h>
#include <MfNode.hh>
#include <MfFile.hh>
#include <MfLink.hh>
#include <Renamer.hh>


    typedef enum {MfDirectory_FileSystem_UNIX,MfDirectory_FileSystem_MSDOS} MfDirectory_FileSystemT;
    
// A private class for MfDirectory:
class MfNodeAction {
public:
    METHOD(MfNodeAction,doIt,\
           (MfNode* that,MfNode* dir,BpString* name,MfMode* mode,
            BOOLEAN printIt)=0,void)
};//MfNodeAction;


#define MfDirectory_SUPER MfNode
class MfDirectory:public MfDirectory_SUPER
{
public:
// birth and death:

    CONSTRUCTOR(MfDirectory)
    DESTRUCTOR(MfDirectory)

        
// override of BpObject methods:

    METHOD(MfDirectory,makeBrother,(void),BpObject*)
    METHOD(MfDirectory,printOnLevel,(FILE* file,CARD32 level),void)


// override of MfNode methods:

    METHOD(MfDirectory,contentSet,(CARD32 index,MfNode* nContent),void)
    METHOD(MfDirectory,contentRemove,(CARD32 index),MfNode*)


    METHOD(MfDirectory,copyToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)

    METHOD(MfDirectory,copyToUnix,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)

    METHOD(MfDirectory,symLinkToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)

    METHOD(MfDirectory,symLinkToUnix,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)

    METHOD(MfDirectory,hardLinkToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)

    METHOD(MfDirectory,hardLinkToUnix,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)



    METHOD(MfDirectory,create,(void),BOOLEAN)
    METHOD(MfDirectory,open,(const char* omode),BOOLEAN)
    METHOD(MfDirectory,read,(void* buffer,INT32 bufSize),INT32)
    METHOD(MfDirectory,write,(void* buffer,INT32 bufSize),INT32)
    METHOD(MfDirectory,seek,(INT32 position),INT32)
    METHOD(MfDirectory,eof,(void),BOOLEAN)
    METHOD(MfDirectory,close,(void),void)
    
// MfDirectory methods:

    METHOD(MfDirectory,loadFiles,(void),void)
        /*
            DO:         recursively scan the file system, create and load
                        the content objects.
        */
    
    METHOD(MfDirectory,fileCard,(void),CARD32)
    METHOD(MfDirectory,file,(CARD32 index),MfFile*)
        
    METHOD(MfDirectory,makeFilesPerformWith,
                        (BpList_ActionWithPr action,BpObject* anObject),void)

    METHOD(MfDirectory,newDirectory,(void),MfDirectory*)
        /*
            RETURN:     a new directory of the same sub-class than this one.
        */
    
    METHOD(MfDirectory,newFile,(void),MfFile*)
        /*
            RETURN:     a new file of a sub-class of MfFile corresponding to
                        the sub-class of this.
        */
    METHOD(MfDirectory,newLink,(void),MfLink*)
        /*
            RETURN:     a new link of a sub-class of MfLink corresponding to
                        the sub-class of this.
        */


    METHOD(MfDirectory,fileSystem,(void),MfDirectory_FileSystemT)
    METHOD(MfDirectory,fileSystemSet,
                        (MfDirectory_FileSystemT nFileSystem),void)
    
    METHOD(MfDirectory,renamerSet,(Renamer* nRenamer),void)
        /*
            POST:       this directory is a copy directory: it will save the
                        renamer cache.
        */
    
    METHOD(MfDirectory,loadRenamerCache,(void),void)
    METHOD(MfDirectory,saveRenamerCache,(void),void)
    METHOD(MfDirectory,printRenamerCache,(void),void)


protected:
    BpList*         fFiles;
    MfDirectory_FileSystemT     fFileSystem;
    Renamer*        renamer;
    MfDirectory*    theCopy; // used while walking the tree.
    MfMode*         theMode; // used while walking the tree.



    METHOD(MfDirectory,actionToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt,
            MfNodeAction* action),void)
    METHOD(MfDirectory,actionToUnix,
           (MfNode* uDir,BpString* uName,MfMode* uMode,BOOLEAN printIt,
            MfNodeAction* action),void)
};

#endif // MfDirectory.


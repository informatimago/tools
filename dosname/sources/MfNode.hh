/******************************************************************************
FILE:               MfNode.hh
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the definition of the MfNode class.
    This class abstract the directory entry. Subclasses are directories,
    files, links, ...
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    $Id: MfNode.hh,v 1.1 2003/12/04 03:46:04 pjbpjb Exp $
   Revision 1.1  95/11/19  08:39:52  pascal
   Initial revision
   
    Revision 1.4  95/04/18  03:08:44  pascal
    Freezing version WORKING_1.
    
    Revision 1.3  95/04/17  03:05:06  pascal
    Added ident keyword.
    
    Revision 1.2  95/04/17  02:08:45  pascal
    contentSet corrected.
    
    Revision 1.1  95/02/25  22:22:43  pascal
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
#ifndef __MfNode__
#define __MfNode__
#include <BcInterface.h>
#include <BcTypes.h>
#include <BpObject.hh>
#include <BpString.hh>
#include <BpList.hh>
#include <MfMode.hh>



#define MfNode_SUPER BpObject
class MfNode:public MfNode_SUPER
{
public:
// birth and death:

    CONSTRUCTOR(MfNode)
    DESTRUCTOR(MfNode)

        
// override of BpObject methods:

    METHOD(MfNode,makeBrother,(void),BpObject*)
    METHOD(MfNode,printOnLevel,(FILE* file,CARD32 level),void)


// MfNode methods:

    METHOD(MfNode,nameSet,(BpString* nName),void)
    METHOD(MfNode,nameGet,(BpString* oName),void)
    METHOD(MfNode,name,(void),BpString*)
    METHOD(MfNode,pathNameGet,(BpString* oPath),void)
    METHOD(MfNode,pathName,(void),BpString*)

    METHOD(MfNode,modeSet,(MfMode* nMode),void)
    METHOD(MfNode,mode,(void),MfMode*)
    
    METHOD(MfNode,exists,(void),BOOLEAN)
        /*
            RETURN:     there is something named pathName() in the file system.
        */
        
    METHOD(MfNode,isDirectory,(void),BOOLEAN)
        /*
            RETURN:     TRUE
            NOTE:       This is the default result; MfFile override it.
        */
    
    METHOD(MfNode,holderCard,(void),CARD32)
    METHOD(MfNode,holder,(CARD32 index),MfNode*)
    METHOD(MfNode,holderSet,(CARD32 index,MfNode* nHolder),void)
    METHOD(MfNode,holderRemove,(CARD32 index),MfNode*)
        
    METHOD(MfNode,contentCard,(void),CARD32)
    METHOD(MfNode,content,(CARD32 index),MfNode*)
    METHOD(MfNode,contentSet,(CARD32 index,MfNode* nContent),void)
    METHOD(MfNode,contentRemove,(CARD32 index),MfNode*)

    METHOD(MfNode,makeContentsPerformWith,
                        (BpList_ActionWithPr action,BpObject* anObject),void)


    METHOD(MfNode,copyToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)

    METHOD(MfNode,copyToUnix,
           (MfNode* uDir,BpString* uName,MfMode* uMode,BOOLEAN printIt),void)

    METHOD(MfNode,symLinkToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)

    METHOD(MfNode,symLinkToUnix,
           (MfNode* uDir,BpString* uName,MfMode* uMode,BOOLEAN printIt),void)

    METHOD(MfNode,hardLinkToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)

    METHOD(MfNode,hardLinkToUnix,
           (MfNode* uDir,BpString* uName,MfMode* uMode,BOOLEAN printIt),void)

    /*
            PRE:        dDir is a MfDirectory where this node will be 
                        copied to. dName is the name of the copy of this node.
                        dMode is the (unix) mode to which the copy will be set.

            NOTE:       These {copy,{sym,hard}Link}To{Dos,Unix} are abstract 
                        methods implemented by subclasses to copy, symlink, 
                        or hardlink a unix file or directory with a unix  
                        name to a dos file or directory with a dos name, and 
                        vice-versa.

                        These methods only act on the current node. 
                        They do not recurse with the contents of directories, 
                        preWalkWith is used to do that.

                        For directory nodes, the sym-link and hard-link methods
                        will probably do a "copy" of the directory as well.

                        If printIt then only writes on stdout the commands 
                        needed to do the actual actions.
        */
    
      

    METHOD(MfNode,create,(void),BOOLEAN)
    METHOD(MfNode,open,(const char* omode),BOOLEAN)
    METHOD(MfNode,read,(void* buffer,INT32 bufSize),INT32)
    METHOD(MfNode,write,(void* buffer,INT32 bufSize),INT32)
    METHOD(MfNode,seek,(INT32 position),INT32)
    METHOD(MfNode,eof,(void),BOOLEAN)
    METHOD(MfNode,close,(void),void)
    
    METHOD(MfNode,traceSet,(BOOLEAN nTrace),void)
protected:
    BpString*       fName;
    MfMode*         fMode;
    MfNode*         fHolder;
    BpList*         fContents;
    FILE*           fFile;
    
    BOOLEAN         fTrace;
};

#endif // MfNode.

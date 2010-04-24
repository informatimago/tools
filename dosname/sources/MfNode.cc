/******************************************************************************
FILE:               MfNode.cc
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the implementation of the MfNode class.

    Optimization: the path of the file is computed only once.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:39:09  pascal
   Initial revision
   
    Revision 1.4  95/04/18  03:08:42  pascal
    Freezing version WORKING_1.
    
    Revision 1.3  95/04/17  03:05:05  pascal
    Added ident keyword.
    
    Revision 1.2  95/04/17  02:07:50  pascal
    contentSet corrected.
    
    Revision 1.1  95/02/25  22:22:49  pascal
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
extern "C"{
#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef unix
#include <unistd.h>
#else
#include <libc.h>       /* creat */
#endif
}
#include <MfNode.hh>
#include <MfUtil.hh>
extern "C"{
#include <BcExcept.h>
}
#include BpClass_hh
#include <BcImplementation.h>

    static const char rcsident[]="$Id: MfNode.cc,v 1.1 2003/12/04 03:46:04 pjbpjb Exp $";


// birth and death:


    CONSTRUCTOR(MfNode)
    {
        BpClass_PLUG(MfNode);
        fName=NEW(BpString);
        fName->retain();
        fMode=NEW(MfMode);
        fMode->retain();
        fMode->modeSet(00777);
        fHolder=NIL;
        fContents=NEW(BpList);
        fContents->retain();
        fTrace=FALSE;
    }//MfNode;
        
    
    DESTRUCTOR(MfNode)
    {
        fContents->release();
        fName->release();
        if(fMode!=NIL){
            fMode->release();
        }
    }//~MfNode;


// override of BpObject methods:

    METHOD(MfNode,makeBrother,(void),BpObject*)
    {
        return(NEW(MfNode));
    }//makeBrother;


    METHOD(MfNode,printOnLevel,(FILE* file,CARD32 level),void)
    {
        MfNode_SUPER::printOnLevel(file,level);
        PRINTONLEVEL(file,level,"%s",fName,fName->string());
        PRINTONLEVEL(file,level,"%p",fHolder,(void*)fHolder);
        PRINTONLEVEL_OPEN(file,level,fContents);
        fContents->printOnLevel(file,level+4);
        PRINTONLEVEL_CLOSE(file,level);
    }//printOnLevel;


// MfNode methods:

    METHOD(MfNode,nameSet,(BpString* nName),void)
    {
        fName->copyFrom(nName);
    }//nameSet;
    
    
    METHOD(MfNode,nameGet,(BpString* oName),void)
    {
        oName->copyFrom(fName);
    }//nameGet;
    
    
    METHOD(MfNode,name,(void),BpString*)
    {
            BpString*       oName=NEW(BpString);
        oName->copyFrom(fName);
        return(oName);
    }//name;
    

    METHOD(MfNode,pathNameGet,(BpString* oPath),void)
    {
        if(fHolder!=NIL){
            fHolder->pathNameGet(oPath);
            oPath->appendString("/");
            oPath->append(fName);
        }else{
            oPath->copyFrom(fName);     // we have relative paths.
        }
    }//pathNameGet;


    METHOD(MfNode,pathName,(void),BpString*)
    {
            BpString*       path=NEW(BpString);
        pathNameGet(path);
        return(path);
    }//pathName;
    
    
    METHOD(MfNode,mode,(void),MfMode*)
    {
        return(fMode);
    }//mode;
    
                
    METHOD(MfNode,modeSet,(MfMode* nMode),void)
    {
        if(fMode!=NIL){
            fMode->release();
        }
        fMode=nMode;
        fMode->retain();
    }//modeSet;
    
    
    METHOD(MfNode,exists,(void),BOOLEAN)
    {
            struct stat         status;
        return(lstat(pathName()->string(),&status)>=0);
    }//exists;
    
    
    METHOD(MfNode,isDirectory,(void),BOOLEAN)
    {
        return(TRUE);
    }//isDirectory;
    
    
    METHOD(MfNode,holderCard,(void),CARD32)
    {
        return((fHolder==NIL)?0:1);
    }//holderCard;
    
    
    METHOD(MfNode,holder,(CARD32 index),MfNode*)
    {
        return((index==0)?fHolder:NIL);
    }//holder;
    
    
    METHOD(MfNode,holderSet,(CARD32 index,MfNode* nHolder),void)
    {
        if((index==0)&&(nHolder!=NIL)){
            if(fHolder!=NIL){
                fHolder->release();
            }
            fHolder=nHolder;
            fHolder->retain();
        }
    }//holderSet;
    
    
    METHOD(MfNode,holderRemove,(CARD32 index),MfNode*)
    {
            MfNode*     result;
        if(index==0){
            result=fHolder;
            fHolder->release();
            fHolder=NIL;
        }else{
            result=NIL;
        }
        return(result);
    }//holderRemove;
    
    
    METHOD(MfNode,contentCard,(void),CARD32)
    {
        return(fContents->count());
    }//contentCard;
    
    
    METHOD(MfNode,content,(CARD32 index),MfNode*)
    {
        return((MfNode*)(fContents->objectAt(index)));
    }//content;
    
    
    METHOD(MfNode,contentSet,(CARD32 index,MfNode* nContent),void)
    {
            CARD32      count;
        count=fContents->count();
        if(index<count){
            fContents->replaceObjectAtWith(index,nContent);
        }else{
            fContents->insertObjectAt(nContent,count);
        }
    }//contentSet;
    
    
    METHOD(MfNode,contentRemove,(CARD32 index),MfNode*)
    {
        return((MfNode*)(fContents->removeObjectAt(index)));
    }//contentRemove;
    

    METHOD(MfNode,makeContentsPerformWith,
                        (BpList_ActionWithPr action,BpObject* anObject),void)
    {
        fContents->makeObjectsPerformWith(action,anObject);
    }//makeContentsPerformWith;
    

    METHOD(MfNode,copyToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)
    {
        BcRAISE(BpObject_eSubclassResponsibility,(void*)this,"copyToDos");
    }//copyToDos;

    METHOD(MfNode,copyToUnix,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)
    {
        BcRAISE(BpObject_eSubclassResponsibility,(void*)this,"copyToUnix");
    }//copyToUnix;

    METHOD(MfNode,symLinkToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)
    {
        BcRAISE(BpObject_eSubclassResponsibility,(void*)this,"symLinkToDos");
    }//symLinkToDos;

    METHOD(MfNode,symLinkToUnix,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)
    {
        BcRAISE(BpObject_eSubclassResponsibility,(void*)this,"symLinkToUnix");
    }//symLinkToUnix;

    METHOD(MfNode,hardLinkToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)
    {
        BcRAISE(BpObject_eSubclassResponsibility,(void*)this,"hardLinkToDos");
    }//hardLinkToDos;

    METHOD(MfNode,hardLinkToUnix,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)
    {
        BcRAISE(BpObject_eSubclassResponsibility,(void*)this,"hardLinkToUnix");
    }//hardLinkToUnix;
    
    
    METHOD(MfNode,create,(void),BOOLEAN)
    {
            int     fd;
        fd=creat(pathName()->string(),07777&(fMode->mode()));
        if(fd>=0){
            /*Unix*/::close(fd);
            return(TRUE);
        }else{
            return(FALSE);
        }
    }//create;
    
    
    METHOD(MfNode,open,(const char* omode),BOOLEAN)
    {
        fFile=fopen(pathName()->string(),omode);
        return(fFile!=NULL);
    }//open;
    
    
    METHOD(MfNode,read,(void* buffer,INT32 bufSize),INT32)
    {
        if(fFile!=NIL){
            clearerr(fFile);
            return(fread(buffer,1,bufSize,fFile));
        }else{
            return(-1);
        }
    }//read;
    
    
    METHOD(MfNode,write,(void* buffer,INT32 bufSize),INT32)
    {
        if(fFile!=NIL){
            clearerr(fFile);
            return(fwrite(buffer,1,bufSize,fFile));
        }else{
            return(-1);
        }
    }//write;
    
    
    METHOD(MfNode,seek,(INT32 position),INT32)
    {
        if(fFile!=NIL){
            clearerr(fFile);
            return(fseek(fFile,position,SEEK_SET));
        }else{
            return(0);
        }
    }//seek;
    
    
    METHOD(MfNode,eof,(void),BOOLEAN)
    {
        if(fFile!=NIL){
            return(feof(fFile));
        }else{
            return(TRUE);
        }
    }//eof;
    
    
    METHOD(MfNode,close,(void),void)
    {
        if(fFile!=NIL){
            clearerr(fFile);
            fclose(fFile);
        }
    }//close;
    

    METHOD(MfNode,traceSet,(BOOLEAN nTrace),void)
    {
        fTrace=nTrace;
    }//traceSet;    
//END MfNode.

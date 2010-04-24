/******************************************************************************
FILE:               MfDirectory.cc
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the implementation of the MfDirectory class.
    This class blah_blah_blah.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:35:48  pascal
   Initial revision
   
    Revision 1.4  95/04/18  03:08:24  pascal
    Freezing version WORKING_1.
    
    Revision 1.3  95/04/17  03:05:35  pascal
    Version optimized for space. Added ident keyword.
    
    Revision 1.2  95/04/17  02:17:10  pascal
    Version with fPath cache in MfNode.
    
    Revision 1.1  95/02/25  22:22:47  pascal
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
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#ifdef unix
#include <unistd.h>
#else
#include <libc.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
}
#include "names.h"
#include MfDirectory_hh
#include MfUtil_hh
#include Renamer_hh
#include BpClass_hh
#include BcImplementation_h

    static const char rcsident[]="$Id: MfDirectory.cc,v 1.2 2004/10/27 03:38:20 pupjb Exp $";


#define DECL_NODEACTION(cname,mname) \
    class cname : public MfNodeAction { \
      public: \
        cname(void);\
        void doIt(MfNode* that,MfNode* dir,\
                        BpString* name,MfMode* mode,BOOLEAN printIt); \
    }; \
    cname::cname(void){} \
    void cname::doIt(MfNode* that,MfNode* dir,\
                        BpString* name,MfMode* mode,BOOLEAN printIt) \
    { that->mname(dir,name,mode,printIt); } \
    static cname  cname##_instance; \
    static cname* mname##_method=&(cname##_instance);


DECL_NODEACTION(     MfCopyToDosAction,    copyToDos)
DECL_NODEACTION(    MfCopyToUnixAction,    copyToUnix)
DECL_NODEACTION( MfHardLinkToDosAction,hardLinkToDos)
DECL_NODEACTION(MfHardLinkToUnixAction,hardLinkToUnix)
DECL_NODEACTION(  MfSymLinkToDosAction, symLinkToDos)
DECL_NODEACTION( MfSymLinkToUnixAction, symLinkToUnix)





// birth and death:

    CONSTRUCTOR(MfDirectory)
    {
        BpClass_PLUG(MfDirectory);
        fFiles=NEW(BpList);
        fFiles->retain();
        renamer=NEW(Renamer);
        renamer->retain();
        fFileSystem=MfDirectory_FileSystem_UNIX;
    }//MfDirectory;
        
    
    DESTRUCTOR(MfDirectory)
    {
        fFiles->release();
        renamer->release();
    }//~MfDirectory;


// override of BpObject methods:

    METHOD(MfDirectory,makeBrother,(void),BpObject*)
    {
        return(NEW(MfDirectory));
    }//makeBrother;


    METHOD(MfDirectory,printOnLevel,(FILE* pFile,CARD32 level),void)
    {
        MfDirectory_SUPER::printOnLevel(pFile,level);
        PRINTONLEVEL_OPEN(pFile,level,fFiles);
        fFiles->printOnLevel(pFile,level+4);
        PRINTONLEVEL_CLOSE(pFile,level);
    }//printOnLevel;

    
// override of MfNode methods:

    METHOD(MfDirectory,contentSet,(CARD32 index,MfNode* nContent),void)
    {
            MfNode*     node;
        if(index<contentCard()){
            node=content(index);
            if(!(node->isDirectory())){
                fFiles->removeObject(node);
            }
        }
        MfDirectory_SUPER::contentSet(index,nContent);
        if(!(nContent->isDirectory())){
            fFiles->addObject(nContent);
        }
    }//contentSet;
    

    METHOD(MfDirectory,contentRemove,(CARD32 index),MfNode*)
    {
            MfNode*     node;
        node=MfDirectory_SUPER::contentRemove(index);
        if(!(node->isDirectory())){
            fFiles->removeObject(node);
        }
        return(node);
    }//contentRemove;
    


    METHOD(MfDirectory,create,(void),BOOLEAN)
    {
        return(0==mkdir(pathName()->string(),07777&(fMode->mode())));
    }//create;


    METHOD(MfDirectory,open,(const char* omode),BOOLEAN)
    {
        return(FALSE);
    }//open;
    
    
    METHOD(MfDirectory,read,(void* buffer,INT32 bufSize),INT32)
    {
        return(-1);
    }//read;
    
    
    METHOD(MfDirectory,write,(void* buffer,INT32 bufSize),INT32)
    {
        return(-1);
    }//write;
    
    
    METHOD(MfDirectory,seek,(INT32 position),INT32)
    {
        return(-1);
    }//seek;
    
    
    METHOD(MfDirectory,eof,(void),BOOLEAN)
    {
        return(TRUE);
    }//eof;
    
    
    METHOD(MfDirectory,close,(void),void)
    {
    }//close;
    
    

// MfDirectory methods:

    METHOD(MfDirectory,loadFiles,(void),void)
    {
            struct direct**     namelist;
            BpString*           fPath;
            BpString*           entryPath;
            BpString*           entryName;
            int                 entrycount;
            int                 i;
            struct stat         status;
            MfDirectory*        dir;
            MfFile*             fil;
            MfMode*             lMode;
BpString*path=NEW(BpString);
        
        entryPath=NEW(BpString);
        entryName=NEW(BpString);
        fPath=pathName();
        entrycount=scandir(fPath->string(),&namelist,NULL,alphasort);
        if(entrycount>0){
            for(i=0;i<entrycount;i++){
                if((strcmp(namelist[i]->d_name,".")!=0)
                &&(strcmp(namelist[i]->d_name,"..")!=0)){
                    entryName->setString(namelist[i]->d_name);
                    entryPath->setString(fPath->string());
                    entryPath->appendString("/");
                    entryPath->append(entryName);
                    if(lstat(entryPath->string(),&status)<0){
                        perrorf("stat(%s)",entryPath->string());
                    }else{
                        switch(status.st_mode&S_IFMT){
                        case S_IFDIR:
                            dir=this->newDirectory();
                            dir->nameSet(entryName);
                            dir->holderSet(0,this);
                            lMode=NEW(MfMode);
                            lMode->modeSet(status.st_mode);
                            dir->modeSet(lMode);
                            contentSet(contentCard(),dir);
dir->pathNameGet(path);
fprintf(stderr,"\r%c[K%s\r",0x1b,path->string());
                            dir->loadFiles();
                            break;  
                        case S_IFREG:
                            fil=this->newFile();
                            fil->nameSet(entryName);
                            fil->holderSet(0,this);
                            lMode=NEW(MfMode);
                            lMode->modeSet(status.st_mode);
                            fil->modeSet(lMode);
                            contentSet(contentCard(),fil);
fil->pathNameGet(path);
fprintf(stderr,"\r%c[K%s\r",0x1b,path->string());
                            break;
                        case S_IFLNK:
                            fil=this->newLink();
                            fil->nameSet(entryName);
                            fil->holderSet(0,this);
                            lMode=NEW(MfMode);
                            lMode->modeSet(status.st_mode);
                            fil->modeSet(lMode);
                            contentSet(contentCard(),fil);
fil->pathNameGet(path);
fprintf(stderr,"\r%c[K%s\r",0x1b,path->string());
                        default:
                            // ignore.
                            break;
                        }//switch
                    }
                }// else ignore '.' or '..'.
                free(namelist[i]);
            }
            free(namelist);
        }else{
            perrorf("scandir(%s)",fPath->string());
        }
    }//loadFiles;
    

    METHOD(MfDirectory,newDirectory,(void),MfDirectory*)
    {
            MfDirectory*    newDir=NEW(MfDirectory);
        newDir->fileSystemSet(fFileSystem);
        newDir->traceSet(fTrace);
        return(newDir);
    }//MfDirectory;
    
    
    METHOD(MfDirectory,newFile,(void),MfFile*)
    {
        MfFile* result=NEW(MfFile);
        // result->fileSystemSet(fFileSystem);
        result->traceSet(fTrace);
        return(result);
    }//newFile;
    

    METHOD(MfDirectory,newLink,(void),MfLink*)
    {
        MfLink* result=NEW(MfLink);
        // result->fileSystemSet(fFileSystem);
        result->traceSet(fTrace);
        return(result);
    }//newLink;
    


    METHOD(MfDirectory,fileCard,(void),CARD32)
    {
        return(fFiles->count());
    }//fileCard;
    

    METHOD(MfDirectory,file,(CARD32 index),MfFile*)
    {
        return((MfFile*)(fFiles->objectAt(index)));
    }//file;
    

    
    METHOD(MfDirectory,makeFilesPerformWith,
                        (BpList_ActionWithPr action,BpObject* anObject),void)
    {
        fFiles->makeObjectsPerformWith(action,anObject);
    }//makeFilesPerformWith;
    

    METHOD(MfDirectory,fileSystem,(void),MfDirectory_FileSystemT)
    {
        return(fFileSystem);
    }//fileSystem;
    
    
    METHOD(MfDirectory,fileSystemSet,(MfDirectory_FileSystemT nFileSystem),void)
    {
        fFileSystem=nFileSystem;
    }//fileSystemSet;
    
    
    METHOD(MfDirectory,renamerSet,(Renamer* nRenamer),void)
    {
        renamer->release();
        renamer=nRenamer;
        renamer->retain();
    }//renamerSet;
    
    
    METHOD(MfDirectory,loadRenamerCache,(void),void)
    {
            BpString*           renamerName;
            FILE*               renamerFile;
            
        switch(fFileSystem){
        case MfDirectory_FileSystem_UNIX:
            renamerName=pathName();
            renamerName->appendString("/.msdosnames");
            renamerFile=fopen(renamerName->string(),"r");
//          if(renamerFile==NIL){
//              fprintf(stderr,"MfDirectory::loadRenamerCache cannot open %s for reading.\n",renamerName->string());
//          }
            renamer->loadUnixCache(renamerFile);
            if(renamerFile!=NULL){
                fclose(renamerFile);
            }
            break;
        case MfDirectory_FileSystem_MSDOS:
            renamerName=pathName();
            renamerName->appendString("/msdosnam");
            renamerFile=fopen(renamerName->string(),"r");
//          if(renamerFile==NIL){
//              fprintf(stderr,"MfDirectory::loadRenamerCache cannot open %s for reading.\n",renamerName->string());
//          }
            renamer->loadDosCache(renamerFile);
            if(renamerFile!=NULL){
                fclose(renamerFile);
            }
            break;
        default:
            fprintf(stderr,"MfDirectory::loadRenamerCache unknown file system.\n");
            break;
        }
    }//loadRenamerCache;
    

    METHOD(MfDirectory,saveRenamerCache,(void),void)
    {
            BpString*           renamerName;
            FILE*               renamerFile;
            
        renamerName=pathName();
        switch(fFileSystem){
        case MfDirectory_FileSystem_UNIX:
            renamerName->appendString("/.msdosnames");
            renamerFile=fopen(renamerName->string(),"w");
            if(renamerFile==NIL){
                fprintf(stderr,"MfDirectory::saveRenamerCache cannot open %s for writting.\n",renamerName->string());
            }else{
                renamer->saveUnixCache(renamerFile);
                fclose(renamerFile);
            }
            break;
        case MfDirectory_FileSystem_MSDOS:
            renamerName->appendString("/msdosnam");
            renamerFile=fopen(renamerName->string(),"w");
            if(renamerFile==NIL){
                fprintf(stderr,"MfDirectory::saveRenamerCache cannot open %s for writting.\n",renamerName->string());
            }else{
                renamer->saveDosCache(renamerFile);
                fclose(renamerFile);
            }
            break;
        default:
            break;
        }
    }//saveRenamerCache;
    

    METHOD(MfDirectory,printRenamerCache,(void),void)
    {
        BpString* renamerName=pathName();
        switch(fFileSystem){
        case MfDirectory_FileSystem_UNIX:
            renamerName->appendString("/.msdosnames");
            printf("cat >%s <<EOF\n",
                   renamerName->shellQuoted()->string());
            renamer->saveUnixCache(stdout);
            break;
        case MfDirectory_FileSystem_MSDOS:
            renamerName->appendString("/msdosnam");
            printf("cat >%s <<EOF\n",
                   renamerName->shellQuoted()->string());
            renamer->saveDosCache(stdout);
            break;
        default:
            printf("Unknown file system type (%d)\n",fFileSystem);
            break;
        }
        printf("EOF\n");
    }//printRenamerCache;
    

    METHOD(MfDirectory,actionToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt,
            MfNodeAction* action),void)
    {
        MfDirectory*        newCopy;
        MfNode*             node;
        BpString*           newName;
        INT32               i;
        INT32               c;
            
        loadRenamerCache();
        newCopy=newDirectory();
        newCopy->holderSet(0,dDir);
        newCopy->nameSet(dName);
        newCopy->modeSet(dMode);
        newCopy->renamerSet(renamer);
        newCopy->fileSystemSet(MfDirectory_FileSystem_MSDOS);
        if(printIt){
            printf("mkdir      %s\n",
                   newCopy->pathName()->shellQuoted()->string());
        }else{
            newCopy->create();
            if(!newCopy->exists()){
                fprintf(stderr,"MfDirectory::copyToDos cannot create the "
                               "copy (%s).\n",dName->string());
                return;
           }
        }
        newName=NEW(BpString);
        c=contentCard();
        for(i=0;i<c;i++){
            node=content(i);
            renamer->setUnixName(node->name()->string(),node->mode());
            newName->setString(renamer->dosName());
            if(fTrace){
                fprintf(stderr,"fromDir=%-20s sourceFile=%-20s "
                                "toDir=%-20s destFile=%-20s destMode=%o\n",
                                pathName()->string(),
                                node->name()->string(),
                                newCopy->pathName()->string(),
                                renamer->dosName(),
                                renamer->mode()->mode());
            }
            action->doIt(node,newCopy,newName,renamer->mode(),printIt);
        }
        if(printIt){
            newCopy->printRenamerCache();
            printf("chmod %04o %s\n",07777&(dMode->mode()),
                   newCopy->pathName()->shellQuoted()->string());
        }else{
            newCopy->saveRenamerCache();
        }
    }//actionToDos;
    
    
    METHOD(MfDirectory,actionToUnix,
           (MfNode* uDir,BpString* uName,MfMode* uMode,BOOLEAN printIt,
            MfNodeAction* action),void)
    {
        MfDirectory*        newCopy;
        MfNode*             node;
        BpString*           newName;
        INT32               i;
        INT32               c;
            
        loadRenamerCache();
        newCopy=newDirectory();
        newCopy->holderSet(0,uDir);
        newCopy->nameSet(uName);
        newCopy->renamerSet(renamer);
        newCopy->fileSystemSet(MfDirectory_FileSystem_UNIX);
        if(printIt){
            printf("mkdir      %s\n",
                   newCopy->pathName()->shellQuoted()->string());
        }else{
            newCopy->create();
            if(!(newCopy->exists())){
                fprintf(stderr,"MfDirectory::copyToUnix cannot create the "
                               "copy (%s).\n",uName->string());
                return;
            }
        }
        newName=NEW(BpString);
        c=contentCard();
        for(i=0;i<c;i++){
            node=this->content(i);
            renamer->setDosName(node->name()->string());
            newName->setString(renamer->unixName());
            if(fTrace){
                fprintf(stderr,"fromDir=%-20s sourceFile=%-20s "
                               "toDir=%-20s destFile=%-20s destMode=%o\n",
                               pathName()->string(),
                               node->name()->string(),
                               newCopy->pathName()->string(),
                               renamer->unixName(),
                               renamer->mode()->mode());
            }
            action->doIt(node,newCopy,newName,renamer->mode(),printIt);
        }
        if(printIt){
            newCopy->printRenamerCache();
            newCopy->modeSet(uMode);
            printf("chmod %04o %s\n",07777&(uMode->mode()),
                   newCopy->pathName()->shellQuoted()->string());
        }else{
            newCopy->saveRenamerCache();
            newCopy->modeSet(uMode);
        }
    }//actionToUnix;


    METHOD(MfDirectory,copyToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)
    {
        this->actionToDos(dDir,dName,dMode,printIt,copyToDos_method);
    }//copyToDos;


    METHOD(MfDirectory,copyToUnix,
           (MfNode* uDir,BpString* uName,MfMode* uMode,BOOLEAN printIt),void)
    {
        this->actionToUnix(uDir,uName,uMode,printIt,copyToUnix_method);
    }//copyToUnix;


    METHOD(MfDirectory,symLinkToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)
    {
        this->actionToDos(dDir,dName,dMode,printIt,symLinkToDos_method);
    }//symLinkToDos;


    METHOD(MfDirectory,symLinkToUnix,
           (MfNode* uDir,BpString* uName,MfMode* uMode,BOOLEAN printIt),void)
    {
        this->actionToUnix(uDir,uName,uMode,printIt,symLinkToUnix_method);
    }//symLinkToUnix;


    METHOD(MfDirectory,hardLinkToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)
    {
        this->actionToDos(dDir,dName,dMode,printIt,hardLinkToDos_method);
    }//hardLinkToDos;


    METHOD(MfDirectory,hardLinkToUnix,
           (MfNode* uDir,BpString* uName,MfMode* uMode,BOOLEAN printIt),void)
    {
        this->actionToUnix(uDir,uName,uMode,printIt,hardLinkToUnix_method);
    }//hardLinkToUnix;



//END MfDirectory.

/*** MfDirectory.cc                   --                     --          ***/

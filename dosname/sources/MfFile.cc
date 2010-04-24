/******************************************************************************
FILE:               MfFile.cc
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the implementation of the MfFile class.
    
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    $Id: MfFile.cc,v 1.1 2003/12/04 03:46:04 pjbpjb Exp $
   Revision 1.1  95/11/19  08:38:12  pascal
   Initial revision
   
    Revision 1.6  95/05/08  02:22:09  pascal
    0003: Ajout d'une methode donnant le nombre de ligne dans le fichier.
    
    Revision 1.5  95/04/18  03:08:29  pascal
    Freezing version WORKING_1.
    
    Revision 1.4  95/04/17  05:03:21  pascal
    Removed StatusInquire (merged it with size()).
    
    Revision 1.3  95/04/17  03:05:36  pascal
    Version optimized for space. Added ident keyword.
    
    Revision 1.2  95/04/17  02:12:48  pascal
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
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
}
#include "names.h"
#include MfFile_hh
#include MfDirectory_hh
#include BpClass_hh
#include BcImplementation_h

    static const char rcsident[]="$Id: MfFile.cc,v 1.1 2003/12/04 03:46:04 pjbpjb Exp $";

#define blockSize   (512)

// birth and death:

    CONSTRUCTOR(MfFile)
    {
        BpClass_PLUG(MfFile);
        fSize=MAX_CARD32;
        fLineCount=MAX_CARD32;
        fBlock=NULL;
    }//MfFile;
        
    
    DESTRUCTOR(MfFile)
    {
    }//~MfFile;


// override of BpObject methods:

    METHOD(MfFile,makeBrother,(void),BpObject*)
    {
        return(NEW(MfFile));
    }//makeBrother;


    METHOD(MfFile,printOnLevel,(FILE* file,CARD32 level),void)
    {
        MfFile_SUPER::printOnLevel(file,level);
        PRINTONLEVEL(file,level,"%lu",fSize,fSize);
    }//printOn;

    
// override of MfNode methods:

    METHOD(MfFile,isDirectory,(void),BOOLEAN)
    {
        return(FALSE);
    }//isDirectory;
    
    

// MfFile methods:

    METHOD(MfFile,size,(void),CARD32)
    {
        if(fSize==MAX_CARD32){
                struct stat     status;
                BpString*       fname=NEW(BpString);
            pathNameGet(fname);
            if(stat(fname->string(),&status)==0){
                fSize=status.st_size;
            }
        }
        return(fSize);
    }//size;
    
        
    METHOD(MfFile,lineCount,(void),CARD32)
    {
            char            command[BUFSIZ];
            FILE*           output;
            BpString*       fname=NEW(BpString);
    
        if(fLineCount==MAX_CARD32){
            pathNameGet(fname);
            sprintf(command,"wc -l %s",fname->shellQuoted()->string());
            output=popen(command,"r");
            if(output==NULL){
                return(MAX_CARD32);
            }else{
                if(1==fscanf(output,"%lu",&fLineCount)){
                    pclose(output);
                    return(fLineCount);
                }else{
                    pclose(output);
                    return(MAX_CARD32);
                }
            }
        }else{
            return(fLineCount);
        }
    }//lineCount;
    

    METHOD(MfFile,compare,(MfFile* otherFile),CARD32)
    {
            char            command[BUFSIZ];
            FILE*           output;
            CARD32          result;
            BpString*       fname=NEW(BpString);
            BpString*       oname=NEW(BpString);
            
        pathNameGet(fname);
        otherFile->pathNameGet(oname);
        sprintf(command,"diff -B -n %s %s | ./countdiffed",
                fname->shellQuoted()->string(),
                oname->shellQuoted()->string());
        output=popen(command,"r");
        if(output==NULL){
            return(MAX_CARD32);
        }else{
            if(1==fscanf(output,"%lu",&result)){
                pclose(output);
                return(result);
            }else{
                pclose(output);
                return(MAX_CARD32);
            }
        }
    }//compare;
    
    
    METHOD(MfFile,loadFirstBlock,(void),void)
    {
            CARD32          wordCount;
            BpString*       fname;
            FILE*           file;
            CARD32          readCount;
            
        if(fBlock==NULL){
            wordCount=((size()+sizeof(CARD32)-1)/sizeof(CARD32));
            fBlock=(CARD32*)malloc(wordCount*sizeof(CARD32));
            if(fBlock==NULL){
                perror("loadFirstBlock malloc");
                return;
            }
            fBlock[wordCount-1]=0;
            fname=NEW(BpString);
            pathNameGet(fname);
            file=fopen(fname->string(),"rb");
            if(file==NULL){
                perror("loadFirstBlock fopen");
                free(fBlock);
                fBlock=NULL;
                return;
            }
            readCount=fread((void*)fBlock,sizeof(char),size(),file);
            fclose(file);
            if(readCount!=size()){
                perror("loadFirstBlock fread");
                free(fBlock);
                fBlock=NULL;
                return;
            }
        }
    }//loadFirstBlock;
    
    
    METHOD(MfFile,compareFirstBlock,(MfFile* otherFile),CARD32)
    {
            CARD32      sizeToCompare;
            CARD32      s;
            
        this->loadFirstBlock();
        otherFile->loadFirstBlock();
        
        sizeToCompare=blockSize;
        s=size();
        if(s<sizeToCompare){
            sizeToCompare=s;
        }
        s=otherFile->size();
        if(s<sizeToCompare){
            sizeToCompare=s;
        }
        if(memcmp((void*)fBlock,(void*)(otherFile->fBlock),sizeToCompare)==0){
            if((sizeToCompare==size())&&(sizeToCompare==otherFile->size())){
                return(0);
            }else{
                return(1);
            }
        }else{
            return(2);
        }
    }//compareFirstBlock;
    

    METHOD(MfFile,copyToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)
    {
        MfFile*     theCopy;
    
        theCopy=((MfDirectory*)dDir)->newFile();
        theCopy->holderSet(0,dDir);
        theCopy->nameSet(dName);
        theCopy->modeSet(dMode);
        if(printIt){
            printf("cp %s %s\n",this->pathName()->shellQuoted()->string(),
                   theCopy->pathName()->shellQuoted()->string());
            printf("chmod %04u %s\n",07777&(theCopy->mode()->mode()),
                   theCopy->pathName()->shellQuoted()->string());
        }else{
            //  theCopy->fileSystemSet(MfDirectory_FileSystem_MSDOS);
            theCopy->copyFileFrom(this);
        }
    }//copyToDos;
    
    
    METHOD(MfFile,copyToUnix,
           (MfNode* uDir,BpString* uName,MfMode* uMode,BOOLEAN printIt),void)
    {
        MfFile*     theCopy;
        
        /*
            A MS-DOS file may contain either the data of a UNIX file,
            or the linked path of a UNIX symbolic link.
        */
        if((uMode->mode()&S_IFMT)==S_IFLNK){
            theCopy=((MfDirectory*)uDir)->newLink();
        }else{
            theCopy=((MfDirectory*)uDir)->newFile();
        }
        theCopy->holderSet(0,uDir);
        theCopy->nameSet(uName);
        theCopy->modeSet(uMode);
    //  theCopy->fileSystemSet(MfDirectory_FileSystem_UNIX);
        if(printIt){
            if((uMode->mode()&S_IFMT)==S_IFLNK){
                printf("ln -s %s %s\n",
                       this->textContents()->shellQuoted()->string(),
                       theCopy->pathName()->shellQuoted()->string());
            }else{
                printf("cp %s %s\n",
                       this->pathName()->shellQuoted()->string(),
                       theCopy->pathName()->shellQuoted()->string());
            }
        }else{
            theCopy->copyFileFrom(this);
        }
    }//copyToUnix;
    
    
    METHOD(MfFile,symLinkToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)
    {
        MfFile*     theCopy;
    
        theCopy=((MfDirectory*)dDir)->newFile();
        theCopy->holderSet(0,dDir);
        theCopy->nameSet(dName);
        theCopy->modeSet(dMode);
        if(printIt){
            // SEE: What kind of relative path could we use ? 
            //      (This should be an option).
            printf("ln -s %s %s\n",
                   this->pathName()->shellQuoted()->string(),
                   theCopy->pathName()->shellQuoted()->string());
        }else{
            // theCopy->fileSystemSet(MfDirectory_FileSystem_MSDOS);
            if(0!=symlink(this->pathName()->string(),
                          theCopy->pathName()->string())){
                perror("MfFile::symLinkToDos symlink");
            }
        }
    }//symLinkToDos;


    METHOD(MfFile,symLinkToUnix,
           (MfNode* uDir,BpString* uName,MfMode* uMode,BOOLEAN printIt),void)
    {
        MfFile*     theCopy;
        
        /*
            A MS-DOS file may contain either the data of a UNIX file,
            or the linked path of a UNIX symbolic link.
        */
        if((uMode->mode()&S_IFMT)==S_IFLNK){
            theCopy=((MfDirectory*)uDir)->newLink();
        }else{
            theCopy=((MfDirectory*)uDir)->newFile();
        }
        theCopy->holderSet(0,uDir);
        theCopy->nameSet(uName);
        theCopy->modeSet(uMode);
    //  theCopy->fileSystemSet(MfDirectory_FileSystem_UNIX);
        if(printIt){
            if((uMode->mode()&S_IFMT)==S_IFLNK){
                // We rebuild the link, since the DOS file is not a link.
                // Hopefully, it'll be a relative symlink.
                printf("ln -s %s %s\n",
                       this->textContents()->shellQuoted()->string(),
                       theCopy->pathName()->shellQuoted()->string());
            }else{
                // We link to the DOS file.
                printf("ln -s %s %s\n",
                       this->pathName()->shellQuoted()->string(),
                       theCopy->pathName()->shellQuoted()->string());
            }
        }else{
            if((uMode->mode()&S_IFMT)==S_IFLNK){
                // We rebuild the link, since the DOS file is not a link.
                // Hopefully, it'll be a relative symlink.
                if(0!=symlink(this->textContents()->string(),
                              theCopy->pathName()->string())){
                    perror("MfFile::symLinkToUnix symlink");
                }
            }else{
                // We link to the DOS file.
                if(0!=symlink(this->pathName()->string(),
                              theCopy->pathName()->string())){
                    perror("MfFile::symLinkToUnix symlink");
                }
            }
        }
    }//symLinkToUnix;


    METHOD(MfFile,hardLinkToDos,
           (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)
    {
        MfFile*     theCopy;
    
        theCopy=((MfDirectory*)dDir)->newFile();
        theCopy->holderSet(0,dDir);
        theCopy->nameSet(dName);
        theCopy->modeSet(dMode);
        if(printIt){
            printf("ln %s %s\n",
                   this->pathName()->shellQuoted()->string(),
                   theCopy->pathName()->shellQuoted()->string());
        }else{
            // theCopy->fileSystemSet(MfDirectory_FileSystem_MSDOS);
            if(0!=link(this->pathName()->string(),
                          theCopy->pathName()->string())){
                perror("MfFile::hardLinkToDos link");
            }
        }
    }//hardLinkToDos;


    METHOD(MfFile,hardLinkToUnix,
           (MfNode* uDir,BpString* uName,MfMode* uMode,BOOLEAN printIt),void)
    {
        MfFile*     theCopy;
        
        /*
            A MS-DOS file may contain either the data of a UNIX file,
            or the linked path of a UNIX symbolic link.
        */
        if((uMode->mode()&S_IFMT)==S_IFLNK){
            theCopy=((MfDirectory*)uDir)->newLink();
        }else{
            theCopy=((MfDirectory*)uDir)->newFile();
        }
        theCopy->holderSet(0,uDir);
        theCopy->nameSet(uName);
        theCopy->modeSet(uMode);
    //  theCopy->fileSystemSet(MfDirectory_FileSystem_UNIX);
        if(printIt){
            if((uMode->mode()&S_IFMT)==S_IFLNK){
                // We rebuild the link, since the DOS file is not a link.
                // Hopefully, it'll be a relative symlink.
                printf("ln -s %s %s\n",
                       this->textContents()->shellQuoted()->string(),
                       theCopy->pathName()->shellQuoted()->string());
            }else{
                // We link to the DOS file.
                printf("ln %s %s\n",
                       this->pathName()->shellQuoted()->string(),
                       theCopy->pathName()->shellQuoted()->string());
            }
        }else{
             if((uMode->mode()&S_IFMT)==S_IFLNK){
                // We rebuild the sym link, since the DOS file is not a link.
                // Hopefully, it'll be a relative symlink.
                if(0!=symlink(this->textContents()->string(),
                              theCopy->pathName()->string())){
                    perror("MfFile::hardLinkToUnix symlink");
                }
            }else{
                // We link to the DOS file.
                if(0!=link(this->pathName()->string(),
                           theCopy->pathName()->string())){
                    perror("MfFile::hardLinkToUnix link");
                }
            }
        }      
    }//hardLinkToUnix;



    METHOD(MfFile,copyFileFrom,(MfFile* source),void)
    {
        char*       buffer;
        char*       writeBuffer;
        INT32       readCount;
        INT32       writeCount;
        INT32       openCount;

        create();
        if(exists()&&(source->size()>0)){
            chmod(pathName()->string(),00666);
            openCount=0;
            if(open("w")){
                openCount++;
            }else{
                fprintf(stderr,
                        "MfFile::copyFileFrom cannot open %s for writting.\n",
                        pathName()->string());
            }
            if(source->open("r")){
                openCount++;
            }else{
                fprintf(stderr,
                        "MfFile::copyFileFrom cannot open %s for reading.\n", 
                        source->pathName()->string());
            }
            if(openCount==2){
                buffer=(char*)BcMem_Allocate(128*1024);
                while(!(source->eof())){
                    readCount=source->read(buffer,128*1024);
                    writeBuffer=buffer;
                    while(readCount>0){
                        writeCount=write(writeBuffer,readCount);
                        if(writeCount>0){
                            writeBuffer+=writeCount;
                            readCount-=writeCount;
                        }
                    }
                }
                BcMem_Deallocate((void**)&buffer);
            }else{
                fprintf(stderr,"Cannot open output (%s) or input (%s) file.\n",
                    pathName()->string(),source->pathName()->string());
            }
            close();
            source->close();
        }
        chmod(pathName()->string(),07777&(fMode->mode()));
    }//copyFileFrom;
    


    METHOD(MfFile,textContents,(void),BpString*)
    {
        BpString*       fname;
        BpString*       contents;
        FILE*           file;
        CARD32          readCount;
        char*           data;
        size_t          fsize=this->size();

        if(fsize==0){
            return(NEW(BpString));
        }
        fname=NEW(BpString);
        pathNameGet(fname);
        file=fopen(fname->string(),"rb");
        if(file==NULL){
            perror("MfFile::textContents fopen");
            return(0);
        }
        data=(char*)malloc((fsize+1)*sizeof(char));
        readCount=fread((void*)data,sizeof(char),fsize,file);
        fclose(file);
        if(readCount!=fsize){
            perror("MfFile::textContents fread");
            free(data);
            return(0);
        }
        data[fsize]='\0';
        contents=NEW(BpString);
        contents->setString(data);
        free(data);
        return(contents);
    }//textContents;
    
//END MfFile.

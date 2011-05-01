/******************************************************************************
FILE:               MfLink.cc
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the implementation of the MfLink class.
    This class blah_blah_blah.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:45:34  pascal
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
extern "C"{
#include <string.h> // memcpy
#ifdef unix
#include <unistd.h>
#else
#include <libc.h>   // readlink,...
#endif
#include <sys/stat.h>
}
#include "names.h"
#include MfLink_hh
#include MfDirectory_hh
#include BpClass_hh
#include BcImplementation_h

static const char rcsid[]="$Id";

// birth and death:

typedef enum {
    State_closed,State_reading,State_writting,State_opened
}               StateT;
        

CONSTRUCTOR(MfLink)
{
    BpClass_PLUG(MfLink);
    state=State_closed;
}//MfLink;
        
    
DESTRUCTOR(MfLink)
{
}//~MfLink;


// override of BpObject methods:

METHOD(MfLink,makeBrother,(void),BpObject*)
{
    return(NEW(MfLink));
}//makeBrother;


METHOD(MfLink,printOnLevel,(FILE* file,CARD32 level),void)
{
    MfLink_SUPER::printOnLevel(file,level);
    //  PRINTONLEVEL(file,level,"%s",NEWFIELD,NEWFIELD);
}//printOnLevel;

    
// override of MfNode methods:


METHOD(MfLink,exists,(void),BOOLEAN)
{
    return(TRUE);
}//exists;
    
    
METHOD(MfLink,create,(void),BOOLEAN)
{
    state=State_closed;
    return(TRUE);
}//create;
    
    
METHOD(MfLink,open,(const char* omode),BOOLEAN)
{
    switch(*omode){
    case 'r':
        state=State_reading;
        fEof=(INT32)readlink(pathName()->string(),linkbuf,BUFSIZ);
        if(fEof<0){
            fEof=0;
        }
        break;
    case 'w':
        state=State_writting;
        fEof=0;
        break;
    default:
        return(FALSE);
    }
    position=0;
    fSize=fEof;
    return(TRUE);
}//open;
    
    
METHOD(MfLink,read,(void* buffer,INT32 bufSize),INT32)
{
    if(state==State_reading){
        if(bufSize>fEof-position){
            bufSize=fEof-position;
        }
        memcpy(buffer,linkbuf+position,bufSize);
        position+=bufSize;
        return(bufSize);
    }else{
        return(0);
    }
}//read;
    
    
METHOD(MfLink,write,(void* buffer,INT32 bufSize),INT32)
{
    if(state==State_writting){
        if(bufSize>BUFSIZ-1-position){
            bufSize=BUFSIZ-1-position;
        }
        memcpy(linkbuf+position,buffer,bufSize);
        position+=bufSize;
        fEof=position;
        fSize=fEof;
        return(bufSize);
    }else{
        return(0);
    }
}//write;
    
    
METHOD(MfLink,seek,(INT32 newPosition),INT32)
{
    return(-1);
}//seek;
    
    
METHOD(MfLink,eof,(void),BOOLEAN)
{
    if(state==State_reading){
        return(fEof==position);
    }else{
        return(TRUE);
    }
}//eof;
    
    
METHOD(MfLink,close,(void),void)
{
    if(state==State_writting){
        linkbuf[fEof]='\0';
        while((fEof>0)
              &&((linkbuf[fEof-1]=='\n')||(linkbuf[fEof-1]=='\r'))){
            fEof--;
        }
        linkbuf[fEof]='\0';
        if(fEof>0){
            unlink(pathName()->string());
            int r=symlink(linkbuf,pathName()->string());
            (void)r;
        }
    }
    state=State_closed;
}//close;
    
    
METHOD(MfLink,size,(void),CARD32)
{
    if(fSize==MAX_CARD32){
        struct stat     status;
        BpString*       fname=NEW(BpString);
        pathNameGet(fname);
        if(lstat(fname->string(),&status)==0){
            fSize=(CARD32)status.st_size;
        }
    }
    return(fSize);
}//size;
    
    
METHOD(MfLink,copyToDos,
       (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)
{
    MfFile*     theCopy;
    
    if(!(dDir->isKindOfClassNamed("MfDirectory"))){
        fprintf(stderr,"dDir is not a MfDirectory in MfLink::copyToDos, "
                "but is %s!\n",dDir->className());
        return;
    }
    theCopy=((MfDirectory*)dDir)->newFile();
    theCopy->holderSet(0,dDir);
    theCopy->nameSet(dName);
    theCopy->modeSet(dMode);
    if(printIt){
        printf("echo %s >  %s\n",
               this->pathName()->shellQuoted()->string(),
               theCopy->pathName()->shellQuoted()->string());
        printf("chmod %04u %s\n",theCopy->mode()->mode(),
               theCopy->pathName()->shellQuoted()->string());
    }else{
        theCopy->copyFileFrom(this);
    }
}//copyToDos;
    
    

METHOD(MfLink,copyToUnix,
       (MfNode* uDir,BpString* uName,MfMode* uMode,BOOLEAN printIt),void)
{
    MfLink*     theCopy;
    
    if(!(uDir->isKindOfClassNamed("MfDirectory"))){
        fprintf(stderr,"uDir is not a MfDirectory in MfLink::copyToDos, "
                "but is %s!\n",uDir->className());
        return;
    }
    theCopy=((MfDirectory*)uDir)->newLink();
    theCopy->holderSet(0,uDir);
    theCopy->nameSet(uName);
    theCopy->modeSet(uMode);
    if(printIt){
        printf("cp %s %s\n",this->pathName()->shellQuoted()->string(),
               theCopy->pathName()->shellQuoted()->string());
    }else{
        theCopy->copyFileFrom(this);
    }
}//copyToUnix;



METHOD(MfLink,symLinkToDos,
       (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)
{
    MfLink_SUPER::symLinkToDos(dDir,dName,dMode,printIt);
}//symLinkToDos;


METHOD(MfLink,symLinkToUnix,
       (MfNode* uDir,BpString* uName,MfMode* uMode,BOOLEAN printIt),void)
{
    MfLink_SUPER::symLinkToUnix(uDir,uName,uMode,printIt);
}//symLinkToUnix;


METHOD(MfLink,hardLinkToDos,
       (MfNode* dDir,BpString* dName,MfMode* dMode,BOOLEAN printIt),void)
{
    MfLink_SUPER::hardLinkToDos(dDir,dName,dMode,printIt);
}//hardLinkToDos;


METHOD(MfLink,hardLinkToUnix,
       (MfNode* uDir,BpString* uName,MfMode* uMode,BOOLEAN printIt),void)
{
    MfLink_SUPER::hardLinkToUnix(uDir,uName,uMode,printIt);
}//hardLinkToUnix;


//// THE END ////

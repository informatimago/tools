/******************************************************************************
FILE:               Renamer.cc
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the implementation of the Renamer class.
    This class blah_blah_blah.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:43:13  pascal
   Initial revision
   
    1995-11-05 <PJB> Creation. 
LEGAL
    Copyright Pascal J. Bourguignon 1995 - 2001

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
#include <ctype.h>
#include <string.h>
}
#include "names.h"
#include Renamer_hh
#include NameSeq_hh
#include AttrString_hh
#include BpClass_hh
#include BcImplementation_h

    static const char rcsid[]="$Id: Renamer.cc,v 1.1 2003/12/04 03:46:04 pjbpjb Exp $";
    
    
    CONSTRUCTOR(Renamer)
    {
        BpClass_PLUG(Renamer);
        curUnixName=NIL;
        curDosName=NIL;
        curMode=NIL;
        unixToDos=NEW(BpDict);
        unixToDos->retain();
        dosToUnix=NEW(BpDict);
        dosToUnix->retain();
    }//Renamer;
    
    
    DESTRUCTOR(Renamer)
    {
        if(curUnixName!=NIL){
            free(curUnixName);
        }
        if(curDosName!=NIL){
            free(curDosName);
        }
        if(curMode!=NIL){
            curMode->release();
        }
        unixToDos->release();
        dosToUnix->release();
    }//~Renamer;
    
    
    METHOD(Renamer,makeBrother,(void),BpObject*)
    {
        return(NEW(Renamer));
    }//makeBrother;
    
    
    METHOD(Renamer,printOnLevel,(FILE* file,CARD32 level),void)
    {
        Renamer_SUPER::printOnLevel(file,level);
        PRINTONLEVEL(file,level,"%s",curUnixName,NULLSTR(curUnixName));
        PRINTONLEVEL(file,level,"%p",curMode,curMode);
        if(curMode!=NIL){
            PRINTONLEVEL_OPEN(file,level, curMode);
            curMode->printOnLevel(file,level+1);
            PRINTONLEVEL_CLOSE(file,level);
        }
        PRINTONLEVEL(file,level,"%s",curDosName,NULLSTR(curDosName));
        PRINTONLEVEL(file,level,"%p",dosToUnix,dosToUnix);
        if(dosToUnix!=NIL){
            PRINTONLEVEL_OPEN(file,level,dosToUnix);
            dosToUnix->printOnLevel(file,level+1);
            PRINTONLEVEL_CLOSE(file,level);
        }
        PRINTONLEVEL(file,level,"%p",unixToDos,unixToDos);
        if(unixToDos!=NIL){
            PRINTONLEVEL_OPEN(file,level,unixToDos);
            unixToDos->printOnLevel(file,level+1);
            PRINTONLEVEL_CLOSE(file,level);
        }
    }//printOnLevel;
    
    
/*
    .           .
    ..          ..
    ...         ___
    .x          _x
    ..x         __x
    ...x        ___x
    x.          x.
    x..         x__
    x.x         x.x
    x..x        x__x
    x...x       x___x
*/

    METHOD(Renamer,processDots,(char* processedUN),void)
    {
            INT32       i;
            
        i=0;
        if(curUnixName[i]=='.'){
            if((curUnixName[1]=='\0')
            ||((curUnixName[1]=='.')&&(curUnixName[2]=='\0'))){
                /* "." or ".." */
                while(curUnixName[i]!='\0'){
                    processedUN[i]='.';
                    i++;
                }
            }else{
                /* curUnixName[1]!='\0' */
                while(curUnixName[i]=='.'){
                    processedUN[i]='_';
                    i++;
                }
            }
        }
        while(curUnixName[i]!='\0'){
            while((curUnixName[i]!='.')&&(curUnixName[i]!='\0')){
                processedUN[i]=curUnixName[i];
                i++;
            }
            if(curUnixName[i]=='.'){
                if(curUnixName[i+1]=='.'){
                    while(curUnixName[i]=='.'){
                        processedUN[i]='_';
                        i++;
                    }
                }else{
                    processedUN[i]=curUnixName[i];
                    i++;
                }
            }
        }
        processedUN[i]='\0';
    }//processDots;


    METHOD(Renamer,setUnixName,(const char* nUN,MfMode* nMode),void)
    {
        if(curUnixName!=NIL){
            free(curUnixName);
        }
        curUnixName=newstr(nUN);
        if(curMode!=NIL){
            curMode->release();
        }
        curMode=nMode;
        curMode->retain();
        if(curDosName!=NIL){
            free(curDosName);
            curDosName=NIL;
        }
    }//setUnixName;
    
    
    METHOD(Renamer,dosName,(void),const char*)
    {
        if(curDosName==NIL){
            computeDosName();
        }
        return(curDosName);
    }//dosName;


    METHOD(Renamer,setDosName,(const char* nDN),void)
    {
        if(curDosName!=NIL){
            free(curDosName);
        }
        curDosName=newstr(nDN);
        if(curUnixName!=NIL){
            free(curUnixName);
            curUnixName=NIL;
        }
        if(curMode!=NIL){
            curMode->release();
            curMode=NIL;
        }
    }//setDosName;
    
    
    METHOD(Renamer,unixName,(void),const char*)
    {
        if(curUnixName==NIL){
            computeUnixName();
        }
        return(curUnixName);
    }//unixName;
    
    
    METHOD(Renamer,mode,(void),MfMode*)
    {
        if(curMode==NIL){
            computeUnixName();
        }
        return(curMode);
    }//mode;
    
    

/*
    The mode is always attached to the value string of the dictionaries
    (because BpDict makes its own copies of the keys).
    computeDosName takes the mode from curMode, which has be set from the
    setUnixName parameter. It puts the mode into the saved 'cache'.
    computeUnixMame takes the mode from the dosToUnix dictionary, loaded
    from the msdosnam file. It puts the mode to the curMode, which will be
    retrieved by mode().
    
*/


    METHOD(Renamer,computeUnixName,(void),void)
    {
        AttrString* aDosName;
        AttrString* aUnixName;
        
        if(curUnixName!=NIL){
            free(curUnixName);
        }
        if(curMode!=NIL){
            curMode->release();
        }
        aDosName=NEW(AttrString);
        aDosName->setString(curDosName);
        aUnixName=(AttrString*)(dosToUnix->objectForKey(aDosName));
        if(aUnixName!=NIL){
            curUnixName=newstr(aUnixName->string());
            curMode=aUnixName->mode();
        }else{
        //SEE: the CurDosName could be already used by a unix file.
            curUnixName=newstr(curDosName);
            aUnixName=NEW(AttrString);
            aUnixName->setString(curUnixName);
            curMode=NEW(MfMode);
            curMode->modeSet(00777);
        }
        curMode->retain();
        aDosName->setMode(curMode);
        unixToDos->setObjectForKey(aDosName,aUnixName);
    }//computeUnixName;
    
    
    METHOD(Renamer,computeDosName,(void),void)
        /*
            PRE:        curDosName=NIL.
            POST:       curDosName!=NIL. curDosName contains a DOS name.
        */
    {
        INT32       i;
        char*       processedUN;
        NameSeq*    names;
        AttrString* aUnixName;
        AttrString* aDosName;
        INT32       n;
        char*       p;
        
        aUnixName=NEW(AttrString);
        aUnixName->setString(curUnixName);
        aDosName=(AttrString*)(unixToDos->objectForKey(aUnixName));
        if(aDosName!=NIL){
            curDosName=newstr(aDosName->string());
            aUnixName->setMode(curMode);
            dosToUnix->setObjectForKey(aUnixName,aDosName);
        }else{
        
    //***************************************
    //*** BEGIN PROCESS UNX NAME TO DOS NAME
    
            processedUN=(char*)malloc(strlen(curUnixName)+1);
            processDots(processedUN);
            for(i=0;processedUN[i]!='\0';i++){
                switch(processedUN[i]){
                case '*':
                case '?':
                case '\\':
                case '/':
                    processedUN[i]='_';
                    break;
                default:
                    break;
                }
            }
            if(processedUN[0]=='.'){
                /* "." or ".." */
                strcpy(curDosName,processedUN);
                // no entry in the cache.
            }else{
                names=NEW(NameSeq);
                names->retain();
                
                i=0;names->analyze(processedUN,&i);
                if(curUnixName[i]!='\0'){
                    fprintf(stderr,"Warning: computeDosName: unix name cannot be analysed entirely\n\t(%s); remaining: (%s)\n",curUnixName,curUnixName+i);
                }
                
                names->reduce();
                curDosName=(char*)malloc(8+1+3+1);
                i=0;names->catenate(curDosName,&i);
                names->release();
                
                for(i=0;curDosName[i]!='\0';i++){
                    if(isupper(curDosName[i])){
                        curDosName[i]=tolower(curDosName[i]);
                    }
                }
                
    //*** END PROCESS UNX NAME TO DOS NAME
    //***************************************

                aDosName=NEW(AttrString);
                aDosName->setString(curDosName);

    //***************************************
    //*** BEGIN PROCESS DOUBLE NAMES

                if(dosToUnix->objectForKey(aDosName)!=NULL){
                    n=0;
                    p=strchr(curDosName,'.');
                    if(p==NULL){
                        p=curDosName+strlen(curDosName);
                        (*p)='.';
                    }
                    p++;
                    while((dosToUnix->objectForKey(aDosName)!=NULL)&&(n<1000)){
                        sprintf(p,"%03ld",n);
                        n++;
                        aDosName->setString(curDosName);
                    }
                    if(n==1000){
                        fprintf(stderr,"Renamer::computeDosName: please be serrious: I'm getting more than a \n   thousand unix names collapsing to the same DOS name!\n");
                        exit(2);
                    }
                }
                
    //*** END PROCESS DOUBLE NAMES
    //***************************************

                aUnixName->setMode(curMode);
                dosToUnix->setObjectForKey(aUnixName,aDosName);
            }
            free(processedUN);
        }
    }//computeDosName;


    PROCEDURE(addIfAbsent,(BpDict* cache,CARD16 nMode,
                            const char* nKey,const char* nValue),void)
    {
            AttrString*     key=NEW(AttrString);
        key->setString(nKey);
        if(cache->objectForKey(key)==NULL){
                AttrString*     value=NEW(AttrString);
                MfMode*         mode=NEW(MfMode);
            value->setString(nValue);
            mode->modeSet(nMode);
            value->setMode(mode);
            cache->setObjectForKey(value,key);
        }
    }//addIfAbsent;
    
    
    METHOD(Renamer,loadUnixCache,(FILE* fCache),void)
    {
        dosToUnix->removeAllObjects();
        unixToDos->removeAllObjects();
        if(fCache!=NULL){
            loadCache(fCache,unixToDos,TRUE);
        }
        addIfAbsent(unixToDos,0666,".msdosnames","msdosnam");
    }//loadUnixCache;
    
    
    METHOD(Renamer,saveUnixCache,(FILE* fCache),void)
    {
        if(fCache!=NULL){
            saveCache(fCache,unixToDos,TRUE);
        }
    }//saveUnixCache;
    
    
    METHOD(Renamer,loadDosCache,(FILE* fCache),void)
    {
        unixToDos->removeAllObjects();
        dosToUnix->removeAllObjects();
        if(fCache!=NULL){
            loadCache(fCache,dosToUnix,FALSE);
        }
        addIfAbsent(dosToUnix,0666,"msdosnam",".msdosnames");
    }//loadDosCache;
    
    
    METHOD(Renamer,saveDosCache,(FILE* fCache),void)
    {
        if(fCache!=NULL){
            saveCache(fCache,dosToUnix,FALSE);
        }
    }//saveDosCache;
    
    
    PROCEDURE(fgets_crnl,(char* buffer,size_t size,FILE* file),char*)
    {
            int             nextc;
            size_t          i;
            
        nextc=fgetc(file);
        i=0;
        while((i<size)&&(nextc!=EOF)){
            buffer[i]=(char)nextc;
            i++;
            if((nextc=='\n')||(nextc=='\r')){
                buffer[i]='\0';
                return(buffer);
            }
            nextc=fgetc(file);
        }
        buffer[i]='\0';
        if(nextc!=EOF){
            return(buffer);
        }else{
            return(NULL);
        }
    }//fgets_crnl;
    
    
    METHOD(Renamer,loadCache,(FILE* fCache,BpDict* cache,BOOLEAN unox),void)
    {
            char        buffer[BUFSIZ];
            char*       m;
            char*       d;
            char*       u;
            char*       mt;
            char*       dt;
            char*       ut;
            INT32       i;
            BpString*   key;
            AttrString* value;
            MfMode*     amode;
        
        while(fgets_crnl(buffer,BUFSIZ-1,fCache)){
            ut=strchr(buffer,'\n');
            if(ut==NULL){
                ut=strchr(buffer,'\r');
            }
            if(ut!=NULL){
                (*ut)='\0';
            }

            i=0;
            m=buffer+i;
            while((buffer[i]!=' ')&&(buffer[i]!='\t')&&(buffer[i]!='\0')){
                i++;
            }
            mt=buffer+i;
            while((buffer[i]==' ')||(buffer[i]=='\t')){
                i++;
            }
            
            d=buffer+i;
            while((buffer[i]!=' ')&&(buffer[i]!='\t')&&(buffer[i]!='\0')){
                i++;
            }
            dt=buffer+i;
            while((buffer[i]==' ')||(buffer[i]=='\t')){
                i++;
            }
            
            u=buffer+i;
            while((buffer[i]!=' ')&&(buffer[i]!='\t')&&(buffer[i]!='\0')){
                i++;
            }
            ut=buffer+i;
            
            if((m==mt)||(d==dt)||(u==ut)){
                if(!((m==mt)&&(d==dt)&&(u==ut))){
                    fprintf(stderr,"NameSeq::loadCache: Missing a name on line: '%s' (skipping).\n",buffer);
                }// else the line is empty, so we completely ignore it.
            }else{
                (*mt)='\0';
                (*dt)='\0';
                (*ut)='\0';
                amode=NEW(MfMode);
                amode->stringValueSet(NEW(BpString(m)));
                key=NEW(AttrString);
                value=NEW(AttrString);
                value->setMode(amode);
                if(unox){
                    key->setString(u);
                    value->setString(d);
                }else{
                    key->setString(d);
                    value->setString(u);
                }
                cache->setObjectForKey(value,key);
            }
        }
    }//loadCache;
    
    
    METHOD(Renamer,saveCache,(FILE* fCache,BpDict* cache,BOOLEAN unox),void)
    {
            BpArray*        keys;
            INT32           i;
            INT32           c;
            AttrString*     value;
            BpString*       key;

//this->printOn(stderr);

        keys=cache->allKeys();
        c=keys->size();
        for(i=0;i<c;i++){
            key=(BpString*)(keys->objectAt(i));
            value=(AttrString*)(cache->objectForKey(key));
            if(unox){
                fprintf(fCache,"%-15s %-14s %s\n",
                        value->mode()->stringValue()->string(),
                        value->string(),key->string());
                //For now, write it any way: this allows the user to change
                //easily the dos name, having all the entries in the cache.
                //if(strcmp(key->string(),value->string())!=0){
                //else there is no need to save in cache: all data is in the
                //     unix directory.
                //}
            }else{
                // we write even when key==value for unix modes are not
                // preserved in dos file system.
                fprintf(fCache,"%-15s %-14s %s\n",
                        value->mode()->stringValue()->string(),
                        key->string(),value->string());
            }
        }
    }//saveCache;
    

//END Renamer.

/*** Renamer.cc                       -- 2003-12-01 05:06:10 -- pascal   ***/

/******************************************************************************
FILE:               WordSeq.cc
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the implementation of the WordSeq class.
    This class blah_blah_blah.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:42:34  pascal
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
#include "names.h"
#include WordSeq_hh
#include CapWord_hh
#include BpClass_hh
#include BcImplementation_h

    static const char rcsid[]="$Id: WordSeq.cc,v 1.1 2003/12/04 03:46:04 pjbpjb Exp $";


    
    CONSTRUCTOR(WordSeq)
    {
        BpClass_PLUG(WordSeq);
        words=NEW(BpList);
        words->retain();
    }//WordSeq;
    
    
    DESTRUCTOR(WordSeq)
    {
        words->release();
    }//WordSeq;
    
    
    METHOD(WordSeq,makeBrother,(void),BpObject*)
    {
        return(NEW(WordSeq));
    }//makeBrother;
    
    
    METHOD(WordSeq,printOnLevel,(FILE* file,CARD32 level),void)
    {
        WordSeq_SUPER::printOnLevel(file,level);
        PRINTONLEVEL(file,level,"%p",words,words);
        if(words!=NIL){
            PRINTONLEVEL_OPEN(file,level,words);
            words->printOnLevel(file,level+1);
            PRINTONLEVEL_CLOSE(file,level);
        }
    }//printOnLevel;
    
    
    METHOD(WordSeq,append,(WordSeq* tail),void)
    {
        words->appendList(tail->words);
    }//append;
    
    
    METHOD(WordSeq,analyze,(const char* source,INT32* next),void)
    {
            CapWord*    capWord;
        
        words->freeObjects();
        capWord=NEW(CapWord);
        words->addObject(capWord);
        capWord->analyze(source,next);
        while(BITIN(charKind[(unsigned char)source[*next]],
                    (CARD32)((1<<ck_ini)|(1<<ck_cap)|(1<<ck_low)
                             |(1<<ck_dig)|(1<<ck_end)))){
            capWord=NEW(CapWord);
            words->addObject(capWord);
            capWord->analyze(source,next);
        }
    }//analyze


    METHOD(WordSeq,catenate,(char* buffer,INT32* bsize),void)
    {
            INT32       i;
            INT32       c;
        
        c=words->count();
        for(i=0;i<c;i++){
            ((CapWord*)(words->objectAt(i)))->catenate(buffer,bsize);
        }
    }/*catenate*/
    
    
    PROCEDURE(CapWord_Compare,(BpObject** a,BpObject** b),SignT)
    {
        return(((CapWord*)(*a))->compareLength((CapWord*)(*b)));
    }//CapWord_Compare;
    
    
    METHOD(WordSeq,reduceTo,(INT32 maxSize),void)
    {
            INT32       i;
            INT32       c;
            PressureT   pressure;
            INT32       len;
            INT32       remainder;
            float       ratio;
        
        c=words->count();

        pressure=Pressure_heavy;
        len=length(pressure);
        if(len<=maxSize){
            pressure=Pressure_medium;
            len=length(pressure);
            if(len<=maxSize){
                pressure=Pressure_light;
                len=length(pressure);
                if(len<=maxSize){
                    return;
                }
            }
        }
        
        ratio=((float)len)/((float)maxSize);
        for(i=0;i<c;i++){
            ((CapWord*)(words->objectAt(i)))->reduceBy(pressure,ratio);
        }
        
        remainder=length(Pressure_light)-maxSize;
        if(remainder>0){
                BpList*     sortedWords;
            sortedWords=NEW(BpList);
            for(i=0;i<c;i++){
                sortedWords->addObject(words->objectAt(i));
            }
            sortedWords->sort(CapWord_Compare);
            while(remainder>0){
                for(i=c-1;(i>=0)&&(remainder>0);i--){
                    ((CapWord*)(sortedWords->objectAt(i)))
                                        ->reduceSoft(&remainder);
                }
            }
        }
    }//reduceTo;
    

    METHOD(WordSeq,print,(FILE* file),void)
    {
            INT32       i;
            INT32       c;
        
        c=words->count();
        for(i=0;i<c;i++){
            ((CapWord*)(words->objectAt(i)))->print(file);
        }
    }//print;



    METHOD(WordSeq,length,(PressureT pressure),INT32)
    {
            INT32       len;
            INT32       i;
            INT32       c;
        
        len=0;
        c=words->count();
        for(i=0;i<c;i++){
            len+=((CapWord*)(words->objectAt(i)))->length(pressure);
        }
        return(len);
    }//length;

//END WordSeq.

/*** WordSeq.cc                       -- 2003-11-20 03:39:30 -- pascal   ***/

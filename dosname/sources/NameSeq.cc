/******************************************************************************
FILE:               NameSeq.cc
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the implementation of the NameSeq class.
    This class blah_blah_blah.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:42:55  pascal
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
#include <stdlib.h>
}
#include "names.h"
#include NameSeq_hh
#include WordSeq_hh
#include CapWord_hh
#include BpClass_hh
#include BcImplementation_h

    static const char rcsid[]="$Id: NameSeq.cc,v 1.1 2003/12/04 03:46:04 pjbpjb Exp $";

    CONSTRUCTOR(NameSeq)
    {
        BpClass_PLUG(NameSeq);
        sequences=NEW(BpList);
        sequences->retain();
    }//NameSeq;
    
    
    DESTRUCTOR(NameSeq)
    {
        sequences->release();
    }//~NameSeq;
    
    
    METHOD(NameSeq,makeBrother,(void),BpObject*)
    {
        return(NEW(NameSeq));
    }//makeBrother;
    
    
    METHOD(NameSeq,printOnLevel,(FILE* file,CARD32 level),void)
    {
        NameSeq_SUPER::printOnLevel(file,level);
        PRINTONLEVEL(file,level,"%p",sequences,sequences);
        if(sequences!=NIL){
            PRINTONLEVEL_OPEN(file,level,sequences);
            sequences->printOnLevel(file,level+1);
            PRINTONLEVEL_CLOSE(file,level);
        }
    }//printOnLevel;
    
    
    METHOD(NameSeq,analyze,(const char* source,INT32* next),void)
    {
            WordSeq*        wordseq;
            
        sequences->freeObjects();

        wordseq=NEW(WordSeq);
        sequences->addObject(wordseq);
        wordseq->analyze(source,next);
        while(source[*next]=='.'){
            (*next)++;
            if(BITIN((unsigned)(charKind[(unsigned char)(source[*next])]),
                     (unsigned)((1<<ck_ini)|(1<<ck_cap)
                                |(1<<ck_low)|(1<<ck_dig)|(1<<ck_end)))){
                wordseq=NEW(WordSeq);
                sequences->addObject(wordseq);
                wordseq->analyze(source,next);
            }
        }
    }//analyze;
    
    
    METHOD(NameSeq,catenateHead,(char* buffer,INT32* bsize),void)
    {
            INT32       i;
            INT32       c;
        
        c=sequences->count();
        if(c<=1){
            ((WordSeq*)(sequences->objectAt(0)))->catenate(buffer,bsize);
        }else{
            for(i=0;i<c-1;i++){
                ((WordSeq*)(sequences->objectAt(i)))->catenate(buffer,bsize);
            }
        }
    }/*catenateHead*/
    
    
    METHOD(NameSeq,catenateTail,(char* buffer,INT32* bsize),void)
    {
            INT32       c;
        c=sequences->count();
        if(c>1){
            ((WordSeq*)(sequences->objectAt(c-1)))->catenate(buffer,bsize);
        }
    }/*catenateTail*/
    
    
    METHOD(NameSeq,catenate,(char* buffer,INT32* bsize),void)
    {
            INT32       c;
        
        c=sequences->count();
        catenateHead(buffer,bsize);
        if(c>1){
            buffer[*bsize]='.';
            (*bsize)++;
            catenateTail(buffer,bsize);
        }
    }//catenate;


    METHOD(NameSeq,reduce,(void),void)
    {
            INT32       c;
            
        c=sequences->count();
        if(c>2){
                WordSeq*        firstWordSeq;
            firstWordSeq=((WordSeq*)(sequences->objectAt(0)));
            while(sequences->count()>2){
                firstWordSeq->append((WordSeq*)(sequences->removeObjectAt(1)));
            }
            c=2;
        }
        ((WordSeq*)(sequences->objectAt(0)))->reduceTo(8);
        if(c>1){
            ((WordSeq*)(sequences->objectAt(1)))->reduceTo(3);
        }
        
        if((((WordSeq*)(sequences->objectAt(0)))->length(Pressure_light)>8)
        ||((c>1)
          &&(((WordSeq*)(sequences->objectAt(1)))->length(Pressure_light)>3))){
            fprintf(stderr,"NameSeq::reduce: Post condition not fulfilled: len(basename)>8 or len(extension)>3\n");
            exit(1);
        }
    }/*reduce*/


    METHOD(NameSeq,print,(FILE* file),void)
    {
            INT32       i;
            INT32       c;
        
        ((WordSeq*)(sequences->objectAt(0)))->print(file);
        c=sequences->count();
        for(i=1;i<c;i++){
            printf("--------\n");
            ((WordSeq*)(sequences->objectAt(i)))->print(file);
        }
    }//print;
    



//END NameSeq.

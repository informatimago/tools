/******************************************************************************
FILE:               SeqStream.cc
LANGUAGE:           C++
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This is an abstract class for a sequential stream of bytes.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1993/05/01 <PJB> Creation.
LEGAL
    Copyright Pascal J. Bourguignon 1993 - 1993
    All rights reserved.
    This program may not be included in any commercial product without the 
    author written permission. It may be used freely for any non-commercial 
    purpose, provided that this header is always included.
******************************************************************************/
#include <BcInterface.h>
#include <BcTypes.h>
#include <SeqStream.hh>
#include <BcImplementation.h>


    CONSTRUCTOR(SeqStream)
    {
        operation=SeqStream_Operation_closed;
        theSubstream=NIL;
    }//SeqStream;
    

    DESTRUCTOR(SeqStream)
    {
        if(operation!=SeqStream_Operation_closed){
            this->close();
        }
    }//~SeqStream;
    
    
    METHOD(SeqStream,reset,(SeqStream* nSubstream),SeqStream*)
    {
        if(operation!=SeqStream_Operation_closed){
            this->close();
        }
        operation=SeqStream_Operation_read;
        theSubstream=nSubstream;
        return(this);
    }//reset;
    
        
    METHOD(SeqStream,rewrite,(SeqStream* nSubstream),SeqStream*)
    {
        if(operation!=SeqStream_Operation_closed){
            this->close();
        }
        operation=SeqStream_Operation_write;
        theSubstream=nSubstream;
        return(this);
    }//rewrite;
    
        
    METHOD(SeqStream,close,(void),void)
    {
        operation=SeqStream_Operation_closed;
    }//close;
    
            
    METHOD(SeqStream,substream,(void),SeqStream*)
    {
        return(theSubstream);
    }//substream;
    
            
    METHOD(SeqStream,get,(CARD8* byte),BOOLEAN)
    {
        if(operation==SeqStream_Operation_read){
            return(this->theSubstream->get(byte));
        }else{
            return(FALSE);
        }
    }//get;


    METHOD(SeqStream,put,(CARD8  byte),BOOLEAN)
    {
        if(operation==SeqStream_Operation_write){
            return(this->theSubstream->put(byte));
        }else{
            return(FALSE);
        }
    }//put;
    

    METHOD(SeqStream,copyTo,(SeqStream* to,CARD32 size),CARD32)
    {
            CARD32          count;
            CARD8           byte;
        
        count=0;
        while(count<size){
            if(!get(&byte)){
                return(count);
            }
            if(!to->put(byte)){
                return(count);
            }
            INC(count);
        }
        return(count);
    }//copyTo;
    

    METHOD(SeqStream,skip,(CARD32 size),CARD32)
    {
            CARD32          count;
            CARD8           byte;
        
        count=0;
        while(count<size){
            if(!get(&byte)){
                return(count);
            }
            INC(count);
        }
        return(count);
    }//skip;
    

//END SeqStream.


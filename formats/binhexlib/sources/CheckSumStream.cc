/******************************************************************************
FILE:               CheckSumStream.cc
LANGUAGE:           C++
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This is the class for a checksummed stream of bytes substreams.
    A 16-bit check sum is maintained while reading or writting the stream.
    When closing the stream, after reading it, two more bytes are read
    and checked; after writting it, the check sum is appened before closing.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1993/05/01 <PJB> Creation.
LEGAL
    Copyright Pascal J. Bourguignon 1993 - 2011
    All rights reserved.
    This program may not be included in any commercial product without the 
    author written permission. It may be used freely for any non-commercial 
    purpose, provided that this header is always included.
******************************************************************************/
#include <BcInterface.h>
#include <BcTypes.h>
#include <SeqStream.hh>
#include <CheckSumStream.hh>
#include <BcImplementation.h>

    CONSTRUCTOR(CheckSumStream)
    {
        checksum=0;
        sumcheck=TRUE;
    }//CheckSumStream;
    
    
    DESTRUCTOR(CheckSumStream)
    {
        // NOP
    }//~CheckSumStream;
    
    
    METHOD(CheckSumStream,reset,(SeqStream* nSubstream),SeqStream*)
    {
        CheckSumStream_SUPER::reset(nSubstream);
        checksum=0;
        sumcheck=TRUE;
        return(this);
    }//reset;
    
    
    METHOD(CheckSumStream,rewrite,(SeqStream* nSubstream),SeqStream*)
    {
        CheckSumStream_SUPER::rewrite(nSubstream);
        checksum=0;
        sumcheck=TRUE;
        return(this);
    }//rewrite;
    
    
    METHOD(CheckSumStream,close,(void),void)
    {
            CARD16                  sum;
            CARD8*                  ptr;
            
        switch(operation){
        case SeqStream_Operation_read:
            ptr=(CARD8*)&sum;
            sumcheck=FALSE;
            do{
                if(!theSubstream->get(&(ptr[0]))){ continue; }
                if(!theSubstream->get(&(ptr[1]))){ continue; }
                sumcheck=(sum==checksum);
            }while(FALSE);//once
            CheckSumStream_SUPER::close();
            break;
        case SeqStream_Operation_write:
            ptr=(CARD8*)(&checksum);
            sumcheck=FALSE;
            do{
                if(!theSubstream->put(ptr[0])){ continue; }
                if(!theSubstream->put(ptr[1])){ continue; }
                sumcheck=TRUE;
            }while(FALSE);//once
            CheckSumStream_SUPER::close();
            break;
        default:
            sumcheck=FALSE;
            break;
        }
    }//close;
    
    
    METHOD(CheckSumStream,check,(void),BOOLEAN)
    {
        return(sumcheck);
    }//check;
    
    
    METHOD(CheckSumStream,get,(CARD8* byte),BOOLEAN)
    {
            BOOLEAN                 ok;

        if(operation!=SeqStream_Operation_read){
            return(FALSE);
        }
        ok=theSubstream->get(byte);
        if(ok){
            checksum=(CARD16)(((CARD32)(checksum)+(CARD32)(*byte))%65536);
        }
        return(ok);
    }//get;
    
    
    METHOD(CheckSumStream,put,(CARD8  byte),BOOLEAN)
    {
        if(operation!=SeqStream_Operation_write){
            return(FALSE);
        }
        checksum=(CARD16)(((CARD32)(checksum)+(CARD32)byte)%65536);
        return(theSubstream->put(byte));
    }//put;
    
    
//END CheckSumStream;


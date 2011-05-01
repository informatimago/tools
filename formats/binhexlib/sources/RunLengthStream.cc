/******************************************************************************
FILE:               RunLengthStream.cc
LANGUAGE:           C++
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This is the class for a run-length compacted stream of bytes.
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
#include <RunLengthStream.hh>
#include <BcImplementation.h>


CONSTRUCTOR(RunLengthStream)
{
    length=0;
    byte=0;
}//RunLengthStream;
    
    
DESTRUCTOR(RunLengthStream)
{
    // NOP
}//~RunLengthStream;
    
    
METHOD(RunLengthStream,reset,(SeqStream* nSubstream),SeqStream*)
{
    RunLengthStream_SUPER::reset(nSubstream);
    length=0;
    byte=0;
    return(this);
}//reset;
    
    
METHOD(RunLengthStream,rewrite,(SeqStream* nSubstream),SeqStream*)
{
    RunLengthStream_SUPER::rewrite(nSubstream);
    length=0;
    byte=0;
    return(this);
}//rewrite;
    
    
METHOD(RunLengthStream,close,(void),void)
{
    switch(operation){
    case SeqStream_Operation_write:
        if(length>1){
            do{
                if((byte==(CARD8)0x90)||(length>3)){
                    if(!theSubstream->put((CARD8)0x90)){ continue; }
                    if(!theSubstream->put((CARD8)(length))){
                        continue;
                    }
                }else{
                    do{
                        if(!theSubstream->put(byte)){ continue; }
                        DEC(length);
                    }while(length>1);
                }
            }while(FALSE);//once
        }
        RunLengthStream_SUPER::close();
        break;
    case SeqStream_Operation_read:
        RunLengthStream_SUPER::close();
        // no error <=> (length==0);
        break;
    default:
        // error: already closed
        break;
    }
}//close;
    

METHOD(RunLengthStream,get,(CARD8* bbyte),BOOLEAN)
{
    CARD8 code;
            
    if(operation!=SeqStream_Operation_read){
        return(FALSE);
    }
    if(length>0){
        DEC(length);
        (*bbyte)=this->byte;
    }else{
        if(!theSubstream->get(&code)){ return(FALSE); }
        if(code==(CARD8)0x90){
            if(!theSubstream->get(&length)){ return(FALSE); }
            if(length==0){
                this->byte=code;
                (*bbyte)=code;
            }else{
                DECR(length,2);
                (*bbyte)=this->byte;
            }
        }else{
            this->byte=code;
            (*bbyte)=code;
        }
    }
    return(TRUE);
}//get;
    

METHOD(RunLengthStream,put,(CARD8 nbyte),BOOLEAN)
{
    if(operation!=SeqStream_Operation_write){
        return(FALSE);
    }
    switch(length){
    case 0:
        if(!theSubstream->put(nbyte)){ return(FALSE); }
        if(nbyte==(CARD8)0x90){
            if(!theSubstream->put((CARD8)0x00)){ return(FALSE); }
        }
        /* going to 1 */
        length=1;
        this->byte=nbyte;
        break;
    case 1:
        if(nbyte==this->byte){
            INC(length);
            /* going to >1 */
        }else{
            /* put the new byte */
            if(!theSubstream->put(nbyte)){ return(FALSE); }
            if(nbyte==(CARD8)0x90){
                if(!theSubstream->put((CARD8)0x00)){ return(FALSE); }
            }
            /* staying in 1 */
            this->byte=nbyte;
        }
        break;
    default:
        if(nbyte==this->byte){
            if(length==254){
                if(!theSubstream->put((CARD8)0x90)){ return(FALSE); }
                if(!theSubstream->put((CARD8)255)){ return(FALSE); }
                /* going to 0 */
                length=0;
            }else{
                INC(length);
                /* staying in >1 */
            }
        }else{
            if((this->byte==(CARD8)0x90)||(length>3)){
                if(!theSubstream->put((CARD8)0x90)){ return(FALSE); }
                if(!theSubstream->put((CARD8)(length))){
                    return(FALSE);
                }
            }else{
                do{
                    if(!theSubstream->put(this->byte)){ return(FALSE); }
                    DEC(length);
                }while(length>1);
            }
            /* put the new nbyte */
            if(!theSubstream->put(nbyte)){ return(FALSE); }
            if(nbyte==(CARD8)0x90){
                if(!theSubstream->put((CARD8)0x00)){ return(FALSE); }
            }
            /* going to 1 */
            length=1;
            this->byte=nbyte;
        }
        break;
    }
    return(TRUE);
}//put
/*
  9090909090
  909005
  9090
  90009000
  909002
*/
    

//// THE END ////

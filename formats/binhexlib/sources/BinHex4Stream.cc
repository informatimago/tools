/******************************************************************************
FILE:               BinHex4Stream.cc
LANGUAGE:           C++
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This is the class for a BinHex4Stream to be used in conjunction with
    SevenStream.
    While reading, this stream skips comment lines, starting colon (first ':'
    at begining of a line), end of lines, and terminating colon. 
    While writing, this stream insert a comment line:
           (This file must be converted with BinHex 4.0)
    followed by a starting colon, then it will insert an end of line every 64
    characters, and it will append a terminating colon at the end.
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
extern "C"{
#include <string.h>
}
#include <BcInterface.h>
#include <BcTypes.h>
#include <ASCII.h>
#include <SeqStream.hh>
#include <SevenStream.hh>
#include <BinHex4Stream.hh>
#include <BcImplementation.h>


    CONSTRUCTOR(BinHex4Stream)
    {
        eoln[0]=ASCII_CR;
        eoln[1]=ASCII_LF;
        eoln[2]=ASCII_NUL;
        count=0;
        eof=TRUE;
    }//BinHex4Stream;
    
    
    DESTRUCTOR(BinHex4Stream)
    {
        // NOP
    }//~BinHex4Stream;
    
    
    METHOD(BinHex4Stream,reset,(SeqStream* nSubstream),SeqStream*)
    {
            CARD8               byte;
            
        BinHex4Stream_SUPER::reset(nSubstream);
        eof=TRUE;
        // read until a colon if 1st column.
        if(!theSubstream->get(&byte)){ eof=TRUE; return(this); }
        while(byte!=':'){
            while((byte!=ASCII_CR)&&(byte!=ASCII_LF)){
                if(!theSubstream->get(&byte)){ return(this); }
            }
            while((byte==ASCII_CR)||(byte==ASCII_LF)){
                if(!theSubstream->get(&byte)){ return(this); }
            }
        }
        eof=FALSE;      
        count=1;
        return(this);
    }//reset;
    

    static
    CARD8       comment[]="(This file must be converted with BinHex 4.0)";
    
    METHOD(BinHex4Stream,writeEoln,(void),BOOLEAN)
    {
            CARD16          index;

        index=0;
        while(eoln[index]!=(CHAR)0){
            if(!theSubstream->put(eoln[index])){ return(FALSE); }
            INC(index);
        }
        count=0;
        return(TRUE);
    }//writeEoln;
    

    METHOD(BinHex4Stream,rewrite,(SeqStream* nSubstream),SeqStream*)
    {
            CARD16          index;
            
        BinHex4Stream_SUPER::rewrite(nSubstream);
        eof=TRUE;
        // write "(This file must be converted with BinHex 4.0)\n:"
        index=0;
        while(comment[index]!=(CHAR)0){
            if(!theSubstream->put(comment[index])){ return(this); }
            INC(index);
        }
        if(!writeEoln()){
            return(this);
        }
        if(!theSubstream->put(':')){ return(this); }
        eof=FALSE;
        count=1;
        return(this);
    }//rewrite;
    

    METHOD(BinHex4Stream,close,(void),void)
    {           
        switch(operation){
        case SeqStream_Operation_write:
            // write ":\n"
            theSubstream->put(':');
            writeEoln();
            BinHex4Stream_SUPER::close();
            break;
        case SeqStream_Operation_read:
            BinHex4Stream_SUPER::close();
            break;
        default:
            break;
        }
        eof=TRUE;
        count=0;
    }//close;
    
    
    METHOD(BinHex4Stream,get,(CARD8* byte),BOOLEAN)
    {
        // SEE: should increment count and check line lengths.
        if((operation!=SeqStream_Operation_read)||(eof)){
            return(FALSE);
        }
        do{
            if(!theSubstream->get(byte)){ eof=TRUE; return(FALSE); }
        }while(((*byte)==ASCII_CR)||((*byte)==ASCII_LF));
        if((*byte)==':'){
            eof=TRUE;
            return(FALSE);
        }
        return(TRUE);       
    }//get;
    
    
    METHOD(BinHex4Stream,put,(CARD8  byte),BOOLEAN)
    {           
        if((operation!=SeqStream_Operation_write)||(eof)){
            return(FALSE);
        }
        if(!theSubstream->put(byte)){ eof=TRUE; return(FALSE); }
        INC(count);
        if(count>=64){
            if(!writeEoln()){ eof=TRUE; return(FALSE); }
        }
        return(TRUE);       
    }//put;
    
    
    METHOD(BinHex4Stream,setEndOfLine,(CHAR* nEoln),void)
    {
        strncpy((char*)eoln,(const char*)nEoln,sizeof(eoln));
        eoln[sizeof(eoln)]=(CHAR)0;
    }//setEndOfLine;


//END BinHex4Stream.

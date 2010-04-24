/******************************************************************************
FILE:               SevenStream.cc
LANGUAGE:           C++
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This is the class for a stream of bytes based on 7-bit substreams.
    (This streams reads and writes only 7-bit values).
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
#include <SevenStream.hh>
#include <BcImplementation.h>

    CHAR        Seven_Table[65]=
        "!\"#$%&\'()*+,-012345689@ABCDEFGHIJKLMNPQRSTUVXYZ[`abcdefhijklmpqr";

    static
    PROCEDURE(CharTo6bit,(CHAR c),CARD8)
        /*
            RETURN:     64 if c is not a valid encoded value,
                        [0..63] the value encoded in c.
        */
    {
            CARD16      index;
        index=0;
        while((index<64)&&(Seven_Table[index]!=c)){
            INC(index);
        }
        return((CARD8)index);
    }//CharTo6bit
    

    static
    PROCEDURE(CharFrom6bit,(CARD8 bits),CHAR)
        /*
            PRE:        bits:[0..63];
            RETURN:     the encoded CHAR representing bits.
        */
    {
        return(Seven_Table[bits]);
    }//CharFrom6bit
    

    CONSTRUCTOR(SevenStream)
    {
        lastok=0;
        index=0;
    }//SevenStream;
    
    
    DESTRUCTOR(SevenStream)
    {
        // NOP
    }//~SevenStream;
    
    
    METHOD(SevenStream,reset,(SeqStream* nSubstream),SeqStream*)
    {
        SevenStream_SUPER::reset(nSubstream);
        index=0;
        lastok=-1;
        return(this);
    }//reset;
    

    METHOD(SevenStream,rewrite,(SeqStream* nSubstream),SeqStream*)
    {
        SevenStream_SUPER::rewrite(nSubstream);
        index=0;
        lastok=-1;
        return(this);
    }//rewrite;
    

    METHOD(SevenStream,close,(void),void)
    {
            CHAR                c;
            
        switch(operation){
        case SeqStream_Operation_write:
            if(index>0){
                while(index<3){
                    buffer[index]=0;
                    INC(index);
                }
                index=0;
        // SEE: do we really need to flush the whole buffer ?
                do{
                    c=CharFrom6bit((buffer[0]&0xfc)>>2);
                    if(!theSubstream->put(c)){ continue; }
                    c=CharFrom6bit(((buffer[0]&0x03)<<4)
                                    |((buffer[1]&0xf0)>>4));
                    if(!theSubstream->put(c)){ continue; }
                    c=CharFrom6bit(((buffer[1]&0x0f)<<2)
                                    |((buffer[2]&0xc0)>>6));
                    if(!theSubstream->put(c)){ continue; }
                    c=CharFrom6bit(buffer[2]&0x3f);
                    if(!theSubstream->put(c)){ continue; }
                }while(FALSE);//once
            }
            SevenStream_SUPER::close();
            break;
        case SeqStream_Operation_read:
            SevenStream_SUPER::close();
            break;
        default:
            break;
        }
    }//close;
    
    
    METHOD(SevenStream,get,(CARD8* byte),BOOLEAN)
    {
        if(operation!=SeqStream_Operation_read){
            return(FALSE);
        }
        if(index==0){
            do{
                    CARD8       c;
                    
                if(!theSubstream->get(&c)){ lastok=-1; continue; }
                buffer[0]=CharTo6bit(c);
                if(buffer[0]==65){ return(FALSE); }
                if(!theSubstream->get(&c)){ lastok=0; continue; }
                buffer[1]=CharTo6bit(c);
                if(buffer[1]==65){ return(FALSE); }
                if(!theSubstream->get(&c)){ lastok=1; continue; }
                buffer[2]=CharTo6bit(c);
                if(buffer[2]==65){ return(FALSE); }
                if(!theSubstream->get(&c)){ lastok=2; continue; }
                buffer[3]=CharTo6bit(c);
                if(buffer[3]==65){ return(FALSE); }
                lastok=3;
            }while(FALSE);//once
        }
/*
    4 6-bit bytes   ->      3 8-bit bytes
    -------------           --------------
    lastok          ->      index : [0..2]
        -1                      < -1    none
         0                      <  0    none
         1                      <  1    [0..0]
         2                      <  2    [0..1]
         3                      <  3    [0..2]
*/
        if(index<lastok){
            switch(index){
            case 0:
                (*byte)=((buffer[0]&0x3f)<<2)|((buffer[1]&0x30)>>4);
                break;
            case 1:
                (*byte)=((buffer[1]&0x0f)<<4)|((buffer[2]&0x3c)>>2);
                break;
            case 2:
                (*byte)=((buffer[2]&0x03)<<6)|(buffer[3]&0x3f);
                break;
            default:
                break;
            }
            INC(index);
            if(index>2){
                index=0;
            }
            return(TRUE);
        }else{
            return(FALSE);
        }
    }//get;
    
    
    METHOD(SevenStream,put,(CARD8  byte),BOOLEAN)
    {
            CHAR                c;
            
        if(operation!=SeqStream_Operation_write){
            return(FALSE);
        }
        buffer[index]=byte;
        INC(index);
        if(index>=3){
            index=0;
            c=CharFrom6bit((buffer[0]&0xfc)>>2);
            if(!theSubstream->put(c)){ return(FALSE); }
            c=CharFrom6bit(((buffer[0]&0x03)<<4)
                            |((buffer[1]&0xf0)>>4));
            if(!theSubstream->put(c)){ return(FALSE); }
            c=CharFrom6bit(((buffer[1]&0x0f)<<2)
                            |((buffer[2]&0xc0)>>6));
            if(!theSubstream->put(c)){ return(FALSE); }
            c=CharFrom6bit(buffer[2]&0x3f);
            if(!theSubstream->put(c)){ return(FALSE); }
        }
        return(TRUE);       
    }//put;
    
    
//END SevenStream.

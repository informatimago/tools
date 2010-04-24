/******************************************************************************
FILE:               StdIOStream.cc
LANGUAGE:           C++
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This is the class for a stream of bytes based stdio FILEs.
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
#include <stdio.h>
#include <stdlib.h>
}
#include <BcInterface.h>
#include <BcTypes.h>
#include <SeqStream.hh>
#include <StdIOStream.hh>
#include <BcImplementation.h>


    CONSTRUCTOR(StdIOStream)
    {
        file=NIL;
    }//StdIOStream;
    
    
    DESTRUCTOR(StdIOStream)
    {
        // NOP
    }//~StdIOStream;
    
    
    METHOD(StdIOStream,reset,(SeqStream* nSubstream),SeqStream*)
    {
        fprintf(stderr,"You mustn't call StdIOStream:reset(SeqStream*); "
                        "call StdIOStream:reset(FILE*) instead.\n");
        exit(1);     
        return(this);
    }//reset;
    
    
    METHOD(StdIOStream,rewrite,(SeqStream* nSubstream),SeqStream*)
    {
        fprintf(stderr,"You mustn't call StdIOStream:rewrite(SeqStream*); "
                        "call StdIOStream:rewrite(FILE*) instead.\n");
        exit(1);     
        return(this);
    }//rewrite;
    

    METHOD(StdIOStream,reset,(FILE* nFile),SeqStream*)
    {
        StdIOStream_SUPER::reset(NIL);
        file=nFile;
        return(this);
    }//reset;

    
    METHOD(StdIOStream,rewrite,(FILE* nFile),SeqStream*)
    {
        StdIOStream_SUPER::rewrite(NIL);
        file=nFile;
        return(this);
    }//rewrite;
    

    METHOD(StdIOStream,close,(void),void)
    {
        file=NIL;
        StdIOStream_SUPER::close();
    }//close;
    
    
    METHOD(StdIOStream,get,(CARD8* byte),BOOLEAN)
    {
        if(operation==SeqStream_Operation_read){
            return(fread(byte,sizeof(CARD8),1,(FILE*)file)==1);
        }else{
            return(FALSE);
        }   
    }//get;

    
    METHOD(StdIOStream,put,(CARD8  byte),BOOLEAN)
    {
        if(operation==SeqStream_Operation_write){
            return(fwrite(&byte,sizeof(CARD8),1,(FILE*)file)==1);           
        }else{
            return(FALSE);
        }   
    }//put

//END StdIOStream.

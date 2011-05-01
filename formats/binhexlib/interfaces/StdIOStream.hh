/******************************************************************************
FILE:               StdIOStream.hh
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
    Copyright Pascal J. Bourguignon 1993 - 2011
    All rights reserved.
    This program may not be included in any commercial product without the 
    author written permission. It may be used freely for any non-commercial 
    purpose, provided that this header is always included.
******************************************************************************/
#ifndef __StdIOStream__
#define __StdIOStream__
extern "C"{
#include <stdio.h>
}
#include <BcInterface.h>
#include <BcTypes.h>
#include <SeqStream.hh>

#define StdIOStream_SUPER   SeqStream
class StdIOStream:public StdIOStream_SUPER
{
public:

    CONSTRUCTOR(StdIOStream)
    DESTRUCTOR(StdIOStream)

    METHOD(StdIOStream,reset,(SeqStream* nSubstream),SeqStream*)
    METHOD(StdIOStream,rewrite,(SeqStream* nSubstream),SeqStream*)
        /*
            NOTE:       use following methods instead!
            DO:         print an error message on stderr and exit.
        */
        
    METHOD(StdIOStream,reset,(FILE* nFile),SeqStream*)
    METHOD(StdIOStream,rewrite,(FILE* nFile),SeqStream*)
        /*
            WARNING:    parameter is FILE*, not SeqStream* !
        */
    METHOD(StdIOStream,close,(void),void)
        
    METHOD(StdIOStream,get,(CARD8* byte),BOOLEAN)
    METHOD(StdIOStream,put,(CARD8  byte),BOOLEAN)
    
protected:
    FILE*               file;
};//StdIOStream;

#endif

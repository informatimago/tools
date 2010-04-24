/******************************************************************************
FILE:               RunLengthStream.hh
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
    Copyright Pascal J. Bourguignon 1993 - 1993
    All rights reserved.
    This program may not be included in any commercial product without the 
    author written permission. It may be used freely for any non-commercial 
    purpose, provided that this header is always included.
******************************************************************************/
#ifndef __RunLengthStream__
#define __RunLengthStream__
#include <BcInterface.h>
#include <BcTypes.h>
#include <SeqStream.hh>

#define RunLengthStream_SUPER   SeqStream
class RunLengthStream:public RunLengthStream_SUPER
{
public:

    CONSTRUCTOR(RunLengthStream)
    DESTRUCTOR(RunLengthStream)
    METHOD(RunLengthStream,reset,(SeqStream* nSubstream),SeqStream*)
    METHOD(RunLengthStream,rewrite,(SeqStream* nSubstream),SeqStream*)
    METHOD(RunLengthStream,close,(void),void)
    METHOD(RunLengthStream,get,(CARD8* byte),BOOLEAN)
    METHOD(RunLengthStream,put,(CARD8  byte),BOOLEAN)
    
protected:
    CARD8                   length; // current length of run    
    CARD8                   byte;   // current CHAR
};//RunLengthStream;

#endif

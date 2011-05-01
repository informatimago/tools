/******************************************************************************
FILE:               CheckSumStream.hh
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
#ifndef __CheckSumStream__
#define __CheckSumStream__
#include <BcInterface.h>
#include <BcTypes.h>
#include <SeqStream.hh>

#define CheckSumStream_SUPER    SeqStream
class CheckSumStream:public CheckSumStream_SUPER
{
public:

    CONSTRUCTOR(CheckSumStream)
    DESTRUCTOR(CheckSumStream)
    METHOD(CheckSumStream,reset,(SeqStream* nSubstream),SeqStream*)
    METHOD(CheckSumStream,rewrite,(SeqStream* nSubstream),SeqStream*)
    METHOD(CheckSumStream,close,(void),void)
    METHOD(CheckSumStream,check,(void),BOOLEAN)
        /*
            PRE:        this has been open with reset(), and has been closed.
            RETURN:     the computed checksum = the checksum read by close.
        */
    METHOD(CheckSumStream,get,(CARD8* byte),BOOLEAN)
    METHOD(CheckSumStream,put,(CARD8  byte),BOOLEAN)
    
protected:
    CARD16                  checksum;
    BOOLEAN                 sumcheck;
};//CheckSumStream;

#endif

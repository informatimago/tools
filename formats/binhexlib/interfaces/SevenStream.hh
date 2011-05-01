/******************************************************************************
FILE:               SevenStream.hh
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
    Copyright Pascal J. Bourguignon 1993 - 2011
    All rights reserved.
    This program may not be included in any commercial product without the 
    author written permission. It may be used freely for any non-commercial 
    purpose, provided that this header is always included.
******************************************************************************/
#ifndef __SevenStream__
#define __SevenStream__
#include <BcInterface.h>
#include <BcTypes.h>
#include <SeqStream.hh>

    extern CHAR     Seven_Table[65];
    /*
        "!\"#$%&\'()*+,-012345689@ABCDEFGHIJKLMNPQRSTUVXYZ[`abcdefhijklmpqr";
            this exclude the following characters:
                ./7:;<=>?OW\]^_gnostuvwxyz{|}~
    */

#define SevenStream_SUPER   SeqStream
class SevenStream:public SevenStream_SUPER
{
public:

    CONSTRUCTOR(SevenStream)
    DESTRUCTOR(SevenStream)
    METHOD(SevenStream,reset,(SeqStream* nSubstream),SeqStream*)
    METHOD(SevenStream,rewrite,(SeqStream* nSubstream),SeqStream*)
    METHOD(SevenStream,close,(void),void)
    METHOD(SevenStream,get,(CARD8* byte),BOOLEAN)
    METHOD(SevenStream,put,(CARD8  byte),BOOLEAN)
    
protected:
    INT16                   lastok;     // index of last byte ok    
    INT16                   index;      // of next 8-bit byte to read/write
        // operation=Seven_OpWrite =>  index=2 => put will be called.
        // operation=Seven_OpRead  =>  index=0 => get will be called.
    CARD8                   buffer[4];
};//SevenStream;

#endif

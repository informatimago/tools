/******************************************************************************
FILE:               BinHex4Stream.hh
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
AUTHOR
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
#ifndef __BinHex4Stream__
#define __BinHex4Stream__
#include <BcInterface.h>
#include <BcTypes.h>
#include <SeqStream.hh>


#define BinHex4Stream_SUPER SeqStream
class BinHex4Stream:public BinHex4Stream_SUPER
{
public:

    CONSTRUCTOR(BinHex4Stream)
    DESTRUCTOR(BinHex4Stream)
    METHOD(BinHex4Stream,setEndOfLine,(CHAR* nEoln),void)
    METHOD(BinHex4Stream,reset,(SeqStream* nSubstream),SeqStream*)
    METHOD(BinHex4Stream,rewrite,(SeqStream* nSubstream),SeqStream*)
    METHOD(BinHex4Stream,close,(void),void)
    METHOD(BinHex4Stream,get,(CARD8* byte),BOOLEAN)
    METHOD(BinHex4Stream,put,(CARD8  byte),BOOLEAN)
    
protected:
    METHOD(BinHex4Stream,writeEoln,(void),BOOLEAN)
    CHAR            eoln[8];
    CARD16          count;
    BOOLEAN         eof;
};//BinHex4Stream;

#endif

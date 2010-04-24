/******************************************************************************
FILE:               SeqStream.hh
LANGUAGE:           C++
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This is an abstract class for a sequential stream of bytes.
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
#ifndef __SeqStream__
#define __SeqStream__
#include <BcInterface.h>
#include <BcTypes.h>
#include <BpObject.hh>

    typedef enum {
                    SeqStream_Operation_closed, 
                    SeqStream_Operation_read,
                    SeqStream_Operation_write
    }           SeqStream_OperationT;

#define SeqStream_SUPER     BpObject
class SeqStream:public SeqStream_SUPER
{
public:

    CONSTRUCTOR(SeqStream)
        /*
            DO:         build a new sequential stream with no substream.
        */

    DESTRUCTOR(SeqStream)
        /*
            DO:         destroy this stream (but not the substream).
        */

    METHOD(SeqStream,reset,(SeqStream* nSubstream),SeqStream*)
        /*
            DO:         open this stream for reading from nSubstream.
            POST:       this->substream()=nSubstream.
            RETURN:     this.
        */
        
    METHOD(SeqStream,rewrite,(SeqStream* nSubstream),SeqStream*)
        /*
            DO:         open this stream for writting to nSubstream.
            POST:       this->substream()=nSubstream.
            RETURN:     this.
        */
        
    METHOD(SeqStream,close,(void),void)
        /*
            DO:         close this stream.
        */
        
    METHOD(SeqStream,substream,(void),SeqStream*)
        /*
            RETURN:     the current substream.
        */
        
    METHOD(SeqStream,get,(CARD8* byte),BOOLEAN)
        /*
            DO:         read a 8-bit value by calling
                            this->substream()->get(byte).
            RETURN:     true when a char could be read,
                        false when error or end-of-file.
            NOTE:       subclasses should override this method.
        */

    METHOD(SeqStream,put,(CARD8  byte),BOOLEAN)
        /*
            DO:         write a 8-bit value by calling
                            this->substream()->put(byte).
            RETURN:     true when the char could be written,
                        false when error.
            NOTE:       subclasses should override this method.
        */
    
    METHOD(SeqStream,copyTo,(SeqStream* to,CARD32 size),CARD32)
    METHOD(SeqStream,skip,(CARD32 size),CARD32)
        /*
            RETURN:     number of skiped or copied bytes.
        */
    
protected:
    SeqStream_OperationT    operation;
    SeqStream*              theSubstream;
};//SeqStream;


#endif

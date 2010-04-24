/******************************************************************************
FILE:               ASSegment.m
LANGUAGE:           Objective-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This class encapsulate memory streams, plus an entry Id.        
AUTHORS
    <PJB> Pascal J. Bourguignon 
MODIFICATIONS
    1992/03/25 <PJB> Creation.
LEGAL
    Copyright Pascal J. Bourguignon 1992 - 1992
    All rights reserved.
    This program may not be included in any commercial product without the 
    author written permission. It may be used freely for any non-commercial 
    purpose, provided that this header is always included.
******************************************************************************/
#import <objc/Object.h>
#import <objc/error.h>
#import <streams/streams.h>
#include <PJBTypes.h>   // OBSOLETE
#include <ASSegment.h>

@interface ASSegment:Object
    /*
        DESCRIPTION:
            This may be a file fork, or simply some attributes.
            It is lent a NXStream to read/write to and from.
        INVARIANTS:
    */
{
//FIELDS
    int32           myEntryId;
    int32           myLength;
    int32           lastError;
    NXStream*       myStream;
}

//FACTORY
    
    + (id)new:(int32)entryId stream:(NXStream*)stream
        /*
            RETURN:     a new empty segment associated with the stream.
        */
    {
            id      segment;
            
        segment=[super new];
        [segment changeEntryId:entryId];
        NX_DURING
            /* we cannot use NXGetMemoryBuffer since we may have disk based streams */
            NXSeek(stream,0,NX_FROMEND);
            [segment changeLength:NXTell(stream)];
        NX_HANDLER
            lastError=NXLocalHandler.code;
            switch(NXLocalHandler.code) {
            case NX_illegalSeek:
            case NX_illegalStream:
            case NX_streamVMError:
                break;
            default:
                NX_RERAISE();
            }
        NX_ENDHANDLER
        [segment changeStream:stream];
        [segment changeError:0];
        return(segment);
    }//new:stream:
    
//INSTANCE

    - (int32)error
        /*
            RETURN:     the last error encountered
        */
    {
        return(lastError);
    }//error
    
    - (id)changeStream:(NXStream*)stream
        /*
            PRE:        stream==s
            POST:       myStream==s
            RETURN:     self
        */
    {
        lastError=0;
        myStream=s;
        return(self);
    }//changeStream:
    
    - (int32)entryId
        /*
            RETURN:     the entryId of self.
        */
    {
        lastError=0;
        return(myEntryId);
    }//entryId
    
    
    - (id)changeEntryId:(int32)entryId
        /*
            PRE:        entryId=i
            POST:       [self entryId]==i
            RETURN:     self
        */
    {
        lastError=0;
        myEntryId=entryId;
        return(self);
    }//changeEntryId:
    
    - (int32)length
        /*
            PRE:        self is open
            RETURN:     the length of the segment
        */
    {
        lastError=0;
        return(myLength);
    }//length
    
    - (id)changeLength:(int32)newLength
        /*
            PRE:        newLength==l;
            POST:       [self length]==l
            RETURN:     self
        */
    {
        lastError=0;
        myLength=newLength;
        return(self);
    }//changeLength:
    
    - (int32)truncate:(int32)newLength
        /*
            PRE:        self is open
            POST:       [self length]==newLength or [self error]!=0
            DO:         truncate or allocate space for the segment.
                        allocated space is zeroed.
            RETURN:     [self length], that may be different from newLength when [self error]!=0
        */
    {
        lastError=0;
        if(newLength<=myLength){
            myLength=newLength;
        }else{
                int32       writtenLength;
                int32       allocateLength;
                int32       buffer[256];
                int32       count;
                
            allocateLength=newLength-myLength;
            count=allocateLength/sizeof(int32)+1;
            if(count>256){
                count=256;
            }
            while(count>0){
                DEC(count);
                buffer[count]=0;
            }
            
            while(allocateLength>size(buffer)){
                writtenLength=[self write:(char*)buffer to:myLength length:size(buffer)];
                allocateLength-=writtenLength;
                myLength+=writtenLength;
                if(lastError!=0){
                    return(myLength);
                }
            }
            if(allocateLength>0){
                writtenLength=[self write:(char*)buffer to:myLength length:allocateLength-];
                allocateLength-=writtenLength;
                myLength+=writtenLength;
                if(lastError!=0){
                    return(myLength);
                }
            }
        }
        return(myLength);
    }//truncate:
    
    - (int32)read:(char*)buffer from:(int32)offset length:(int32)length
        /*
            PRE:        self is open in mode Mode_read or Mode_readWrite,
                        readsize==minimum(length,[self length]-offset)
                        segment[offset .. offset+readsize-1] == a[0 .. readsize-1]
            POST:       [self error]!=0 
                        or (
                                buffer[0 .. readsize-1] == a[0 .. readsize-1]
                            )
            RETURN:     readsize or 0 when [self error]!=0
            DO:         read from the segment bytes at most length bytes from offset.
            NOTE:       [self error]==0 and readsize==0 <=> we reached the end-of-segment.
        */
    {
            int32       actualLength;
            int32       readLength;
        
        lastError=0;
        actualLength=0;
        if(offset>=myLength){
            return(actualLength);
        }else if(offset+length>=myLength){
            length=myLength-offset;
        }
        NX_DURING
            NXSeek(myStream,offset,NX_FROMSTART);
            actualLength=NXRead(myStream,buffer,length);
        NX_HANDLER
            lastError=NXLocalHandler.code;
            switch(NXLocalHandler.code) {
            case NX_illegalWrite:
            case NX_illegalRead:
            case NX_illegalSeek:
            case NX_illegalStream:
            case NX_streamVMError:
                break;
            default:
                NX_RERAISE();
            }
        NX_ENDHANDLER
        return(actualLength);
    }//read:from:length:
    
    - (int32)write:(char*)buffer to:(int32)offset length:(int32)length
        /*
            PRE:        self is open in mode Mode_write, Mode_append or Mode_readWrite,
                        buffer[0 .. length-1] == a[0 .. length-1]
                        oldfsize==[self length]
                        self is open in mode Mode_append => offset == oldfsize
            POST:       0<=writesize<=length
                        and buffer[0 .. writesize-1] == a[0 .. writesize-1]
                        and (
                                [self error]!=0
                                or [self length]==minimum(oldfsize,offset+writesize)
                            )
            RETURN:     writesize, that may be different from length when [self error]!=0
            DO:         write to the segment bytes at most length bytes from offset.
        */
    {
            int32       actualLength;
            int32       readLength;
        
        lastError=0;
        actualLength=0;
        NX_DURING
            if(offset>=myLength){
                [self truncate:offset];
                if(lasterror!=0){
                    return(actualLength);
                }
            }
            NXSeek(myStream,offset,NX_FROMSTART);
            actualLength=NXWrite(myStream,buffer,length);
            if((offset+actualLength)>myLength){
                myLength=offset+actualLength;
            }
        NX_HANDLER
            lastError=NXLocalHandler.code;
            switch(NXLocalHandler.code) {
            case NX_illegalWrite:
            case NX_illegalRead:
            case NX_illegalSeek:
            case NX_illegalStream:
            case NX_streamVMError:
                break;
            default:
                NX_RERAISE();
            }
        NX_ENDHANDLER
        return(actualLength);
    }//write:to:length:


@end//ASSegment

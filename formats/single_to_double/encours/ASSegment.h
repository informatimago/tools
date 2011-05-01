/******************************************************************************
FILE:               ASSegment.h
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
    Copyright Pascal J. Bourguignon 1992 - 2011
    All rights reserved.
    This program may not be included in any commercial product without the 
    author written permission. It may be used freely for any non-commercial 
    purpose, provided that this header is always included.
******************************************************************************/
#import <objc/Object.h>
#import <streams/streams.h>
#include <PJBTypes.h>   // OBSOLETE

@interface ASSegment:Object
    /*
        DESCRIPTION:
            This may be a file fork, or simply some attributes.
            It is lent a NXStream to read/write to and from.
        INVARIANTS:
    */
{
//FIELDS
}

//FACTORY
    
    + (id)new:(int32)entryId stream:(NXStream*)stream;
        /*
            RETURN:     a new empty segment associated with the stream.
        */
        
//INSTANCE

    - (int32)entryId;
        /*
            RETURN:     the entryId of self.
        */
        
    - (id)changeEntryId:(int32)entryId;
        /*
            PRE:        entryId=i
            POST:       [self entryId]==i
            RETURN:     self
        */
    
    - (int32)length;
        /*
            PRE:        self is open
            RETURN:     the length of the segment
        */
        
    - (int32)truncate:(int32)newLength;
        /*
            PRE:        self is open
            POST:       [self length]==newLength or [self error]!=0
            DO:         truncate or allocate space for the segment.
                        allocated space is zeroed.
            RETURN:     [self length], that may be different from newLength when [self error]!=0
        */
        
    - (int32)read:(char*)buffer from:(int32)offset length:(int32)length;
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
            NOTE:       readsize==0 <=> we reached the end-of-segment.
        */
        
    - (int32)write:(char*)buffer to:(int32)offset length:(int32)length;
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

@end//ASSegment

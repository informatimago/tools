/******************************************************************************
FILE:               AppleSingle.h
LANGUAGE:           Objective-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This object is able to access Apple-Single files.       
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
#include <PJBTypes.h>   // OBSOLETE


typedef enum {Mode_read,Mode_write,Mode_readWrite,Mode_append} ModeT;

typedef enum {
    Segm_DataFork=          1,
    Segm_ResourceFork=      2,
    Segm_RealName=          3,
    Segm_Comment=           4,
    Segm_BWIcon=            5,
    Segm_ColorIcon=         6,
    Segm_FileDateInfo=      8,
    Segm_FinderInfo=        9,
    Segm_MacintoshFileInfo= 10,
    Segm_ProDOSFileInfo=    11,
    Segm_MSDOSFileInfo=     12,
    Segm_AFPShortName=      13,
    Segm_AFPFileInfo=       14,
    Segm_AFPDirectoryId=    15
}                   SegmT;

    
@interface AppleSingle:Object
    /*
        DESCRIPTION:
            The purpose of this class is to provide the client with a low-level
            input/output service on the differents segments composing an Apple-Single file.
        INVARIANTS:
            all int32 but error codes are positive.
    */
{
//FIELDS

}

//FACTORY

    + (id)new;
        /*
            RETURN:     a new closed AppleSingle file object,
                        or nil when out-of memory.
        */
    
        
//INSTANCE

    - (int32)error;
        /*
            RETURN:     the code of the last error that occured with self. 
        */

    - (id)create:(char*)fname mode:(ModeT)mode;
        /*
            PRE:        self is closed, mode!=Mode_read
            POST:       [self error]==0 <=> self is open for mode access.
            RETURN:     self
            DO:         create the file fname, and open it for mode access.
        */
        
    - (id)open:(char*)fname mode:(ModeT)mode;
        /*
            PRE:        self is closed
            POST:       [self error]==0 <=> self is open for mode access.
            RETURN:     self
            DO:         open the file fname for mode access.
        */
        
    - (id)close;
        /*
            PRE:        self is open
            DO:         closes the file a
            POST:       self is closed.
        */
    
    - (int32)dataLength;
        /*
            PRE:        self is open
            RETURN:     the length of the data fork
        */
        
    - (int32)dataTruncate:(int32)newLength;
        /*
            PRE:        self is open
            POST:       [self dataLength]==newLength or [self error]!=0
            DO:         truncate or allocate space for the data fork.
                        allocated space is zeroed.
            RETURN:     [self dataLength], that may be different from newLength when [self error]!=0
        */
        
    - (int32)dataRead:(char*)buffer from:(int32)offset length:(int32)length;
        /*
            PRE:        self is open in mode Mode_read or Mode_readWrite,
                        readsize==minimum(length,[self dataLength]-offset)
                        datafork[offset .. offset+readsize-1] == a[0 .. readsize-1]
            POST:       [self error]!=0 
                        or (
                                buffer[0 .. readsize-1] == a[0 .. readsize-1]
                            )
            RETURN:     readsize or 0 when [self error]!=0
            DO:         read from the data fork bytes at most length bytes from offset.
            NOTE:       readsize==0 <=> we reached the end-of-segment.
        */
        
    - (int32)dataWrite:(char*)buffer to:(int32)offset length:(int32)length;
        /*
            PRE:        self is open in mode Mode_write, Mode_append or Mode_readWrite,
                        buffer[0 .. length-1] == a[0 .. length-1]
                        oldfsize==[self dataLength]
                        self is open in mode Mode_append => offset == oldfsize
            POST:       0<=writesize<=length
                        and buffer[0 .. writesize-1] == a[0 .. writesize-1]
                        and (
                                [self error]!=0
                                or [self dataLength]==minimum(oldfsize,offset+writesize)
                            )
            RETURN:     writesize, that may be different from length when [self error]!=0
            DO:         write to the data fork bytes at most length bytes from offset.
        */

/**
    All right, that is strictly the same thing for the two following groups of methods:
        the first for the resource fork, and the second for any segment in the 
        Apple-Single file.
    Note that actually, the data and the resource fork are the segments 1 and 2.
    When a segment does not exist, its segmLength is 0; to delete a segment,
    simply truncate it to 0. (Actually, the entry for a truncated segment is not removed
    from the entry descriptor table, but it is the same semantically).
**/

    - (int32)rsrcLength;
    - (int32)rsrcResize:(int32)newLength;
    - (int32)rsrcReadBuffer:(char*)buffer from:(int32)offset length:(int32)length;
    - (int32)rsrcWriteBuffer:(char*)buffer from:(int32)offset length:(int32)length;
    
    - (int32)segmLength:(int32)segNum;
    - (int32)segmTruncate:(int32)segNum newLength:(int32)newLength;
    - (int32)segmRead:(int32)segNum buffer:(char*)buffer from:(int32)offset length:(int32)length;
    - (int32)segmWrite:(int32)segNum buffer:(char*)buffer from:(int32)offset length:(int32)length;
    
    
    - (id)free;
        /*
            DO:         closes the file associated with self if any,
                        and frees the memory allocated to self.
            RETURN:     nil
        */
        

@end //AppleSingle

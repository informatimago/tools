/******************************************************************************
FILE:               MacBinary.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program converts to and from MacBinary file format.
USAGE
        macbinary -x file
            extract 
        macbinary -c file datafork rsrcfork [ -C creator ] [ -T type ]
AUTHORS
    <PJB> Pascal J. Bourguignon 
MODIFICATIONS
    1992/04/18 <PJB> Creation (MacBinary).
BUGS
    Not up-to-date to BcInterface/BcTypes/BcImplementation.
LEGAL
    Copyright Pascal J. Bourguignon 1992 - 1992
    All rights reserved.
    This program may not be included in any commercial product without the 
    author written permission. It may be used freely for any non-commercial 
    purpose, provided that this header is always included.
******************************************************************************/
#include <MacBinary.h>
#include <BcImplementation.h>
/* macro use C++ extensions such as & for reference parameters */


/*
    <PJB> This comment and the MacBinary_HeaderT adapted from mfrc.
    "mfrc" Information:
        Program written by Mark Harris -- Documentation by Eddy J. Gurney
        <mark@monitor.plymouth.mi.us>     <eddy@jafus.mi.org>
    Format of a MacBinary file:  (From mcvert by Doug Moore)
    A MacBinary file is composed of 128 FIELD(blocks.  The first block is the
    info_header (see below).  Then comes the data fork, null padded to fill the
    last block.  Then comes the resource fork, padded to fill the last block.  
    A proposal to follow with the text of the Get Info box has not been 
    implemented, to the best of my knowledge. Version, zero1 and zero2 are 
    what the receiving program looks at to determine if a MacBinary transfer 
    is being initiated.
*/ 
    typedef struct{     
        /*
            info file header (128 CARD8s).
            Unfortunately, these longs don't align to word boundaries
        */
        CARD8   version;  /* there is only a version 0 at this time */
        CARD8   nlen;  /* Length of filename. */
        CARD8   name[63];  /* Filename (only 1st nlen are significant)*/
        CARD8   type[4];  /* File type. */
        CARD8   auth[4];  /* File creator. */
        CARD8   flags;  /* file flags: LkIvBnSyBzByChIt */
        /* Locked, Invisible, Bundle, System, Bozo, Busy, Changed, Init */
        CARD8   zero1;
        CARD8   icon_vert[2];  /* Vertical icon position within window */
        CARD8   icon_horiz[2];  /* Horizontal icon postion in window */
        CARD8   window_id[2];  /* Window or folder ID. */
        CARD8   protect;  /* = 1 for protected file, 0 otherwise */
        CARD8   zero2;
        CARD8   dlen[4];  /* Data Fork length (CARD8s) -   most sig.  */
        CARD8   rlen[4];  /* Resource Fork length         CARD8   first */
        CARD8   ctim[4];  /* File's creation date. */
        CARD8   mtim[4];  /* File's "last modified" date. */
        CARD8   ilen[2];  /* GetInfo message length */
        CARD8   flags2;  /* Finder flags, bits 0-7 */
        CARD8   unused[14];
        CARD8   packlen[4];  /* length of total files when unpacked */
        CARD8   headlen[2];  /* length of secondary header */
        CARD8   uploadvers;  /* Version of MacBinary II that the uploading */
        /*   program is written for */
        CARD8   readvers;  /* Minimum MacBinary II version needed to read this file */
        CARD8   crc[2];  /* CRC of the previous 124 CARD8s */
        CARD8   padding[2];  /* two trailing unused CARD8s */
    } MacBinary_HeaderT;
    

    PROCEDURE(LongToBytes,(IN(long,CARD32),OUT(bytes[4],CARD8)),void)
    {
        bytes[0]=(CARD8)(long/16777216);
        bytes[1]=(CARD8)((long/65536)%256);
        bytes[2]=(CARD8)((long/256)%256);
        bytes[3]=(CARD8)(long%256);
    }/*LongToBytes*/

    PROCEDURE(BytesToLong,(IN(bytes[4],CARD8),OUT(long,CARD32)),void)
    {
        long=(((((CARD32)bytes[0])*256)+((CARD32)bytes[1]))*256
                    +((CARD32)bytes[2]))*256+((CARD32)bytes[3]);
    }/*BytesToLong*/
    
    
    PROCEDURE(MacBinary_Split,(IN(mbin,FILE*),IN(data,FILE*),IN(rsrc,FILE*)),void)
    {
    }
    
        
/*** MacBinary.c                      -- 2003-12-02 15:14:21 -- pascal   ***/

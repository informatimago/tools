/******************************************************************************
FILE:               BinHex.h
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This module can encode and decode a binhex file.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1992/05/13 <PJB> Creation.
    1993/04/28 <PJB> Updated.
LEGAL
    Copyright Pascal J. Bourguignon 1992 - 1993
    All rights reserved.
    This program may not be included in any commercial product without the 
    author written permission. It may be used freely for any non-commercial 
    purpose, provided that this header is always included.
******************************************************************************/
#ifndef __BinHex__
#define __BinHex__
#include <BcInterface.h>
#include <BcTypes.h>


    typedef struct {
        CHAR                    name[64];   /* max 63 chars*/
        CARD8                   version;
        CARD32                  type;
        CARD32                  creator;
        CARD16                  flags;      /* & 0xf800*/
        CARD32                  dataLength;
        CARD32                  rsrcLength;
    }                       BinHex_InfoT;
    
    typedef BOOLEAN (*BinHex_GetPr)(void*/*stream*/,char*/*byte*/);
        /*
            DO:         read and return a 8-bit character.
            RETURN:     true when a char could be read,
                        false when error or end-of-file.
        */

    typedef BOOLEAN (*BinHex_PutPr)(void*/*stream*/,char/*byte*/);
        /*
            DO:         write a 8-bit character.
            RETURN:     true when the char could be written,
                        false when error.
        */
        
    PROCEDURE(BinHex_Hqx8Encode,
            (BinHex_GetPr get,BinHex_PutPr put,
            void* data,void* rsrc,void* binhex,BinHex_InfoT* info),BOOLEAN)
        /*
            PRE:        info contains information about the Macintosh file.
            DO:         encode the data and rsrc streams read by get 
                         and write the binhex encoded stream with get.
            RETURN:     true if all is ok
                        false if an error occured.
        */  
        
    PROCEDURE(BinHex_Hqx8Decode,
            (BinHex_GetPr get,BinHex_PutPr put,
            void* data,void* rsrc,void* binhex,BinHex_InfoT* info),BOOLEAN)
        /*
            PRE:        info points to an BinHex_InfoT record.
            POST:       info contains information about the decoded 
                        Macintosh file.
            DO:         decode the binhex stream read by get
                         and write data and rsrc streams with get.
            RETURN:     true if all is ok
                        false if an error occured.
        */

    PROCEDURE(BinHex_Hqx7Encode,
            (BinHex_GetPr get,BinHex_PutPr put,
            void* data,void* rsrc,void* binhex,BinHex_InfoT* info),BOOLEAN)
        /*
            PRE:        info contains information about the Macintosh file.
            DO:         encode the data and rsrc streams read by get 
                         and write the binhex encoded stream with get.
            RETURN:     true if all is ok
                        false if an error occured.
        */
    
    PROCEDURE(BinHex_Hqx7Decode,
            (BinHex_GetPr get,BinHex_PutPr put,
            void* data,void* rsrc,void* binhex,BinHex_InfoT* info),BOOLEAN)
        /*
            PRE:        info points to an BinHex_InfoT record.
            POST:       info contains information about the decoded 
                        Macintosh file.
            DO:         decode the binhex stream read by get
                         and write data and rsrc streams with get.
            RETURN:     true if all is ok
                        false if an error occured.
        */
    
    PROCEDURE(BinHex_HqxInfo,
            (BinHex_GetPr get,void* binhex,BinHex_InfoT* info),BOOLEAN)
        /*
            PRE:        info points to an BinHex_InfoT record.
            POST:       info contains information about the decoded 
                        Macintosh file.
            DO:         decode the binhex stream read by get
                         and fills the info record.
            RETURN:     true if all is ok
                        false if an error occured.
        */
        
#endif
/*** BinHex.h                         --                     --          ***/

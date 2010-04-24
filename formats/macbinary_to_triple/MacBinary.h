/******************************************************************************
FILE:               MacBinary.h
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This module allows creating and reading MacBinary files.
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
#ifndef MacBinary_h
#define MacBinary_h
#include <BcInterface.h>
#include <BcTypes.h>
#include <stdio.h>


    typedef void* MacBinary_FileT;        /* opaque MacBinary file */
    typedef char  MacBinary_FileNameT[64]; /* null-terminated Mac File Name */
    typedef struct{
        MacBinary_FileNameT name;
        CARD32              type;
        CARD32              creator;
        CARD8               flags;      /* SET OF MacBinary_FlagT */
        BOOLEAN             protected;
    } MacBinary_FileInfoT;
    typedef enum{
        MacBinary_Flag_Locked=128,
        MacBinary_Flag_Invisible=64,
        MacBinary_Flag_Bundle=32,
        MacBinary_Flag_System=16,
        MacBinary_Flag_Bozo=8,
        MacBinary_Flag_Busy=4,
        MacBinary_Flag_Changed=2,
        MacBinary_Flag_Init=1
    } MacBinary_FlagT;
    
    PROCEDURE(MacBinary_InfoToMac,(char* mbfname,MacBinary_FileInfoT* mbfinfo),void)
    PROCEDURE(MacBinary_InfoFromMac,(MacBinary_FileInfoT mbfinfo,char* mbfname),void)   
    PROCEDURE(MacBinary_Create,(FILE* file),void)

#endif 
/*** MacBinary.h                      -- 2003-12-02 15:00:15 -- pascal   ***/

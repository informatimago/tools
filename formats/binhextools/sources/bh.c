/******************************************************************************
FILE:               bh.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This tool can fork or join data, resource and hqxinfo fork of a binhex
    encoded file:
        fname.hqx <-> (fname.data, fname.rsrc, fname.hqxinfo)

USAGE
    bh -keys fname
        keys : x|c|t [v] [7|8] [f hqxfile] [fname]
        same meaning as tar; t implies v and does nop.
    
        bhinfo hqxfile                  # or
        bhinfo < hqxfile                # or
        bh -tf hqxfile                  # or
        bh -t < hqxfile                 # will display the hqxinfo header.
            => 
                          name = "filename"
                          type = 'type'
                       creator = 'crea'
                         flags = $xxxx
                data fork size = 99999999999
            resource fork size = 99999999999
        
        bhdecode < hqxfile
        bhdecode hqxfile
        bh -xf[v] hqxfile [fname]       # or
        bh -x[v] [fname] < hqxfile      # will create fname.data, fname.rsrc
                                        # and fname.hqxinfo
            input hqxfile may be in Hqx7 as well as in Hqx8 format.
            if fname is not specified, it is taken from hqxfile header.
            => create fname.data, fname.rsrc and fname.hqxinfo
            
        bhencode [-7|-8] [-t TYPE] [-c CREA] fname > hqxfile
        bhencode [-7|-8] [-t TYPE] [-c CREA] fname hqxfile
        bh -cf[v][7|8] [-t TYPE] [-c CREA] hqxfile fname    # or
        bh -c[v][7|8] [-t TYPE] [-c CREA] fname > hqxfile   # at least
                                        # one of fname.data,
                                        # fname.rsrc or fname.hqxinfo
                                        # must exist.
            output file is in Hqx8 format by default.
            -7 => Hqx7 format.
            => grab fname.data, fname.rsrc and fname.hqxinfo                        

    ( -c fname [ -7 | -8 ] [ -T type ] [ -C crea ] | -x [ fname ] | -t )
         [ -v ] [ -f hqxfile ]
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1992/05/13 <PJB> Creation.
    1993/04/28 <PJB> Programmed analysis of parameters.
BUGS
    bh -t  reads the whole hqx file.
LEGAL
    Copyright Pascal J. Bourguignon 1992 - 1993
    All rights reserved.
    This program may not be included in any commercial product without the 
    author written permission. It may be used freely for any non-commercial 
    purpose, provided that this header is always included.
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <BcInterface.h>
#include <BcTypes.h>
#include <BinHex.h>
#include <BcImplementation.h>

#if 0
    static BOOLEAN bhget(void* file,char* byte)
    {
        return(fread(byte,sizeof(char),1,(FILE*)file)==1);          
    }/*bhget*/

    static BOOLEAN bhput(void* file,char byte)
    {
        return(fwrite(&byte,sizeof(char),1,(FILE*)file)==1);            
    }/*bhput*/
#endif


    static CHAR Usage[]=
                    "### Usage:\n#\tbh "
                    "(-c fname [-7|-8] [-T type] [-C crea]|-x [fname]|-t) \n"
                    "#\t\t[-v] [-f hqxfile]\n"
                    "#\tbhinfo   <=> bh -t\n"
                    "#\tbhencode <=> bh -c\n"
                    "#\tbhdecode <=> bh -x\n";
    
    
PROCEDURE(main,(int argc,char** random_argv),int)
{
    CHAR**          argv=(CHAR**)random_argv;
    FILE*           fhqx;
        
    /*
        ( -c fname [ -7 | -8 ] [ -T type ] [ -C crea ] | -x [ fname ] | -t )
        [ -v ] [ -f hqxfile ]
    */
    BOOLEAN     nooption=TRUE;      /* -c or -x or -t mandatory!*/
    BOOLEAN     create=FALSE;       /* -c*/
    BOOLEAN     sevenSet=FALSE;
    BOOLEAN     seven=FALSE;        /* -7|-8*/
    BOOLEAN     typeSet=FALSE;      /* -T*/
    CARD32      type=0;
    BOOLEAN     creatorSet=FALSE;   /* -C*/
    CARD32      creator=0;
    BOOLEAN     extract=FALSE;      /* -x*/
    CHAR*       fname=NIL;          /* (NIL or fname)*/
    BOOLEAN     getinfo=FALSE;      /* -t*/
    BOOLEAN     verbose=FALSE;      /* -v*/
    CHAR*       hqxname=NIL;        /* -f (NIL or hqxname)*/
        
    CARD16      i;
    CARD16      j;
        
    i=1;
    while(i<argc){
        if(argv[i][0]=='-'){
            if(argv[i][2]!=(CHAR)0){
                fprintf(stderr,"### Invalid option: %s\n%s",argv[i],Usage);
                return(1);
            }
            switch(argv[i][1]){
            case 'c':
            case 'x':
            case 't':
                if(!nooption){
                    fprintf(stderr,"### Only one option from {-c, -x, -t} "
                                    "at once.\n%s",Usage);
                    return(1);
                }
                nooption=FALSE;
                switch(argv[i][1]){
                case 'c':
                    create=TRUE;
                    j=i+1;
                    if(j<argc){
                        fname=argv[j];
                    }else{
                        fprintf(stderr,"### -c must be followed by the "
                                        "file name\n%s",Usage);
                        return(1);
                    }
                    i=j;
                    break;
                case 'x':
                    extract=TRUE;
                    j=i+1;
                    if(j<argc){
                        if(argv[j][0]!='-'){
                            fname=argv[j];
                            i=j;
                        }
                    }
                    break;
                case 't':
                    getinfo=TRUE;
                    break;
                }
                break;
            case '7':
            case '8':
                if(!create){
                    fprintf(stderr,"### -7 or -8 can only be used "
                                    "after -c\n%s",Usage);
                    return(1);
                }
                if(sevenSet){
                    fprintf(stderr,"### Only one option from {-7, -8} "
                                    "at once.\n%s",Usage);
                    return(1);
                }
                sevenSet=TRUE;
                seven=argv[i][1]=='7';
                break;
            case 'T':
            case 'C':
                if(!create){
                    fprintf(stderr,"### -T or -C can only be used "
                                    "after -c\n%s",Usage);
                    return(1);
                }
                j=i+1;
                if(j<argc){
                    if(strlen((char*)(argv[j]))!=4){
                        fprintf(stderr,"### -T or -C must be followed by "
                                        "a four-character string\n%s",Usage);
                        return(1);
                    }
                    if(argv[i][1]=='T'){
                        if(typeSet){
                            fprintf(stderr,"### -T may be specified "
                                            "only once\n%s",Usage);
                            return(1);
                        }
                        type=*(CARD32*)(argv[j]);
                        typeSet=TRUE;
                    }else{
                        if(creatorSet){
                            fprintf(stderr,"### -T may be specified "
                                            "only once\n%s",Usage);
                            return(1);
                        }
                        creator=*(CARD32*)(argv[j]);
                        creatorSet=TRUE;
                    }
                }else{
                    fprintf(stderr,"### -T or -C must be followed by a type "
                                    " or a creator (4 char)\n%s",Usage);
                    return(1);
                }
                i=j;
                break;
            case 'f':
                if(hqxname!=NIL){
                    fprintf(stderr,"### -f may be specified "
                                    "only once\n%s",Usage);
                    return(1);
                }
                j=i+1;
                if(j<argc){
                    hqxname=argv[j];
                }else{
                    fprintf(stderr,"### -f must be followed by the hqx "
                                    "file name\n%s",Usage);
                    return(1);
                }
                i=j;
                break;
            case 'v':
                if(verbose){
                    fprintf(stderr,"### -v may be specified "
                                    "only once\n%s",Usage);
                    return(1);
                }
                verbose=TRUE;
                break;
            default:
                fprintf(stderr,"### Invalid option: %s\n%s",argv[i],Usage);
                return(1);
            }
        }
        INC(i);
    }
    if(nooption){
        fprintf(stderr,"### At least one of {-c, -x, -t} must be "
                        "specified.\n%s",Usage);
        return(1);
    }
    
/*----------------------------------------------
    ( -c fname [ -7 | -8 ] [ -T type ] [ -C crea ] | -x [ fname ] | -t )
         [ -v ] [ -f hqxfile ]

-c fname -f hqx     input (fname.data,fname.rsrc,fname.hqxinfo) output hqx
-c fname > hqx      input (fname.data,fname.rsrc,fname.hqxinfo) output stdout
-x fname -f hqx     input hqx   output (fname.data,fname.rsrc,fname.hqxinfo) 
-x fname < hqx      input stdin output (fname.data,fname.rsrc,fname.hqxinfo)
-x -f hqx           input hqx   output (fnhqx.data,fnhqx.rsrc,fnhqx.hqxinfo)
-x < hqx            input stdin output (fnhqx.data,fnhqx.rsrc,fnhqx.hqxinfo)
    
*/
    if(create){
        printf("create ");
        printf("fname=%s ",fname);
        printf("%s ",seven?"seven":"eight");
        if(typeSet){
            printf("type=%08lx ",type);
        }
        if(creatorSet){
            printf("creator=%08lx ",creator);
        }
    }
    if(extract){
        printf("extract ");
        if(fname!=NIL){
            printf("fname=%s ",fname);
        }
    }
    if(getinfo){
        printf("getinfo ");
    }
    if(verbose){
        printf("verbose ");
    }
    if(hqxname){
        printf("hqxname=%s ",hqxname);
    }
    printf("\n");
    return(0);

    if(create){
        if(hqxname==NIL){
            fhqx=stdout;
        }else{
            fhqx=fopen((char*)(hqxname),"w");
            if(fhqx==NIL){
                fprintf(stderr,"### Cannot create output file %s\n",hqxname);
                return(2);
            }
        }
        if(seven){
        
        }else{
        
        }
    }else{
        if(hqxname==NIL){
            fhqx=stdin;
        }else{
            fhqx=fopen((char*)(hqxname),"r");
            if(fhqx==NIL){
                fprintf(stderr,"### Cannot open input file %s\n",hqxname);
                return(2);
            }
        }
        if(extract){
        
        }else{
        
        }
    }
    
    
    return(0);
}/*main*/

/*** bh.c                             --                     --          ***/

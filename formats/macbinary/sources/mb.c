/******************************************************************************
FILE:               mb.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This tool can fork or join data, resource and mbinfo fork of a MacBinary-2
    encoded file:
        fname.mb <-> (fname.data, fname.rsrc, fname.mbinfo)

USAGE
    mb  ( -c fname [ -T type ] [ -C crea ] | -x [ fname ] | -t ) 
            [ -v ] [ -f mbfile ]

        same meaning as tar; -t implies v and does nop.
    
        mbinfo mbfile                   # or
        mbinfo < mbfile                 # or
        mb -t -f mbfile                 # or
        mb -t < mbfile                  # will display the mbinfo header.
            => 
                          name = "filename"
                          type = 'type'
                       creator = 'crea'
                         flags = $xxxx
                data fork size = 99999999999
            resource fork size = 99999999999
        
        mbdecode < mbfile
        mbdecode mbfile
        mb -x [-v] -f mbfile [fname]    # or
        mb -x [-v] [fname] < mbfile     # will create fname.data, fname.rsrc
                                        # and fname.mbinfo
            input mbfile may be in mb7 as well as in mb8 format.
            if fname is not specified, it is taken from mbfile header.
            => create fname.data, fname.rsrc and fname.mbinfo
            
        mbencode [-t TYPE] [-c CREA] fname > mbfile
        mbencode [-t TYPE] [-c CREA] fname mbfile
        mb -c fname [-t TYPE] [-c CREA] [-v] -f mbfile  # or
        mb -c fname [-t TYPE] [-c CREA] [-v]  > mbfile  # at least
                                        # one of fname.data,
                                        # fname.rsrc or fname.mbinfo
                                        # must exist.
            output file is in mb8 format by default.
            => grab fname.data, fname.rsrc and fname.mbinfo                     
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1993/05/04 <PJB> Creation. (From bh.c).
LEGAL
    Copyright Pascal J. Bourguignon 1992 - 2011
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
#include <BcImplementation.h>

#define roundup(size,modulo)    ((((size)+(modulo)-1)/(modulo))*(modulo))
#define BUFFERSIZE  (16*1024)

#ifdef __BIG_ENDIAN__

static PROCEDURE(fromCARD16,(unsigned int /*CARD16*/ from,CARD8* to),void)
{
    to[0]=from>>8;
    to[1]=from&0xff;
}/*fromCARD16;*/
    

static PROCEDURE(toCARD16,(CARD8* from),CARD16)
{
    return((((CARD16)(from[0]))<<8)|(CARD16)(from[1]));
}/*toCARD16;*/
    

static PROCEDURE(fromCARD32,(CARD32 from,CARD8* to),void)
{
    to[0]=from>>24;
    to[1]=(from>>16)&0xff;
    to[2]=(from>>8)&0xff;
    to[3]=from&0xff;
}/*fromCARD16;  */


static PROCEDURE(toCARD32,(CARD8* from),CARD32)
{
    return((((CARD32)(from[0]))<<24)
           |(((CARD32)(from[1]))<<16)
           |(((CARD32)(from[2]))<<8)
           |(CARD32)(from[3]));
}/*toCARD16;*/

#else

static PROCEDURE(fromCARD16,(unsigned int /*CARD16*/  from,CARD8* to),void)
{
    to[1]=(CARD8)(from>>8);
    to[0]=(CARD8)(from&0xff);
}/*fromCARD16;*/
    

static PROCEDURE(toCARD16,(CARD8* from),CARD16)
{
    return((CARD16)((from[1]<<8)|from[0]));
}/*toCARD16;*/
    

static PROCEDURE(fromCARD32,(CARD32 from,CARD8* to),void)
{
    to[3]=(CARD8)(from>>24);
    to[2]=(CARD8)((from>>16)&0xff);
    to[1]=(CARD8)((from>>8)&0xff);
    to[0]=(CARD8)(from&0xff);
}/*fromCARD32;  */

static PROCEDURE(toCARD32,(CARD8* from),CARD32)
{
    return((CARD32)((from[3]<<24)
                    |(from[2]<<16)
                    |(from[1]<<8)
                    |from[0]));
}/*toCARD32;*/
    
#endif
 
#if 0

static PROCEDURE(blockclear,(CARD8* memory,CARD32 size),void)
{
    while(size>0){
        (*memory)=0;
        memory++;
        size--;
    }
}/*blockclear;*/
    

static PROCEDURE(blockcopy,(CARD8* from,CARD8* to,CARD32 size),void)
{
    while(size>0){
        (*to)=(*from);
        to++;
        from++;
        size--;
    }
}/*blockcopy;*/

#else
#define blockclear  bzero
#define blockcopy   bcopy
#endif
    
/*////////////////////////////////////////////////////////////////*/


typedef struct {
    CHAR                name[64];   /* C string, strlen(name) in [1..63]*/
    /* Finder Info:                 // Macintosh encoding.*/
    CARD8               type[4];
    CARD8               creator[4];
    CARD16              finderFlags;
    CARD16              vertical;
    CARD16              horizontal;
    CARD16              folderId;
    /* End of Finder Info.*/
    CARD8               filler;
    CARD8               protectedBit;   /* low-bit.*/
    CARD32              creationDate;
    CARD32              modificationDate;
    CARD32              dataSize;       /* in the MacBinary file.*/
    CARD32              resourceSize;   /* in the MacBinary file.*/
    CARD32              totalSize;      /* as decompressed Macintosh file.*/
    CARD16              commentSize;
    CARD16              headerSize;
}                   MacBinary_FileInfoT;

typedef struct {
    CARD8               version0;       /* =0*/
    CARD8               nameLength;
    CHAR                name[63];
    CHAR                type[4];
    CHAR                creator[4];
    CARD8               finderFlagH;
    CARD8               filler74;       /* =0*/
    CARD8               vertical[2];    /* file's position in its window*/
    CARD8               horizontal[2];
    CARD8               folderId[2];
    CARD8               protectedBit;
    CARD8               filler82;       /* =0*/
    CARD8               dataSize[4];
    CARD8               resourceSize[4];
    CARD8               creationDate[4];
    CARD8               modificationDate[4];
    CARD8               commentSize[2]; /* MB2*/
    CARD8               finderFlagL;    /* MB2*/
    CARD8               filler102[14];  /* MB2*/
    CARD32              totalSize;      /* MB2*/
    CARD16              headerSize;     /* MB2*/
    CARD8               writerVersion;  /* MB2*/
    CARD8               readerVersion;  /* MB2*/
    CARD16              checksum;       /* MB2*/
    CARD8               computerType;   /* =0*/
    CARD8               OSID;           /* =0*/
}                   MacBinary_HeaderT;
    
/* secondary header                     // MB2*/
/* data fork*/
/* resource fork*/
/* comment                              // MB2*/

/* ((headerSize=127)/128)*128       */



static PROCEDURE(MacBinary_CheckSum,(MacBinary_HeaderT* header),CARD16)
{
    CARD8*          h;
    CARD16          sum;
    CARD16          i;
            
    h=(CARD8*)header;
    sum=0;
    i=126;
    while(i>0){
        INCR(sum,(*h));
        h++;
        i--;
    }
    return(sum);
}/*MacBinary_CheckSum;*/
    
    
static PROCEDURE(MacBinary_HeaderFormat,(MacBinary_HeaderT* header),CARD16)
{
    if((header->version0==0)&&(header->filler74==0)){
        if(header->checksum==MacBinary_CheckSum(header)){
            /* this is a MacBinary 2 header.*/
            return(2);
        }else if(header->filler82==0){
            /* this is a MacBinary 1 header.*/
            return(1);
            /* all bytes from finderFlagL to checksum should be 0.*/
            /* namelength in [1..63].*/
            /* dataSize and resourceSize in [0..$7fffff].*/
        }else{
            /* this is not a valid MacBinary header.*/
            return(0);
        }
    }else{
        /* this is not a valid MacBinary header.*/
        return(0);
    }
}/*MacBinary_HeaderFormat;*/
    
    
static PROCEDURE(MacBinary_BuildHeader,
                 (MacBinary_HeaderT* header,MacBinary_FileInfoT* finfo),void)
{
    CARD8       finderFlag[2];
        
    header->version0=0;
    fromCARD16(finfo->finderFlags,&(finderFlag[0]));
    blockclear(&(header->name[0]),sizeof(header->name));
    header->nameLength=(CARD8)strlen((char*)(finfo->name));
    strcpy((char*)(header->name),(char*)(finfo->name));
    /* SEE: following two lines assume sizeof(CARD8)=1*/
    blockcopy(&(finfo->type[0]),&(header->type[0]),sizeof(header->type));
    blockcopy(&(finfo->creator[0]),&(header->creator[0]),
              sizeof(header->creator));
    header->finderFlagH=finderFlag[0];
    header->filler74=0;
    fromCARD16(finfo->vertical,&(header->vertical[0]));
    fromCARD16(finfo->horizontal,&(header->horizontal[0]));
    fromCARD16(finfo->folderId,&(header->folderId[0]));
    header->protectedBit=finfo->protectedBit;
    header->filler82=0;
    fromCARD32(finfo->dataSize,&(header->dataSize[0]));
    fromCARD32(finfo->resourceSize,&(header->resourceSize[0]));
    fromCARD32(finfo->creationDate,(CARD8*)(&(header->creationDate)));
    fromCARD32(finfo->modificationDate,
               (CARD8*)(&(header->modificationDate)));
    fromCARD16(finfo->commentSize,(CARD8*)(&(header->commentSize)));
    fromCARD32(finfo->totalSize,(CARD8*)(&(header->totalSize)));
    fromCARD16(finfo->headerSize,(CARD8*)(&(header->headerSize)));
    header->finderFlagL=finderFlag[1];
    header->writerVersion=129;
    header->readerVersion=129;
    /* checksum is only for byte preceding checksum*/
    header->checksum=MacBinary_CheckSum(header);
    header->computerType=0;
    header->OSID=0;
}/*MacBinary_BuildHeader;*/
    

static PROCEDURE(MacBinary_HeaderToInfo,
                 (MacBinary_HeaderT* header,MacBinary_FileInfoT* finfo),void)
{
    CARD8       finderFlag[2];

    blockclear(&(finfo->name[0]),sizeof(finfo->name));
    strcpy((char*)(finfo->name),(char*)(header->name));
    blockcopy(&(header->type[0]),&(finfo->type[0]),sizeof(finfo->type));
    blockcopy(&(header->creator[0]),&(finfo->creator[0]),
              sizeof(finfo->creator));
    finderFlag[0]=header->finderFlagH;
    finderFlag[1]=header->finderFlagL;
    finfo->finderFlags=toCARD16(&(finderFlag[0]));
    finfo->vertical=toCARD16(&(header->vertical[0]));
    finfo->horizontal=toCARD16(&(header->horizontal[0]));
    finfo->folderId=toCARD16(&(header->folderId[0]));
    finfo->filler=0;
    finfo->protectedBit=header->protectedBit;
    finfo->creationDate=toCARD32(&(header->creationDate[0]));
    finfo->modificationDate=toCARD32(&(header->modificationDate[0]));
    finfo->dataSize=toCARD32(&(header->dataSize[0]));
    finfo->resourceSize=toCARD32(&(header->resourceSize[0]));
    finfo->commentSize=toCARD16((CARD8*)(&(header->commentSize)));
    finfo->totalSize=toCARD32((CARD8*)(&(header->totalSize)));
    finfo->headerSize=toCARD16((CARD8*)(&(header->headerSize)));
}/*MacBinary_HeaderToInfo;*/

    

static PROCEDURE(MacBinary_CopyFork,
                 (FILE* from,FILE* to,CARD32 size,
                  BOOLEAN readPadding,BOOLEAN writePadding),BOOLEAN)
/*
  RETURN:     TRUE = no error, FALSE = error.
*/
{
    CARD8       buffer[BUFFERSIZE];
    CARD32      padding;
    CARD32      count;
        
    padding=(readPadding||writePadding)?(roundup(size,128)-size):(0);
    while(size>=BUFFERSIZE){
        count=(CARD32)fread(buffer,sizeof(CARD8),BUFFERSIZE,from);
        if(count!=BUFFERSIZE){ return(FALSE); }
        count=(CARD32)fwrite(buffer,sizeof(CARD8),BUFFERSIZE,to);
        if(count!=BUFFERSIZE){ return(FALSE); }
        size-=BUFFERSIZE;
    }
    if(size>0){
        count=(CARD32)fread(buffer,sizeof(CARD8),size,from);
        if(count!=size){ return(FALSE); }
        count=(CARD32)fwrite(buffer,sizeof(CARD8),size,to);
        if(count!=size){ return(FALSE); }
    }
    if(padding>0){
        if(readPadding){
            count=(CARD32)fread(buffer,sizeof(CARD8),padding,from);
            if(count!=padding){ return(FALSE); }
        }
        if(writePadding){
            blockclear(buffer,padding);
            count=(CARD32)fwrite(buffer,sizeof(CARD8),padding,to);
            if(count!=padding){ return(FALSE); }
        }
    }
    return(TRUE);
}/*MacBinary_CopyFork;*/
    
    
static PROCEDURE(MacBinary_Skip,
                 (FILE* from,CARD32 size,BOOLEAN readPadding),BOOLEAN)
{
    CARD8       buffer[BUFFERSIZE];
    CARD32      padding;
    CARD32      count;
        
    padding=(readPadding)?(roundup(size,128)-size):(0);
    while(size>=BUFFERSIZE){
        count=(CARD32)fread(buffer,sizeof(CARD8),BUFFERSIZE,from);
        if(count!=BUFFERSIZE){ return(FALSE); }
        size-=BUFFERSIZE;
    }
    if(size>0){
        count=(CARD32)fread(buffer,sizeof(CARD8),size,from);
        if(count!=size){ return(FALSE); }
    }
    if(padding>0){
        count=(CARD32)fread(buffer,sizeof(CARD8),padding,from);
        if(count!=size){ return(FALSE); }
    }
    return(TRUE);
}/*MacBinary_Skip;*/
    
    

static const char*      Macintosh_to_Next[256]=
    {
        "\000","\001","\002","\003","\004","\005","\006","\007",
        "\010","\011","\012","\013","\014","\015","\016","\017",
        "\020","\021","\022","\023","\024","\025","\026","\027",
        "\030","\031","\032","\033","\034","\035","\036","\037",
        " ","!","\"","#","$","%","&","'","(",")","*","+",",","-",".","/",
        "0","1","2","3","4","5","6","7","8","9",":",";","<","=",">","?",
        "@","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O",
        "P","Q","R","S","T","U","V","W","X","Y","Z","[","\\","]","^","_",
        "`","a","b","c","d","e","f","g","h","i","j","k","l","m","n","o",
        "p","q","r","s","t","u","v","w","x","y","z","{","|","}","~","\177",
        "Ö","Ü","á","â","ë","ñ","ö","÷","’","◊","Ÿ","ÿ","⁄","€","›","‹",
        "ﬁ","ﬂ","‚","‡","‰","Â","Á","Ì","Ï","Ó","","Ô","Û","Ú","Ù","ˆ",
        "≤"," ","¢","£","ß","∑","∂","˚","∞","†",NIL,"¬","»",NIL,"·","È",
        NIL,"—",NIL,NIL,"•","ù","Ê",NIL,NIL,NIL,NIL,"„","Î",NIL,"Ò","˘",
        "ø","°","æ",NIL,"¶",NIL,NIL,"´","ª","º","Ä","Å","Ñ","ï","Í","˙",
        "±","–","™","∫","`","'","ü",NIL,"˝",NIL,"§","®","¨","≠","Æ","Ø",
        "≥","¥","∏","π","Ω","É","ä","Ç","ã","à","ç","é","è","å","ì","î",
        NIL,"í","ò","ô","ó","ı","√","ƒ","≈","∆","«"," ","À","Õ","Œ","œ"
    };


#if 0
static const char*      Next_to_Macintosh[256]=
    {
        "\000","\001","\002","\003","\004","\005","\006","\007",
        "\010","\011","\012","\013","\014","\015","\016","\017",
        "\020","\021","\022","\023","\024","\025","\026","\027",
        "\030","\031","\032","\033","\034","\035","\036","\037",
        " ","!","\"","#","$","%","&","'","(",")","*","+",",","-",".","/",
        "0","1","2","3","4","5","6","7","8","9",":",";","<","=",">","?",
        "@","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O",
        "P","Q","R","S","T","U","V","W","X","Y","Z","[","\\","]","^","_",
        "`","a","b","c","d","e","f","g","h","i","j","k","l","m","n","o",
        "p","q","r","s","t","u","v","w","x","y","z","{","|","}","~","\177",
        " ","À","Á","Â","Ã","Ä","Å","Ç","È","É","Ê","Ë","Ì","Í","Î","Ï",
        NIL,"Ñ","Ò","Ó","Ô","Õ","Ö","Ù","Ú","Û","Ü",NIL,NIL,"µ",NIL,"÷",
        "©","¡","¢","£","⁄","¥","ƒ","§","€","'","“","«","‹","›","ﬁ","ﬂ",
        "®","–","†","‡","·",NIL,"¶","•","‚","„","”","»","…","‰","¬","¿",
        NIL,"‘","’","ˆ","˜","¯","˘","˙","¨",NIL,"˚","¸",NIL,"˝","˛","ˇ",
        "—","±",NIL,NIL,NIL,"à","á","â","ã","ä","å","ç","è","é","ê","ë",
        "ì","Æ","í","ª","î","ï","∂","ñ",NIL,"Ø","Œ","º","ò","ó","ô","õ",
        "ö","æ","ù","ú","û","ı","ü",NIL,NIL,"ø","œ","ß",NIL,"ÿ",NIL,NIL
    };

static PROCEDURE(FName_NextToMac,(CHAR* fname),void)
{
    CARD16      i;
    const CHAR* c;
                
    i=0;
    while(fname[i]!=(CHAR)0){
        c=Next_to_Macintosh[fname[i]];
        fname[i]=(c==NIL)?('_'):(*c);
        if(fname[i]<' '){
            fname[i]='_';
        }else if(fname[i]==':'){
            fname[i]='/';
        }
        i++;
    }
    if(fname[0]=='.'){
        fname[0]='_';
    }
}/*FName_NextToMac;*/
#endif    
    
static PROCEDURE(FName_MacToNext,(CHAR* fname),void)
{
    CARD16      i;
    const CHAR* c;
                
    i=0;
    while(fname[i]!=(CHAR)0){
        c=(const CHAR*)Macintosh_to_Next[fname[i]];
        fname[i]=(CHAR)((c==NIL)?('_'):(*c));
        if(fname[i]<=' '){
            fname[i]='_';
        }else if(fname[i]=='/'){
            fname[i]=':';
        }
        i++;
    }
}/*FName_MacToNext;*/
    
    
static PROCEDURE(forkopen,
                 (const CHAR* fork,const CHAR* fname,
                  const CHAR* extension,const CHAR* mode,
                  BOOLEAN mandatory),FILE*)
{
    CHAR        name[1024];
    FILE*       file;
            
    strcpy((char*)(name),(const char*)(fname));
    strcat((char*)(name),(const char*)(extension));
    file=fopen((char*)(name),(const char*)mode);
    if((file==NIL)&&(mandatory)){
        fprintf(stderr,"Error while opening %s fork (%s)\n",fork,name);
    }
    return(file);
}/*forkopen;*/


static PROCEDURE(MacBinary_Create,
                 (FILE* fmbin,const CHAR* fname,
                  const CARD8* type,const CARD8* creator,BOOLEAN verbose),BOOLEAN)
{
    MacBinary_HeaderT       header;
    MacBinary_FileInfoT     info;
        
    FILE*                   fdata;
    FILE*                   frsrc;
    FILE*                   finfo;
            
    CARD32                  count;
            
    blockclear((CARD8*)(&info),sizeof(info));
    finfo=forkopen((const CHAR*)"information",fname,
                   (const CHAR*)".mbinfo",(const CHAR*)"r",FALSE);
    if(finfo!=NIL){
        count=(CARD32)fread(&info,1,sizeof(info),finfo);
        if(count!=sizeof(info)){
            blockclear((CARD8*)(&info),sizeof(info));
        }
    }
        
    fdata=forkopen((const CHAR*)"data",fname,
                   (const CHAR*)"",(const CHAR*)"r",FALSE);
    if(fdata==NIL){
        info.dataSize=0;
    }else{
        fseek(fdata,0,SEEK_END);
        info.dataSize=(CARD32)ftell(fdata);
        fseek(fdata,0,SEEK_SET);
    }
        
    frsrc=forkopen((const CHAR*)"resource",fname,
                   (const CHAR*)".rsrc",(const CHAR*)"r",FALSE);
    if(frsrc==NIL){
        info.resourceSize=0;
    }else{
        fseek(frsrc,0,SEEK_END);
        info.resourceSize=(CARD32)ftell(frsrc);
        fseek(frsrc,0,SEEK_SET);
    }
        
    if((finfo==NIL)&&(fdata==NIL)&&(frsrc==NIL)){
        fprintf(stderr,"# There is no data fork named %s\n",fname);
        return(FALSE);
    }
        
    /* SEE: should set creationDate and modificationDate.*/
    /* SEE: should set other Finder info.*/
        
    strcpy((char*)(info.name),(const char*)(fname));
    if(type!=NIL){
        blockcopy(type,&(info.type),sizeof(info.type));
    }
    if(creator!=NIL){
        blockcopy(creator,&(info.creator),sizeof(info.creator));
    }
        
    MacBinary_BuildHeader(&header,&info);
        
    count=(CARD32)fwrite(&header,1,sizeof(header),fmbin);
    if(count!=sizeof(header)){ return(FALSE); }
        
    if(info.dataSize>0){
        if(!MacBinary_CopyFork(fdata,fmbin,info.dataSize,FALSE,TRUE)){
            return(FALSE);
        }
    }
        
    if(info.resourceSize>0){
        if(!MacBinary_CopyFork(frsrc,fmbin,info.resourceSize,FALSE,TRUE)){
            return(FALSE);
        }
    }
        
    return(TRUE);
}/*MacBinary_Create; */


static PROCEDURE(PrintInfo,(FILE* output,MacBinary_FileInfoT* info),void)
{
    fprintf(output,"                 name = %s\n",info->name);
    fprintf(output,"                 type = %08"FMT_CARD32_HEX"\n",*(CARD32*)&(info->type));
    fprintf(output,"              creator = %08"FMT_CARD32_HEX"\n",*(CARD32*)&(info->creator));
    fprintf(output,"         finder flags = %04"FMT_CARD16_HEX"\n",(info->finderFlags));
    fprintf(output,"             vertical = %"FMT_CARD16"\n",info->vertical);
    fprintf(output,"           horizontal = %"FMT_CARD16"\n",info->horizontal);
    fprintf(output,"             folderId = %"FMT_CARD16"\n",info->folderId);
    fprintf(output,"            protected = %"FMT_CARD16"\n",(info->protectedBit));
    fprintf(output,"        creation date = %"FMT_CARD32"\n",info->creationDate);
    fprintf(output,"    modification date = %"FMT_CARD32"\n",info->modificationDate);
    fprintf(output,"            data size = %"FMT_CARD32"\n",info->dataSize);
    fprintf(output,"        resource size = %"FMT_CARD32"\n",info->resourceSize);
    fprintf(output,"           total size = %"FMT_CARD32"\n",info->totalSize);
    fprintf(output,"         comment size = %"FMT_CARD16"\n",info->commentSize);
    fprintf(output,"secondary header size = %"FMT_CARD16"\n",info->headerSize);
}/*PrintInfo;*/
    
    
static PROCEDURE(MacBinary_GetInfo,
                 (FILE* fmbin,BOOLEAN verbose,
                  MacBinary_FileInfoT* info),BOOLEAN)
{
    MacBinary_HeaderT       header;
    CARD32                  count;
    CARD16                  format;
            
    count=(CARD32)fread(&header,sizeof(CARD8),sizeof(header),fmbin);
    if(count!=sizeof(header)){
        fprintf(stderr,"Cannot read the header.\n");
        return(FALSE);
    }
    format=MacBinary_HeaderFormat(&header);
    switch(format){
    case 1:
        if(verbose){
            fprintf(stderr,"Header format: MacBinary 1 format.\n");
        }
        break;
    case 2:
        if(verbose){
            fprintf(stderr,"Header format: MacBinary 2 format,\n"
                    "   writer version= %u\n"
                    "   reader version= %u\n",
                    (header.writerVersion),
                    (header.readerVersion));
        }
        if(header.readerVersion>129){
            fprintf(stderr,"I cannot read this version (%d).\n",
                    (CARD16)(header.readerVersion));
            return(FALSE);
        }
        break;
    default:
        fprintf(stderr,"It is not a MacBinary header.\n");
        return(FALSE);
    }
    MacBinary_HeaderToInfo(&header,info);
    return(TRUE);       
}/*MacBinary_GetInfo;*/
    
    
static PROCEDURE(MacBinary_Extract,
                 (FILE* fmbin,const CHAR* fname,BOOLEAN verbose),BOOLEAN)
{
    MacBinary_FileInfoT     info;
    CARD32                  count;
    BOOLEAN                 dontoverride=FALSE; /* useless.*/
    FILE*                   fdata;
    FILE*                   frsrc;
    FILE*                   finfo;

    if(!MacBinary_GetInfo(fmbin,verbose,&info)){
        return(FALSE);
    }
    if(fname==NIL){
        fname=info.name;
        FName_MacToNext(info.name);
        dontoverride=TRUE;
        if(verbose){
            fprintf(stderr,"Using file name found in header: %s "
                    "(converted to NeXT encoding)\n",fname);
        }
    }else{
        if(verbose){
            fprintf(stderr,"Using given file name: %s\n",fname);
        }
    }       
    if(info.headerSize>0){
        if(!MacBinary_Skip(fmbin,info.headerSize,TRUE)){
            fprintf(stderr,"Error while skipping secondary header.\n");
            return(FALSE);
        }
    }
    if(info.dataSize>0){
        fdata=forkopen((const CHAR*)"data",fname,
                       (const CHAR*)"",(const CHAR*)"w",TRUE);
        if(fdata==NIL){
            return(FALSE);
        }
    }else{
        fdata=NIL;
    }
    if(info.resourceSize>0){
        frsrc=forkopen((const CHAR*)"resource",fname,
                       (const CHAR*)".rsrc",(const CHAR*)"w",TRUE);
        if(frsrc==NIL){
            return(FALSE);
        }
    }else{
        frsrc=NIL;
    }
    finfo=forkopen((const CHAR*)"information",fname,
                   (const CHAR*)".mbinfo",
                   (const CHAR*)"w",TRUE);
    if(fdata!=NIL){
        if(!MacBinary_CopyFork(fmbin,fdata,info.dataSize,TRUE,FALSE)){
            fprintf(stderr,"Error while copying the data fork.\n");
            return(FALSE);
        }
    }
    if(frsrc!=NIL){
        if(!MacBinary_CopyFork(fmbin,frsrc,info.resourceSize,TRUE,FALSE)){
            fprintf(stderr,"Error while copying the resource fork.\n");
            return(FALSE);
        }
    }
    count=(CARD32)fwrite(&info,1,sizeof(info),finfo);
    if(count!=sizeof(info)){
        fprintf(stderr,"Error while writting information fork.\n");
        return(FALSE);
    }
    /* forgetting the comment.*/
    /* SEE: could update the creation and modification dates.*/
    return(TRUE);
}/*MacBinary_Extract; */


static CHAR Usage[]=
  "### Usage:\n#\tmb "
  "(-c fname [-T type] [-C crea]|-x [fname]|-t [-F]) \n"
  "#\t\t[-v] [-f mbfile]\n"
  "#\tmbinfo   <=> mb -t\n"
  "#\tmbencode <=> mb -c\n"
  "#\tmbdecode <=> mb -x\n";
    
/*
  ( -c fname [ -T type ] [ -C crea ] | -x [ fname ] | -t )
  [ -v ] [ -f mbfile ]

  -c fname -f mb      input (fname.data,fname.rsrc,fname.mbinfo) output mb
  -c fname > mb       input (fname.data,fname.rsrc,fname.mbinfo) output stdout
  -x fname -f mb      input mb    output (fname.data,fname.rsrc,fname.mbinfo) 
  -x fname < mb       input stdin output (fname.data,fname.rsrc,fname.mbinfo)
  -x -f mb            input mb    output (fnmb.data,fnmb.rsrc,fnmb.mbinfo)
  -x < mb             input stdin output (fnmb.data,fnmb.rsrc,fnmb.mbinfo)
    
*/


PROCEDURE(main,(int argc,char** random_argv),int)
{
    CHAR** argv=(CHAR**)random_argv;
    FILE*                   fmbin;
    
    BOOLEAN     nooption=TRUE;      /* -c or -x or -t mandatory!*/
    BOOLEAN     create=FALSE;       /* -c*/
    CARD8*      type=NIL;
    CARD8*      creator=NIL;
    BOOLEAN     extract=FALSE;      /* -x*/
    CHAR*       fname=NIL;          /* (NIL or fname)*/
    BOOLEAN     getinfo=FALSE;      /* -t*/
    BOOLEAN     verbose=FALSE;      /* -v*/
    CHAR*       mbname=NIL;         /* -f (NIL or mbname)*/
    BOOLEAN     force=FALSE;        /* -F force dump info.*/
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
                    j=(CARD16)(i+1);
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
                    j=(CARD16)(i+1);
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
            case 'T':
            case 'C':
                if(!create){
                    fprintf(stderr,"### -T or -C can only be used "
                            "after -c\n%s",Usage);
                    return(1);
                }
                j=(CARD16)(i+1);
                if(j<argc){
                    if(strlen((char*)(argv[j]))!=4){
                        fprintf(stderr,"### -T or -C must be followed by "
                                "a four-character string\n%s",Usage);
                        return(1);
                    }
                    if(argv[i][1]=='T'){
                        if(type!=NIL){
                            fprintf(stderr,"### -T may be specified "
                                    "only once\n%s",Usage);
                            return(1);
                        }
                        type=argv[j];
                    }else{
                        if(creator!=NIL){
                            fprintf(stderr,"### -T may be specified "
                                    "only once\n%s",Usage);
                            return(1);
                        }
                        creator=argv[j];
                    }
                }else{
                    fprintf(stderr,"### -T or -C must be followed by a type "
                            " or a creator (4 char)\n%s",Usage);
                    return(1);
                }
                i=j;
                break;
            case 'f':
                if(mbname!=NIL){
                    fprintf(stderr,"### -f may be specified "
                            "only once\n%s",Usage);
                    return(1);
                }
                j=(CARD16)(i+1);
                if(j<argc){
                    mbname=argv[j];
                }else{
                    fprintf(stderr,"### -f must be followed by the mb "
                            "file name\n%s",Usage);
                    return(1);
                }
                i=j;
                break;
            case 'F':
                force=TRUE;
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
    
    if(create){
        if(mbname==NIL){
            fmbin=stdout;
        }else{
            fmbin=fopen((char*)mbname,"w");
            if(fmbin==NIL){
                fprintf(stderr,"### Cannot create output file %s\n",mbname);
                return(2);
            }
        }
        if(!MacBinary_Create(fmbin,fname,type,creator,verbose)){
            return(1);
        }
    }else{
        if(mbname==NIL){
            fmbin=stdin;
        }else{
            fmbin=fopen((char*)mbname,"r");
            if(fmbin==NIL){
                fprintf(stderr,"### Cannot open input file %s\n",mbname);
                return(2);
            }
        }
        if(extract){
            if(!MacBinary_Extract(fmbin,fname,verbose)){
                return(1);
            }
        }else if(getinfo){
            MacBinary_FileInfoT     info;
                
            if(!force&&!MacBinary_GetInfo(fmbin,verbose,&info)){
                return(1);
            }
            PrintInfo(stdout,&info);
            return(0);
        }else{
            fprintf(stderr,"!!! Internal error: at least one of "
                    "-c -x -t is mandatory.\n");
            return(1);
        }
    }
    
    return(0);
}/*main*/


/***
    if(create){
    printf("create ");
    printf("fname=%s ",fname);
    if(type!=NIL){
    printf("type=%08lx ",*(CARD32*)type);
    }
    if(creator!=NIL){
    printf("creator=%08lx ",*(CARD32*)creator);
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
    if(mbname){
    printf("mbname=%s ",mbname);
    }
    printf("\n");
***/

/**** THE END ****/

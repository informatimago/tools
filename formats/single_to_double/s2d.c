/******************************************************************************
FILE:               s2d.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program converts files from Apple-Single to Apple-Double format.
    
        THIS IMPLEMENTATION RELIES ON A GLOBAL VARIABLE 
            (AppleSD_HeaderT header)
        AND THUS IS FULL OF SIDE-EFFECTS. NOTHING CAN BE EXTRACTED. 
        NO MODULARIZATION. YOU HAVE TO REWRITE IT FROM SCRATCH. 
AUTHORS
    <PJB> Pascal J. Bourguignon 
MODIFICATIONS
    1992/03/25 <PJB> Creation.
    1992/03/27 <PJB> Replaced the use of raw UNIX I/O routines by that of stdio
                        buffered routines.
LEGAL
    Copyright Pascal J. Bourguignon 1992 - 2011
    All rights reserved.
    This program may not be included in any commercial product without the 
    author written permission. It may be used freely for any non-commercial 
    purpose, provided that this header is always included.
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* #include <unistd.h> */
#include <sys/file.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <BcTypes.h>

#ifndef SEEK_SET
#define SEEK_SET 0      /* arguments to fseek function */
#define SEEK_CUR 1
#define SEEK_END 2
#endif




/*
  s2d src dst
*/

#define     ASCII_BS    (8)
#define     ASCII_LF    (10)
#define     ASCII_CR    (13)
#define     ASCII_DEL   (127)



typedef struct {
    INT32           magic;
    INT32           version;
    INT32           filler[4];
    INT16           entryCount;
}       AppleSD_HeaderT;

typedef struct {
    INT32           id;
    INT32           offset;
    INT32           length;
}       AppleSD_EntryT;

#define AppleSD_Single_Magic        (0x00051600)
#define AppleSD_Double_Magic        (0x00051607)
#define AppleSD_Version_2           (0x00020000)

AppleSD_HeaderT     header;

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

typedef char    OSType[4];
typedef struct {
    INT16   h,v;
}               Point;

struct FInfo {
    OSType fdType;                  /*the type of the file*/
    OSType fdCreator;               /*file's creator*/
    unsigned short fdFlags;         /*flags ex. hasbundle,invisible,locked, etc.*/
    Point fdLocation;               /*file's location in folder*/
    short fdFldr;                   /*folder containing file*/
};
typedef struct FInfo FInfo;

#if 0
static char     SegmentNameStrings[16][32]={
    "Segment #%d",
    "DataFork",
    "ResourceFork",
    "RealName",
    "Comment",
    "BWIcon",
    "ColorIcon",
    "Segment #7",
    "FileDateInfo",
    "FinderInfo",
    "MacintoshFileInfo",
    "ProDOSFileInfo",
    "MSDOSFileInfo",
    "AFPShortName",
    "AFPFileInfo",
    "AFPDirectoryId"
};

static void GetSegmentName(int id,char* name)
{
    if((id<=0)||(Segm_AFPDirectoryId<id)){
        sprintf(name,SegmentNameStrings[0],id);
    }else{
        strcpy(name,SegmentNameStrings[id]);
    }
}/*GetSegmentName*/
#endif

static BOOLEAN AppleSD_IsSingle(FILE* fd)
{
    
    INT32       size;
        
    if(0!=fseek(fd,0,SEEK_SET)){
        exit(20);
    }
    size=(INT32)fread(&header,1,sizeof(header),fd);
    if(size==sizeof(header)){
        return((header.magic==AppleSD_Single_Magic));
    }else{
        exit(21);
    }
}/*AppleSD_IsSingle*/


static BOOLEAN AppleSD_IsDouble(FILE* fd)
{
    AppleSD_HeaderT     hdr;
    INT32               size;
        
    if(0!=fseek(fd,0,SEEK_SET)){
        exit(22);
    }
    size=(INT32)fread(&hdr,1,sizeof(hdr),fd);
    if(size==sizeof(hdr)){
        return((hdr.magic==AppleSD_Double_Magic));
    }else{
        exit(23);
    }
}/*AppleSD_IsDouble*/


/*****
      {
      char    name[32];
      GetSegmentName(entry->id,name);
      fprintf(stderr,"entry[%d]: offset=%lx, length=%lx, id=%d (%s)\n",i,entry->offset,entry->length,entry->id,name);
      }
*****/

static BOOLEAN AppleSD_GetEntry(FILE* fd,INT32 id, AppleSD_EntryT* entry)
{
    INT32       i;
    INT32       size;
        
    if(0!=fseek(fd,sizeof(AppleSD_HeaderT),SEEK_SET)){
        exit(24);
    }
    i=0;
    while(i<header.entryCount){
        size=(INT32)fread(entry,1,sizeof(AppleSD_EntryT),fd);
        if(size==sizeof(AppleSD_EntryT)&&(entry->id==id)){
            return(TRUE);
        }
        i++;
    }
    return(FALSE);
}/*AppleSD_GetEntry*/


static BOOLEAN AppleSD_GetType(FILE* fd,char* type)
{
    AppleSD_EntryT      entry;
    FInfo               finfo;
    int                 size;
    
    if(AppleSD_GetEntry(fd,Segm_FinderInfo,&entry)){
        if(0!=fseek(fd,entry.offset,SEEK_SET)){
            /*fprintf(stderr,"Cannot fseek.\n");*/
            return(FALSE);
        }
        size=(INT32)fread(&finfo,1,sizeof(FInfo),fd);
        type[0]=finfo.fdType[0];
        type[1]=finfo.fdType[1];
        type[2]=finfo.fdType[2];
        type[3]=finfo.fdType[3];
        /*if(size!=sizeof(FInfo)){ fprintf(stderr,"Bad size for Segm_FinderInfo:%d.\n",size); }*/
        return(size==sizeof(FInfo));
    }else{
        /*fprintf(stderr,"AppleSD_GetEntry returned FALSE.\n");*/
        return(FALSE);
    }
}/*AppleSD_GetType*/



static void AppleSD_CopyData(FILE* filefd,FILE* datafd)
{
    AppleSD_EntryT  entry;
    char            buffer[4096];
    INT32           actual;
        
    if(AppleSD_GetEntry(filefd,1,&entry)){
        if(0!=fseek(filefd,entry.offset,SEEK_SET)){
            exit(28);
        }else{
            while((unsigned)(entry.length)>=sizeof(buffer)){
                actual=(INT32)fread(&buffer,1,sizeof(buffer),filefd);
                if(actual!=sizeof(buffer)){
                    exit(1);
                }
                actual=(INT32)fwrite(&buffer,1,sizeof(buffer),datafd);
                if(actual!=sizeof(buffer)){
                    exit(2);
                }
                entry.length-=(INT32)sizeof(buffer);
            }
            if(entry.length>0){
                actual=(INT32)fread(&buffer,1,(unsigned)(entry.length),filefd);
                if(actual!=entry.length){
                    exit(3);
                }
                actual=(INT32)fwrite(&buffer,1,(unsigned)(entry.length) ,datafd);
                if(actual!=entry.length){
                    exit(4);
                }
            }
        }
    }
}/*AppleSD_CopyData*/


static void AppleSD_SetDouble(FILE* fd)
{
    AppleSD_HeaderT     hdr;
    INT32               size;
        
    if(0!=fseek(fd,0,SEEK_SET)){
        exit(26);
    }
    size=(INT32)fread(&hdr,1,sizeof(hdr),fd);
    if(size!=sizeof(hdr)){
        exit(36);
    }
    hdr.magic=AppleSD_Double_Magic;
    if(0!=fseek(fd,0,SEEK_SET)){
        exit(29);
    }
    size=(INT32)fwrite(&hdr,1,sizeof(hdr),fd);
    if(size!=sizeof(hdr)){
        fprintf(stderr,"error on fwrite sizeof(hdr)=%d, size=%"FMT_INT32"\n",
                (int)(sizeof(hdr)),size);
        exit(37);
    }
}/*AppleSD_SetDouble*/



static BOOLEAN AppleSD_ClearEntry(FILE* filefd,INT32 id)
{
    INT32               i;
    INT32               size;
    AppleSD_EntryT      entry;
        
    if(0!=fseek(filefd,sizeof(AppleSD_HeaderT),SEEK_SET)){
        exit(25);
    }
    i=0;
    while(i<header.entryCount){
        size=(INT32)fread(&entry,1,sizeof(AppleSD_EntryT),filefd);
        if((size==sizeof(AppleSD_EntryT))&&(entry.id==id)){
            if(0!=fseek(filefd,-(signed)sizeof(AppleSD_EntryT),SEEK_CUR)){
                exit(27);
            }else{
                entry.id+=(INT32)0x80000000;
                size=(INT32)fwrite(&entry,1,sizeof(AppleSD_EntryT),filefd);
                if(size==sizeof(AppleSD_EntryT)){
                    return(TRUE);
                }else{
                    exit(35);
                }
            }
        }
        i++;
    }
    return(FALSE);
}/*AppleSD_ClearEntry*/


static void AppleSD_CopyOther(FILE* srcfd,FILE* dstfd)
{
    char            buffer[4096];
    INT32           actual;
    INT32           fsize;
    
    if(0!=fseek(srcfd,0,SEEK_END)){
        exit(6);
    }
    fsize=(INT32)ftell(srcfd);
    if(0!=fseek(srcfd,0,SEEK_SET)){
        exit(30);
    }else{
        while((unsigned)fsize>=sizeof(buffer)){
            actual=(INT32)fread(&buffer,1,sizeof(buffer),srcfd);
            if(actual!=sizeof(buffer)){
                exit(7);
            }
            actual=(INT32)fwrite(&buffer,1,sizeof(buffer),dstfd);
            if(actual!=sizeof(buffer)){
                exit(8);
            }
            fsize-=(INT32)sizeof(buffer);
        }
        if(fsize>0){
            actual=(INT32)fread(&buffer,1,(unsigned)fsize,srcfd);
            if(actual!=fsize){
                exit(9);
            }
            actual=(INT32)fwrite(&buffer,1,(unsigned)fsize,dstfd);
            if(actual!=fsize){
                exit(10);
            }
        }
    }
    AppleSD_SetDouble(dstfd);
    if(AppleSD_ClearEntry(dstfd,1)){
        return;
    }else{
        exit(11);
    }
}/*AppleSD_CopyOther*/


static void AppleSD_Single_to_Double(FILE* filefd,FILE* datafd,FILE* rsrcfd)
{
    AppleSD_CopyData(filefd,datafd);
    AppleSD_CopyOther(filefd,rsrcfd);
}/*AppleSD_Single_to_Double*/






static void usage(char* upname)
    
{
    printf("WARNING: Not all these options are implemented! "\
           "Read the source, Luke!\n");
    printf("# %s usage:\n",upname);
    printf("#   setfile -t TYPE -c CREA unix_data_file ...\n");
    printf("#   dcat < apple_single_file > data_fork\n");
    printf("#   rcat < apple_single_file > resource_fork\n");
    printf("#   dcat apple_single_file ... > data_forks\n");
    printf("#   rcat apple_single_file ... > resource_forks\n");
    printf("#   rcat = s2d -r\n");
    printf("#   dcat = s2d -d\n");
    printf("#   s2d [ -i signature_dictionary ] file ... # replace file by (file.ext, file.ext)\n");
}/*usage*/


#if 0
static void printerr(int err)
{
    fprintf(stderr,"Error: %d\n",err);
}/*printerr*/


static void MakeTempName(char*src,char*temp)
{
    strcpy(temp,src);
    strcat(temp,".lc[ ]");
}/*MakeTempName*/
#endif

static void basename(char* src,char* dst)
{
    char*       lastslash;
    char*       current;
    
    current=src;
    lastslash=src;
    while((*current)!=0){
        if((*current)=='/'){
            current++;
            lastslash=current;
        }else{
            current++;
        }
    }
    while((*lastslash)!=0){
        (*dst)=(*lastslash);
        lastslash++;
        dst++;
    }
    (*dst)=(*lastslash);
}/*basename*/


static INT32 makeresourcefilename(char* data,char* rsrc)
{
    char*       lastslash;
    char*       start;
    
    start=rsrc;
    lastslash=rsrc;
    while((*data)!=0){
        (*rsrc)=(*data);
        rsrc++;
        if((*data)=='/'){
            lastslash=rsrc;
        }
        data++;
    }
    data-=(rsrc-lastslash);
    rsrc=lastslash;
    (*rsrc)='%';
    rsrc++;
    while((*data)!=0){
        (*rsrc)=(*data);
        rsrc++;
        data++;
    }
    (*rsrc)=(*data);
    return((INT32)(rsrc-start));
}/*makeresourcefilename*/


static char PrintingChar(int/*char*/ c)
{
    if((' '<=c)&&(c<=0x7e)){
        return((char)c);
    }else{
        return('?');
    }
}/*PrintingChar*/


int main(int argc,char** argv)
{
    int         err;
        
    struct stat filestatus;
        
    FILE*   filefd;
    FILE*   datafd;
    FILE*   rsrcfd;
        
    char        rdestination[1024];
    char        type[8];
    int         i;
        
    basename(argv[0],rdestination);
    if((strcmp(rdestination,"is")==0)&&(argc==2)){
        filefd=fopen(argv[1],"r");
        if(filefd==NIL){
            fprintf(stderr,"I cannot open %s.\n",argv[1]);
            exit(2);
        }
        if(AppleSD_IsSingle(filefd)){
            fclose(filefd);
            exit(0);
        }
        fclose(filefd);
        exit(31);
    }else if(strcmp(rdestination,"type")==0){
        i=1;
        while(i<argc){
            filefd=fopen(argv[i],"r");
            if(filefd==NIL){
                fprintf(stderr,"I cannot open %s.\n",argv[i]);
            }else{
                if(AppleSD_IsSingle(filefd)||AppleSD_IsDouble(filefd)){
                    if(AppleSD_GetType(filefd,type)){
                        printf("%c%c%c%c %s\n",PrintingChar(type[0]),PrintingChar(type[1]),PrintingChar(type[2]),PrintingChar(type[3]),argv[i]);
                    }else{
                        fprintf(stderr,"I cannot get type of %s.\n",argv[i]);
                    }
                }else{
                    printf("%s is not Apple-single nor Apple-double.\n",argv[i]);
                }
                fclose(filefd);
            }
            i++;
        }
        exit(0);
    }else if(strcmp(rdestination,"setfile")==0){
        struct {
            AppleSD_HeaderT     header; 
            AppleSD_EntryT      entries[16]; /* only one used.*/
            FInfo               finfo;
        }                   ffork;
        char                fname[2048];

        ffork.header.magic=AppleSD_Double_Magic;
        ffork.header.version=AppleSD_Version_2;
        ffork.header.filler[0]=0;
        ffork.header.filler[1]=0;
        ffork.header.filler[2]=0;
        ffork.header.filler[3]=0;
        ffork.header.entryCount=1;
        ffork.entries[0].id=Segm_FinderInfo;
        ffork.entries[0].offset=(INT32)((INTPTR)(&(ffork.finfo))-(INTPTR)(&(ffork)));
        ffork.entries[0].length=sizeof(ffork.finfo);
        i=1;
        while(i<16){
            ffork.entries[i].id=0;
            ffork.entries[i].offset=0;
            ffork.entries[i].length=0;
            i++;
        }
        strncpy(ffork.finfo.fdType,"TEXT",4);
        strncpy(ffork.finfo.fdCreator,"UNIX",4);
        ffork.finfo.fdFlags=0;
        ffork.finfo.fdLocation.h=0;
        ffork.finfo.fdLocation.v=0;
        ffork.finfo.fdFldr=0;
        i=1;
        while(i<argc){
            if(strcmp(argv[i],"-t")==0){
                i++;
                if(i<argc){
                    ffork.finfo.fdType[0]=argv[i][0];
                    ffork.finfo.fdType[1]=argv[i][1];
                    ffork.finfo.fdType[2]=argv[i][2];
                    ffork.finfo.fdType[3]=argv[i][3];
                }else{
                    usage(argv[0]);
                    exit(1);
                }
            }else if(strcmp(argv[i],"-c")==0){
                i++;
                if(i<argc){
                    ffork.finfo.fdCreator[0]=argv[i][0];
                    ffork.finfo.fdCreator[1]=argv[i][1];
                    ffork.finfo.fdCreator[2]=argv[i][2];
                    ffork.finfo.fdCreator[3]=argv[i][3];
                }else{
                    usage(argv[0]);
                    exit(1);
                }
            }else{
                INT32   actual;

                makeresourcefilename(argv[i],fname);
                rsrcfd=fopen(fname,"w+");
                if(rsrcfd==NIL){
                    fprintf(stderr,"I cannot create '%s'.\n",fname);
                    exit(16);
                }
                actual=(INT32)fwrite(&ffork,1,sizeof(ffork),rsrcfd);
                if(actual!=sizeof(ffork)){
                    exit(2);
                }
                fclose(rsrcfd);
            }
            i++;
        }
        exit(0);
    }
    
    if(argc!=3){
        usage(argv[0]);
        exit(12);
    }

    makeresourcefilename(argv[2],rdestination);
        
    filefd=fopen(argv[1],"r");
    if(filefd==NIL){
        fprintf(stderr,"I cannot open '%s'.\n",argv[1]);
        exit(13);
    }
    
    if(!AppleSD_IsSingle(filefd)){
        fclose(filefd);
        exit(0);
    }

    datafd=fopen(argv[2],"r");
    if(datafd!=NIL){
        printf("file '%s' already exists.",argv[2]);
        fclose(filefd);
        fclose(datafd);
        exit(14);
    }
    datafd=fopen(argv[2],"w+");
    if(datafd==NIL){
        fprintf(stderr,"I cannot create '%s'.\n",argv[2]);
        fclose(filefd);
        exit(15);
    }
    
    rsrcfd=fopen(rdestination,"w+");
    if(rsrcfd==NIL){
        fprintf(stderr,"I cannot create '%s'.\n", rdestination);
        fclose(datafd);
        fclose(filefd);
        exit(16);
    }

    AppleSD_Single_to_Double(filefd,datafd,rsrcfd);
    
    fclose(datafd);
    fclose(rsrcfd);
    fclose(filefd);
    
    err=stat(argv[1],&filestatus);
    if(err==0){
        int r;
        r=chown(argv[2],filestatus.st_uid,filestatus.st_gid);
        chmod(argv[2],(mode_t)(filestatus.st_mode));
        r=chown(rdestination,filestatus.st_uid,filestatus.st_gid);
        chmod(rdestination,(mode_t)(filestatus.st_mode));
        (void)r;
    }

    exit(0);
}/*main*/


    
/*
  setfile -t TYPE -c CREA unix_data_file ...
  create a file named '%unix_data_file' with the signature and type.
  dcat < apple_single_file > data_fork
  rcat < apple_single_file > resource_fork
  dcat apple_single_file ... > data_forks
  rcat apple_single_file ... > resource_forks
  rcat = s2d -r
  dcat = s2d -d
  s2d [ -i signature_dictionary ] file ... 
  # replace file by (file.ext,%file.ext)
  signature_dictionary file format    
  sig_file    ::= line
  sig_file    ::= line sig_file
  line        ::= signature <.> extension <RC>
  signature   ::= <'> <caractere> <caractere> <caractere> <caractere> <'>
  signature   ::= hexa hexa hexa hexa hexa hexa hexa hexa 
  hexa        ::= <0|1>|<2>|<3>|<4>|<5>|<6>|<7>|<8>|<9>|<A>|<B>|<C>|<D>|<E>|<F>
  extension   ::= <caractere>
  extension   ::= <caractere> | extension


  printf("# %s usage:\n",pname);
  printf("#   setfile -t TYPE -c CREA unix_data_file ...\n");
  printf("#   dcat < apple_single_file > data_fork\n");
  printf("#   rcat < apple_single_file > resource_fork\n");
  printf("#   dcat apple_single_file ... > data_forks\n");
  printf("#   rcat apple_single_file ... > resource_forks\n");
  printf("#   rcat = s2d -r\n");
  printf("#   dcat = s2d -d\n");
  printf("#   s2d [ -i signature_dictionary ] file ... # replace file by (file.ext, %file.ext)\n");

*/



/**** THE END ****/

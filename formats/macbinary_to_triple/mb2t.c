/******************************************************************************
FILE:               mb2t.c
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
    1992/03/25 <PJB> Creation (s2d).
    1992/03/27 <PJB> Replaced the use of raw UNIX I/O routines by that of stdio
                        buffered routines.
    1992/04/18 <PJB> Addapted from s2d.
LEGAL
    Copyright Pascal J. Bourguignon 1992 - 1992
    All rights reserved.
    This program may not be included in any commercial product without the 
    author written permission. It may be used freely for any non-commercial 
    purpose, provided that this header is always included.
******************************************************************************/
#include    <sys/file.h>
#include    <sys/errno.h>
#include    <sys/types.h>
#include    <sys/stat.h>

#include    <stdio.h>

#ifndef SEEK_SET
#define SEEK_SET 0      /* arguments to fseek function */
#define SEEK_CUR 1
#define SEEK_END 2
#endif

#include <ASCII.h>



typedef struct {
    int32           magic;
    int32           version;
    int32           filler[4];
    int16           entryCount;
}       AppleSD_HeaderT;

typedef struct {
    int32           id;
    int32           offset;
    int32           length;
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
    int16   h,v;
}               Point;

struct FInfo {
    OSType fdType;                  /*the type of the file*/
    OSType fdCreator;               /*file's creator*/
    unsigned short fdFlags;         /*flags ex. hasbundle,invisible,locked, etc.*/
    Point fdLocation;               /*file's location in folder*/
    short fdFldr;                   /*folder containing file*/
};
typedef struct FInfo FInfo;


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

void GetSegmentName(int id,char* name)
{
    if((id<=0)||(Segm_AFPDirectoryId<id)){
        sprintf(name,SegmentNameStrings[0],id);
    }else{
        strcpy(name,SegmentNameStrings[id]);
    }
}//GetSegmentName

boolean AppleSD_IsSingle(FILE* fd)
{
    
        int32       size;
        
    if(0!=fseek(fd,0,SEEK_SET)){
        exit(20);
    }
    size=fread(&header,1,sizeof(header),fd);
    if(size==sizeof(header)){
        return((header.magic==AppleSD_Single_Magic));
    }else{
        exit(21);
    }
}//AppleSD_IsSingle

boolean AppleSD_IsDouble(FILE* fd)
{
        AppleSD_HeaderT     header;
        int32               size;
        
    if(0!=fseek(fd,0,SEEK_SET)){
        exit(22);
    }
    size=fread(&header,1,sizeof(header),fd);
    if(size==sizeof(header)){
        return((header.magic==AppleSD_Double_Magic));
    }else{
        exit(23);
    }
}//AppleSD_IsDouble


/*****
{
        char    name[32];
    GetSegmentName(entry->id,name);
    fprintf(stderr,"entry[%d]: offset=%lx, length=%lx, id=%d (%s)\n",i,entry->offset,entry->length,entry->id,name);
}
*****/
boolean AppleSD_GetEntry(FILE* fd,int32 id, AppleSD_EntryT* entry)
{
        int32       i;
        int32       size;
        
    if(0!=fseek(fd,sizeof(AppleSD_HeaderT),SEEK_SET)){
        exit(24);
    }
    i=0;
    while(i<header.entryCount){
        size=fread(entry,1,sizeof(AppleSD_EntryT),fd);
        if(size==sizeof(AppleSD_EntryT)&&(entry->id==id)){
            return(true);
        }
        i++;
    }
    return(false);
}//AppleSD_GetEntry

boolean AppleSD_GetType(FILE* fd,char* type)
{
        AppleSD_EntryT      entry;
        FInfo               finfo;
        int                 size;
    
    if(AppleSD_GetEntry(fd,Segm_FinderInfo,&entry)){
        if(0!=fseek(fd,entry.offset,SEEK_SET)){
//fprintf(stderr,"Cannot fseek.\n");
            return(false);
        }
        size=fread(&finfo,1,sizeof(FInfo),fd);
        type[0]=finfo.fdType[0];
        type[1]=finfo.fdType[1];
        type[2]=finfo.fdType[2];
        type[3]=finfo.fdType[3];
//if(size!=sizeof(FInfo)){ fprintf(stderr,"Bad size for Segm_FinderInfo:%d.\n",size); }
        return(size==sizeof(FInfo));
    }else{
//fprintf(stderr,"AppleSD_GetEntry returned false.\n");
        return(false);
    }
}//AppleSD_GetType


void AppleSD_CopyData(FILE* filefd,FILE* datafd)
{
        AppleSD_EntryT  entry;
        char            buffer[4096];
        int32           actual;
        
    if(AppleSD_GetEntry(filefd,1,&entry)){
        if(0!=fseek(filefd,entry.offset,SEEK_SET)){
            exit(28);
        }else{
            while(entry.length>=sizeof(buffer)){
                actual=fread(&buffer,1,sizeof(buffer),filefd);
                if(actual!=sizeof(buffer)){
                    exit(1);
                }
                actual=fwrite(&buffer,1,sizeof(buffer),datafd);
                if(actual!=sizeof(buffer)){
                    exit(2);
                }
                entry.length-=sizeof(buffer);
            }
            if(entry.length>0){
                actual=fread(&buffer,1,entry.length,filefd);
                if(actual!=entry.length){
                    exit(3);
                }
                actual=fwrite(&buffer,1,entry.length ,datafd);
                if(actual!=entry.length){
                    exit(4);
                }
            }
        }
    }
}//AppleSD_CopyData

void AppleSD_SetDouble(FILE* fd)
{
        AppleSD_HeaderT     header;
        int32               size;
        
    if(0!=fseek(fd,0,SEEK_SET)){
        exit(26);
    }
    size=fread(&header,1,sizeof(header),fd);
    if(size!=sizeof(header)){
        exit(36);
    }
    header.magic=AppleSD_Double_Magic;
    if(0!=fseek(fd,0,SEEK_SET)){
        exit(29);
    }
    size=fwrite(&header,1,sizeof(header),fd);
    if(size!=sizeof(header)){
        fprintf(stderr,"error on fwrite sizeof(header)=%d, size=%d\n",sizeof(header),size);
        exit(37);
    }
}//AppleSD_SetDouble


boolean AppleSD_ClearEntry(FILE* filefd,int32 id)
{
        int32               i;
        int32               size;
        AppleSD_EntryT      entry;
        
    if(0!=fseek(filefd,sizeof(AppleSD_HeaderT),SEEK_SET)){
        exit(25);
    }
    i=0;
    while(i<header.entryCount){
        size=fread(&entry,1,sizeof(AppleSD_EntryT),filefd);
        if((size==sizeof(AppleSD_EntryT))&&(entry.id==id)){
            if(0!=fseek(filefd,-sizeof(AppleSD_EntryT),SEEK_CUR)){
                exit(27);
            }else{
                entry.id+=0x80000000;
                size=fwrite(&entry,1,sizeof(AppleSD_EntryT),filefd);
                if(size==sizeof(AppleSD_EntryT)){
                    return(true);
                }else{
                    exit(35);
                }
            }
        }
        i++;
    }
    return(false);
}//AppleSD_ClearEntry

void AppleSD_CopyOther(FILE* srcfd,FILE* dstfd)
{
        AppleSD_EntryT  entry;
        char            buffer[4096];
        int32           actual;
        int32           fsize;
    
    if(0!=fseek(srcfd,0,SEEK_END)){
        exit(6);
    }
    fsize=ftell(srcfd);
    if(0!=fseek(srcfd,0,SEEK_SET)){
        exit(30);
    }else{
        while(fsize>=sizeof(buffer)){
            actual=fread(&buffer,1,sizeof(buffer),srcfd);
            if(actual!=sizeof(buffer)){
                exit(7);
            }
            actual=fwrite(&buffer,1,sizeof(buffer),dstfd);
            if(actual!=sizeof(buffer)){
                exit(8);
            }
            fsize-=sizeof(buffer);
        }
        if(fsize>0){
            actual=fread(&buffer,1,fsize,srcfd);
            if(actual!=fsize){
                exit(9);
            }
            actual=fwrite(&buffer,1,fsize,dstfd);
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
}//AppleSD_CopyOther

void AppleSD_Single_to_Double(FILE* filefd,FILE* datafd,FILE* rsrcfd)
{
        char        buffer[4096];
    
    AppleSD_CopyData(filefd,datafd);
    AppleSD_CopyOther(filefd,rsrcfd);
}//AppleSD_Single_to_Double





void usage(pname)
    char*       pname;
{
    if(pname[0]=='s'){
        printf("# usage:    %s source destination\n#    will create %%destination and destination\n",pname);
    }else{
        printf("# usage:    %s file\n#  will create %%destination and destination\n",pname);
    }
}/*usage*/


void printerr(err)
    int         err;
{
    fprintf(stderr,"Error: %d\n",err);
}/*printerr*/

void MakeTempName(src,temp)
    char*       src;
    char*       temp;
{
    strcpy(temp,src);
    strcat(temp,".lc[ ]");
}/*MakeTempName*/

void basename(char* src,char* dst)
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
}//basename

int32 makeresourcefilename(char* data,char* rsrc)
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
    return(rsrc-start);
}/*makeresourcefilename*/

char PrintingChar(char c)
{
    if((' '<=c)&&(c<=0x7e)){
        return(c);
    }else{
        return('?');
    }
}//PrintingChar

int main(argc,argv)
    int         argc;
    char*       argv[];
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
        if(filefd==nil){
            fprintf(stderr,"I cannot open %s.\n",argv[1]);
            exit(2);
        }
        if(AppleSD_IsSingle(filefd)){
            close(filefd);
            exit(0);
        }
        close(filefd);
        exit(31);
    }else if(strcmp(rdestination,"type")==0){
        i=1;
        while(i<argc){
            filefd=fopen(argv[i],"r");
            if(filefd==nil){
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
                close(filefd);
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
    if(filefd==nil){
        fprintf(stderr,"I cannot open '%s'.\n",argv[1]);
        exit(13);
    }
    
    if(!AppleSD_IsSingle(filefd)){
        close(filefd);
        exit(0);
    }

    datafd=fopen(argv[2],"r");
    if(datafd!=nil){
        printf("file '%s' already exists.",argv[2]);
        fclose(filefd);
        fclose(datafd);
        exit(14);
    }
    datafd=fopen(argv[2],"w+");
    if(datafd==nil){
        fprintf(stderr,"I cannot create '%s'.\n",argv[2]);
        close(filefd);
        exit(15);
    }
    
    rsrcfd=fopen(rdestination,"w+");
    if(rsrcfd==nil){
        fprintf(stderr,"I cannot create '%s'.\n", rdestination);
        close(datafd);
        close(filefd);
        exit(16);
    }

    AppleSD_Single_to_Double(filefd,datafd,rsrcfd);
    
    fclose(datafd);
    fclose(rsrcfd);
    fclose(filefd);
    
    err=stat(argv[1],&filestatus);
    if(err==0){
        chown(argv[2],filestatus.st_uid,filestatus.st_gid);
        chmod(argv[2],filestatus.st_mode);
        chown(rdestination,filestatus.st_uid,filestatus.st_gid);
        chmod(rdestination,filestatus.st_mode);
    }

    exit(0);
}/*main*/


    
/*

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
    printf("#   dcat < apple_single_file > data_fork\n");
    printf("#   rcat < apple_single_file > resource_fork\n");
    printf("#   dcat apple_single_file ... > data_forks\n");
    printf("#   rcat apple_single_file ... > resource_forks\n");
    printf("#   rcat = s2d -r\n");
    printf("#   dcat = s2d -d\n");
    printf("#   s2d [ -i signature_dictionary ] file ... # replace file by (file.ext, %file.ext)\n");

*/



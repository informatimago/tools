/******************************************************************************
FILE:               lc.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program converts LF and CR, and eats BS and previous characters.
    BS does not go back up the lines.
    Each end of line is converted to the selected end of line sequence.
    The end of line is the first sequence in the list (in decreasing priority):
        cr,lf
        lf,cr
        cr
        lf
    bonBBBBCC<<BBCCCDD<<CC<<<<<BBB<<<<<<<<<joBB<<ur!
USAGE
    lc -[b][d][m|u|p|c] [-(c|a)SD] file... | < file > file
        -sSIZE          truncate lines at SIZE characters. 
        -z              truncate at first ^Z (ASCII_SUB) // NOT IMPLEMENTED.
        -b              remove BS
        -d              remove DEL
        -m              mac     cr
        -u              unix    lf
        -p              pc      cr,lf
        -c              cp      lf,cr
        -aSD            do an approximative conversion from S char 
                            set to D char set.
        -cSD            do an approximative conversion from S char
                            set to D char set.
        
                char_set is one of:
                    n               8-bit Next character set.
                    m               8-bit Macintosh character set.
(NOT IMPLEMENTED)   i               8-bit IBM-PC character set.
(NOT IMPLEMENTED)   e               8-bit EBCDIC.
(NOT IMPLEMENTED)   a               7-bit ASCII.
(NOT IMPLEMENTED)   u               16-bit UNICODE (only 7-bits ASCII is 
                                                    recognized for now).
        If no character set is specified or same character set is specified
        for both, then no conversion is done.
        
        Character conversion is done after other processing, but specifying 
        UNICODE as source character set imply 16-bit input prior other 
        processing.
AUTHORS
    <PJB> Pascal J. Bourguignon 
MODIFICATIONS
    1990/12/08 <PJB> Creation.
    1991/01/22 <PJB> Changed the scheme for updating the file.
                     (used to link/unlink, but that does not work between 
                     two file systems).
    1991/02/08 <PJB> Added the deletion of the temp file.
            Added -d option to remove DEL and corresponding previous character.
    1991/06/28 <PJB> Facultative option [u|m|p|c].
    1993/05/11 <PJB> Added option to truncate MS-DOS file at ^Z.
    1993/09/26 <PJB> Updated.
    Revision 1.2  2004/06/24 18:19:37  pjbpjb
    Updated. Added a concatenate function to paste back split strings for
    ISO-C limitations.

    Revision 1.1  2003/12/04 03:46:03  pjbpjb
    Cleaned-up.
    Fix.

    Revision 1.2  2003/11/22 22:30:53  pascal
    Added options to ascii.
    Corrected some compilation problems.
    
    Revision 1.1  2001/04/30 01:58:08  pascal
    Initial entry into CVS.
    
    Revision 1.1  2001/04/27 06:45:10  pascal
    Added to CVS.
    
    Revision 1.7  95/11/23  02:17:52  pascal
    Skip nodes other than regular files.
    Made the temporary names less special.
    
    Revision 1.6  95/11/23  01:41:55  pascal
    rcs finds some unterminated string in this text!
    
    Revision 1.5  95/09/08  19:59:16  pascal
    modification en cours, option -ssize.
    
    Revision 1.3  1994/07/13  12:11:43  pbo
    Transformed C++ comments into C comments.

    Revision 1.2  1994/02/15  05:21:21  pascal
    Added RCS keywords.

LEGAL
    Copyright Pascal J. Bourguignon 1990 - 2011
    All rights reserved.
    This program or any part of it may not be included in any commercial 
    product without the author written permission. It may be used freely for 
    any non-commercial purpose, provided that this header is always included.
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(linux)||defined(AIX)
#include <unistd.h>
#else
#include <libc.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include <BcTypes.h>


/*
 */

#define     ASCII_BS    (8)
#define     ASCII_LF    (10)
#define     ASCII_CR    (13)
#define     ASCII_DEL   (127)

#define     toMac       (0)
#define     toUnix      (1)
#define     toPC        (2)
#define     toCP        (3)
#define     toNone      (4)

#define csISOCODE       (0)
#define csASCII         (1)
#define csEBCDIC        (2)
#define csNext          (3)
#define csMacintosh     (4)
#define csIbmpc         (5)



typedef char        SmallStringT[4];
    
SmallStringT    linefeed[5/* toMac..toNone */]={{ASCII_CR,0},{ASCII_LF,0},{ASCII_CR,ASCII_LF,0},{ASCII_LF,ASCII_CR,0},{0}};
BOOLEAN         removebs;
BOOLEAN         removedel;
INT32           converto;


#if 0    
static INT32 skip(char* line,INT32 current)
{
    INT32           count;
    
    count=0;
    while((current>=0)&&((removebs&&(line[current]==ASCII_BS))
                         ||(removedel&&(line[current]==ASCII_DEL)))){
        current--;
        count++;
    }
    while((current>=0)&&(count>0)){
        /* the first time, line[current]!=ASCII_BS */
        if((removebs&&(line[current]==ASCII_BS))
           ||(removedel&&(line[current]==ASCII_DEL))){
            current=skip(line,current);
        }else{
            current--;
            count--;
        }
    }
    return(current);
}/*skip*/
#endif
    
#if 0    
static INT32 old_copytoend(char* line,INT32 length)
/*
  POST:       for each i in copytoend(@@line,length,count) 
  to length-1, line[i]#ASCII_BS
*/
{
    INT32           current;
    INT32           destination;
        
    destination=length;
    current=length-1;
    while(current>=0){
        if((removebs&&(line[current]==ASCII_BS))
           ||(removedel&&(line[current]==ASCII_DEL))){
            current=skip(line,current);
        }else{
            destination--;
            line[destination]=line[current/* current-1 ???? */];
            current--;
        }
    }
    return(destination);
}/*copytoend*/
#endif    

static INT32 copytobegin(char* line,INT32 length)
/*
  POST:   for each i in 0 to copytobegin(@@line,length,count)-1, 
  line[i]#ASCII_BS
*/
{
    INT32           current;
    INT32           destination;
    
    current=0;
    destination=0;
    while(current<length){
        if((removebs&&(line[current]==ASCII_BS))
           ||(removedel&&(line[current]==ASCII_DEL))){
            if(destination>0){
                destination--;
            }
        }else{
            line[destination]=line[current];
            destination++;
        }
        current++;
    }   
    return(destination);
}/*copytobegin*/
    

#define     BufSize     (128*1024)
        
static char            inbuf[BufSize];
static char            outbuf[BufSize];
static CARD32          inmark;
static CARD32          inlen;
static CARD32          outlen;
    

static void initbufs(void)
{
    inmark=0;
    inlen=0;
    outlen=0;
}/*initbufs*/

    
static INT32 lc_getline(FILE* in,char** line,INT32* len)
/*
  PRE:        NOT feof(in) OR (inmark<inlen)
  POST:       *line points to the first character of the next line.
  *len is the number of character in this next line.
  */
{
    INT32           start;
    INT32           err;
            
    start=(INT32)inmark;
    *line=inbuf+inmark;
    while((inmark<inlen)&&(inbuf[inmark]!=ASCII_CR)
          &&(inbuf[inmark]!=ASCII_LF)){
        inmark++;
    }
    if(inmark>=inlen){
        inlen-=(CARD32)start;
        /* inlen = number of byte remaining in the buffer ; inlen << BufSize */
        if(inlen>0){
            strncpy(inbuf,inbuf+start,inlen);
        }
        clearerr(in);
        inlen+=(CARD32)fread(inbuf+inlen,1,BufSize-inlen,in);
        err=ferror(in);
        if(err!=0){
            fprintf(stderr,"error in lc_getline\n");
            return(err);
        }
        inmark=0;
        start=(INT32)inmark;
        *line=inbuf;
        while((inmark<inlen)&&(inbuf[inmark]!=ASCII_CR)
              &&(inbuf[inmark]!=ASCII_LF)){
            inmark++;
        }
    }
    *len=(INT32)inmark-start;
    linefeed[toNone][0]=inbuf[inmark];
    linefeed[toNone][1]=inbuf[inmark+1];
    if(inmark+1<inlen){
        if(inbuf[inmark]==ASCII_CR){
            if(inbuf[inmark+1]==ASCII_LF){
                inmark+=2;
                linefeed[toNone][2]=0;
            }else{
                inmark+=1;
                linefeed[toNone][1]=0;
            }
        }else if(inbuf[inmark]==ASCII_LF){
            if(inbuf[inmark+1]==ASCII_CR){
                inmark+=2;
                linefeed[toNone][2]=0;
            }else{
                inmark+=1;
                linefeed[toNone][1]=0;
            }
        }else{
            /* should not occur ?!? */
            linefeed[toNone][2]=0;
        }
    }else if(inmark<inlen){
        /* inbuf[inmark] is ASCII_CR or ASCII_LF */
        linefeed[toNone][1]=0;
        inmark++;
    }
    return(0);
}/*lc_getline*/

    
static INT32 eof(FILE* fd)
{
    return((feof(fd))&&(inmark>=inlen));
}/*eof*/

    
static INT32 flush(FILE* out)
{
    clearerr(out);
    if(outlen>0){
        size_t r=fwrite(outbuf,1,outlen,out);
        (void)r;
    }
    outlen=0;
    return(ferror(out));
}/*flush*/
    

static INT32 putstr(FILE* out,char* str,CARD32 len)
{
    char*       dst;
    INT32       err;
            
    if(len>0){
        if(outlen+len>BufSize){
            err=flush(out);
            if(err!=0){
                fprintf(stderr,"error in putstr\n");
                return(err);
            }
        }
        dst=outbuf+outlen;
        strncpy(dst,str,len);
        outlen+=len;
    }
    return(0);
}/*putstr*/
    
    
    
    
static INT32 convert(FILE* in,FILE* out)
{
    INT32           length;
    INT32           err;
    char*           line;
    char*           lf;
    CARD32          lfsize;
            
    lf=linefeed[converto];
    lfsize=(CARD32)strlen(lf);
    initbufs();
    while(!eof(in)){
        err=lc_getline(in,&line,&length);
        if(err!=0){
            printf("error in convert (lc_getline)\n");
            return(err);
        }
        if(removebs||removedel){
            length=copytobegin(line,length);
        }
        err=putstr(out,line,(unsigned)length);
        if(err!=0){
            printf("error in convert (putstr)\n");
            return(err);
        }
        err=putstr(out,lf,lfsize);
        if(err!=0){
            return(err);
        }
    }
    flush(out);
    return(0);
}/*convert*/
    
        
static void usage(char* pname)
{   
    printf("# %s usage:\n   %s -[b](m|u|p|c) file... | < file > file\n",pname,pname);
}/*usage*/
    
    
static void printerr(INT32 err)
{
    fprintf(stderr,"Error: %"FMT_INT32" G%sH\n",err,strerror(err));
}/*printerr*/
    
    
static void MakeTempName(char* src,char* temp)
/*
  PRE:        The number of file in the file system < MAXINT32
*/
{
    INT32           n;
    struct stat     status;
    
    n=-1;
    do{
        n++;
        sprintf(temp,"%s.%"FMT_INT32,src,n);
    }while(lstat(temp,&status)==0);
}/*MakeTempName*/

    
static const char* FileType(unsigned int mode)
{
    switch(mode&S_IFMT){
    case S_IFDIR:   return("directory");
    case S_IFCHR:   return("character special device");
    case S_IFBLK:   return("block special device");
    case S_IFREG:   return("regular file");
    case S_IFLNK:   return("symbolic link");
    case S_IFSOCK:  return("socket");
    default:        return("something of unknown type");
    }
}/*FileType*/


int main(int argc,char* argv[])
{
    FILE*           fin;
    FILE*           fout;
    INT32           i;
    INT32           j;
    INT32           maxlinesize=MAX_INT32;
    INT32           err;
    char*           opt;
    struct stat     filestatus;
    char            OutputName[BUFSIZ];
    char            TempLink[BUFSIZ];
    BOOLEAN         nooption;
    BOOLEAN         filestatus_is_valid;
        
    nooption=TRUE;
    removedel=FALSE;
    removebs=FALSE;
    converto=toNone;
    
    i=1;
    if((i<argc)&&(argv[i][0]=='-')){
        j=1;
        opt=argv[i];
        while(opt[j]!=0){
            switch(opt[j]){
            case 'b':
                removebs=TRUE;
                nooption=FALSE;
                break;
            case 'd':
                removedel=TRUE;
                nooption=FALSE;
                break;
            case 'm':
                converto=toMac;
                nooption=FALSE;
                break;
            case 'u':
                converto=toUnix;
                nooption=FALSE;
                break;
            case 'p':
                converto=toPC;
                nooption=FALSE;
                break;
            case 'c':
                converto=toCP;
                nooption=FALSE;
                break;
            case 's':
                {
                    INT32 s=atoi(opt+j+1);
                    if(s>0){
                        maxlinesize=s;
                        break;
                    }
                }
                /* fall thru: */
            default:
                fprintf(stderr,"### Invalid option: %c\n",opt[j]);
                usage(argv[0]);
                return(1);
            }
            j++;
        }
        i++;
    }
    if(!nooption){
        if(i<argc){
            while(i<argc){
                err=lstat(argv[i],&filestatus);
                if(err==0){
                    if((filestatus.st_mode&S_IFMT)==S_IFREG){
                        filestatus_is_valid=TRUE;
                    }else{
                        fprintf(stderr,"%s works only on regular files; '%s' is a %s and will be skipped.\n",argv[0],argv[i],FileType(filestatus.st_mode));
                        i++;
                        continue;
                    }
                }else{
                    filestatus_is_valid=FALSE;
                }
                fin=fopen(argv[i],"r");
                if(fin==NIL){
                    fprintf(stderr,"### I cannot open the input file \"%s\"; skipping it.\n",argv[i]);
                }else{
                    MakeTempName(argv[i],OutputName);
                    fout=fopen(OutputName,"w");
                    if(fout==NIL){
                        fprintf(stderr,"### I cannot open the output file \"%s\"; skipping \"%s\".\n",OutputName,argv[i]);
                        fclose(fin);
                    }else{
                        err=convert(fin,fout);
                        fclose(fin);
                        fclose(fout);
                        if(err==0){
                            MakeTempName(argv[i],TempLink);
                            err=rename(argv[i],TempLink);
                            if(err==0){
                                err=link(OutputName,argv[i]);
                                if(err==0){
                                    if(filestatus_is_valid){
                                        int r=chown(argv[i],filestatus.st_uid,
                                                    filestatus.st_gid);
                                        (void)r;
                                        chmod(argv[i],filestatus.st_mode);
                                    }
                                    unlink(TempLink);
                                }else{
                                    perror("Error while linking: ");
                                    fprintf(stderr,"### I cannot link the output file \"%s\" to the input file \"%s\"; skipping it.\n",OutputName,argv[i]);
                                    rename(TempLink,argv[i]);
                                }
                            }else{
                                printerr(err);
                                fprintf(stderr,"### I cannot rename the input file \"%s\"; skipping it.\n",argv[i]);
                            }
                            unlink(OutputName);
                        }else{
                            fprintf(stderr,"error after convert\n");
                            printerr(err);
                            fprintf(stderr,"### Error while converting the input file \"%s\"; skipping it.\n",argv[i]);
                        }
                    }
                }
                i++;
            }
        }else{
            err=convert(stdin,stdout);
            if(err!=0){
                printerr(err);
                fprintf(stderr,"### Error while converting the input file \"%s\"; skipping it.\n","stdin");
            }
        }
    }else{
        fprintf(stderr,"### An option is needed.\n");
        usage(argv[0]);
        return(1);
    }
    return(0);
}/*main*/

/**** THE END ****/

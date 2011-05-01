/******************************************************************************
FILE:               segment.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program segment a file into several smaller files.
USAGE
    segment [-n 'formatstr'] [-u number] 
            [-s size | [-a|-b 'cutstr'] [-r count] ] [-p] file...

        -n 'formatstr'

            The default -n option is: -n '%s.%03d'.
            The format string is a printf-like format string restricted 
            to %% (for %), %s and %[n]d formats. There must be at least 
            one %d format, and up to 6 %s or %d format specifiers.
            '%s' are filled with the source file name, and '%d' are
            filled with the segment number. Example: 
            segment -n 'directory_%s/%s_%4d/%s.segment' file1 file2
            will create files such as :
            directory_file1/file1_0000/file1.segment
            directory_file1/file1_0001/file1.segment
            directory_file2/file2_0000/file2.segment
            directory_file2/file2_0001/file2.segment
            directory_file2/file2_0002/file2.segment

        -s size
    
            Size is a decimal number of bytes. Defaults to 65536.
            The number may be suffixed by a factor K (2^10), 
            M (2^20) or G (2^30) 

        -u number 
    
            Specify a unique segment to be extracted. Count from 0.

        -a 'cutstr' 
    
            A new segment is created After each 'cutstr' found in the
            input file.
    
        -b 'cutstr' 
    
            A new segment is created Before each 'cutstr' found in the
            input file.
    
        -r count

            This option specify a repeat count to be applied on the
            cut criteria before creating the new segment. Examples :
            segment -s 1024 -r 48 
            create a new segment every 48*1024 bytes.
            segment -a PROCEDURE -r 2 
            create a new segment every other 'PROCEDURE'.
    
        -p
    
            output progress information. Defaults to none.  
    
        -d

            print debugging information.

    Several -a or -b options can be given. The criteria that gives the
    smallest segment is applied.  -s is exclusive from (-a|-b|-r) options.

BUG
    We don't report a bad status when problems occur!

    Current version doesn't create the needed directories.

    We cannot insert the number inside the name: it would need a rex analyser
    such as: -e '/\(.*\)\.\(.*\)/\2-%04d-\1-%s/'.

AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1990-04-28 <PJB> Creation.
    1993-12-18 <PJB> Added -n option.
    1994-12-02 <PJB> Added -a|-b|-r options.
    1994-12-06 <PJB> Debugged.
    1995-11-19 <PJB> Added options. Corrected some bugs and compiled with djgpp.
    2002-11-15 <PJB> Added -u option to extract a single segment.
                     Added factors (K,M,G) for size options.

LEGAL
    GPL
    Copyright Pascal J. Bourguignon 1990 - 2011

    This  program is  free software;  you can  redistribute  it and/or
    modify it  under the  terms of the  GNU General Public  License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    This program  is distributed in the  hope that it  will be useful,
    but  WITHOUT ANY WARRANTY;  without even  the implied  warranty of
    MERCHANTABILITY or FITNESS FOR  A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a  copy of the GNU General Public License
    along with  this program; see the  file COPYING; if  not, write to
    the Free  Software Foundation, Inc.,  59 Temple Place,  Suite 330,
    Boston, MA 02111-1307 USA
******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* On what system is there is #include <macros.h> ? */
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sysexits.h>
#include <stdarg.h>

#include <BcTypes.h>


#define min(a,b)    ((a)<(b)?(a):(b))


#define FileNameSize    (FILENAME_MAX)
#ifndef BufferSize
#define BufferSize      (256*1024)
#endif

const char segment_id[]="$Id";
static BOOLEAN      progress=       FALSE;
static BOOLEAN      debug=          FALSE;
static const char*        pname;

/* #ifdef __WORDSIZE */
/* #if __WORDSIZE != 32 */
/* #error "We were assuming a 32bit wordsize!" */
/* #endif */
/* #endif */

static int fseek64(FILE* stream,long offset,int whence)
{
    long   max=LONG_MAX;
    long   hop=1024*1024*1024;
    long   small_offset;
    int    res;
    switch(whence){
    case SEEK_SET:
        res=fseek(stream,0,SEEK_SET);
        if((res==0)&&(offset>0)){
            return(fseek64(stream,offset,SEEK_CUR));
        }else{
            return(res);
        }
            
    case SEEK_CUR:
        res=0;
        if(offset<0){
            hop=(-hop);
            max=(-max);
            while(max>=offset){
                res=fseek(stream,hop,SEEK_CUR);
                if(res!=0){
                    return(res);
                }
                offset-=hop;
            }
            if(0>offset){
                small_offset=offset;
                res=fseek(stream,small_offset,SEEK_CUR);
            }
        }else{
            while(max<=offset){
                res=fseek(stream,hop,SEEK_CUR);
                if(res!=0){
                    return(res);
                }
                offset-=hop;
            }
            if(0<offset){
                small_offset=offset;
                res=fseek(stream,small_offset,SEEK_CUR);
            }
        }
        return(res);
            
    case SEEK_END:
        res=fseek(stream,0,SEEK_END);
        if(res==0){
            return(fseek64(stream,offset,SEEK_CUR));
        }else{
            return(res);
        }

    default:
        return(EINVAL);
    }
}/*fseek64*/


    
static void MakeSegName(char* segname,char* format,int/*INT16*/ fcase,
                        char* w,CARD32 n)
{
    switch(fcase){
    case  0:    sprintf(segname,format,w,w,w,w,w,w);    break;  
    case  1:    sprintf(segname,format,w,w,w,w,w,n);    break;  
    case  2:    sprintf(segname,format,w,w,w,w,n,w);    break;  
    case  3:    sprintf(segname,format,w,w,w,w,n,n);    break;  
    case  4:    sprintf(segname,format,w,w,w,n,w,w);    break;  
    case  5:    sprintf(segname,format,w,w,w,n,w,n);    break;  
    case  6:    sprintf(segname,format,w,w,w,n,n,w);    break;  
    case  7:    sprintf(segname,format,w,w,w,n,n,n);    break;  
    case  8:    sprintf(segname,format,w,w,n,w,w,w);    break;  
    case  9:    sprintf(segname,format,w,w,n,w,w,n);    break;  
    case 10:    sprintf(segname,format,w,w,n,w,n,w);    break;  
    case 11:    sprintf(segname,format,w,w,n,w,n,n);    break;  
    case 12:    sprintf(segname,format,w,w,n,n,w,w);    break;  
    case 13:    sprintf(segname,format,w,w,n,n,w,n);    break;  
    case 14:    sprintf(segname,format,w,w,n,n,n,w);    break;  
    case 15:    sprintf(segname,format,w,w,n,n,n,n);    break;  
    case 16:    sprintf(segname,format,w,n,w,w,w,w);    break;  
    case 17:    sprintf(segname,format,w,n,w,w,w,n);    break;  
    case 18:    sprintf(segname,format,w,n,w,w,n,w);    break;  
    case 19:    sprintf(segname,format,w,n,w,w,n,n);    break;  
    case 20:    sprintf(segname,format,w,n,w,n,w,w);    break;  
    case 21:    sprintf(segname,format,w,n,w,n,w,n);    break;  
    case 22:    sprintf(segname,format,w,n,w,n,n,w);    break;  
    case 23:    sprintf(segname,format,w,n,w,n,n,n);    break;  
    case 24:    sprintf(segname,format,w,n,n,w,w,w);    break;  
    case 25:    sprintf(segname,format,w,n,n,w,w,n);    break;  
    case 26:    sprintf(segname,format,w,n,n,w,n,w);    break;  
    case 27:    sprintf(segname,format,w,n,n,w,n,n);    break;  
    case 28:    sprintf(segname,format,w,n,n,n,w,w);    break;  
    case 29:    sprintf(segname,format,w,n,n,n,w,n);    break;  
    case 30:    sprintf(segname,format,w,n,n,n,n,w);    break;  
    case 31:    sprintf(segname,format,w,n,n,n,n,n);    break;  
    case 32:    sprintf(segname,format,n,w,w,w,w,w);    break;  
    case 33:    sprintf(segname,format,n,w,w,w,w,n);    break;  
    case 34:    sprintf(segname,format,n,w,w,w,n,w);    break;  
    case 35:    sprintf(segname,format,n,w,w,w,n,n);    break;  
    case 36:    sprintf(segname,format,n,w,w,n,w,w);    break;  
    case 37:    sprintf(segname,format,n,w,w,n,w,n);    break;  
    case 38:    sprintf(segname,format,n,w,w,n,n,w);    break;  
    case 39:    sprintf(segname,format,n,w,w,n,n,n);    break;  
    case 40:    sprintf(segname,format,n,w,n,w,w,w);    break;  
    case 41:    sprintf(segname,format,n,w,n,w,w,n);    break;  
    case 42:    sprintf(segname,format,n,w,n,w,n,w);    break;  
    case 43:    sprintf(segname,format,n,w,n,w,n,n);    break;  
    case 44:    sprintf(segname,format,n,w,n,n,w,w);    break;  
    case 45:    sprintf(segname,format,n,w,n,n,w,n);    break;  
    case 46:    sprintf(segname,format,n,w,n,n,n,w);    break;  
    case 47:    sprintf(segname,format,n,w,n,n,n,n);    break;  
    case 48:    sprintf(segname,format,n,n,w,w,w,w);    break;  
    case 49:    sprintf(segname,format,n,n,w,w,w,n);    break;  
    case 50:    sprintf(segname,format,n,n,w,w,n,w);    break;  
    case 51:    sprintf(segname,format,n,n,w,w,n,n);    break;  
    case 52:    sprintf(segname,format,n,n,w,n,w,w);    break;  
    case 53:    sprintf(segname,format,n,n,w,n,w,n);    break;  
    case 54:    sprintf(segname,format,n,n,w,n,n,w);    break;  
    case 55:    sprintf(segname,format,n,n,w,n,n,n);    break;  
    case 56:    sprintf(segname,format,n,n,n,w,w,w);    break;  
    case 57:    sprintf(segname,format,n,n,n,w,w,n);    break;  
    case 58:    sprintf(segname,format,n,n,n,w,n,w);    break;  
    case 59:    sprintf(segname,format,n,n,n,w,n,n);    break;  
    case 60:    sprintf(segname,format,n,n,n,n,w,w);    break;  
    case 61:    sprintf(segname,format,n,n,n,n,w,n);    break;  
    case 62:    sprintf(segname,format,n,n,n,n,n,w);    break;  
    case 63:    sprintf(segname,format,n,n,n,n,n,n);    break;  
    default:    sprintf(segname,"%s.%03"FMT_CARD32,w,n);break;
    }/*switch*/
}/*MakeSegName*/
    

typedef struct CutS {
    struct CutS*        next;
    const char*         cutstr;
    BOOLEAN             after;
    CARD32              cutlen;
    CARD32              count;
}                   CutT;

static const char* print_format(int size){
    /* if(sizeof(off_t)==sizeof(long long)){ */
    /*     return("%lld"); */
    /* }else */ if(size==sizeof(long)){
        return("%ld");
    }else if(size==sizeof(short)){
        return("%hd");
    }else if(size==sizeof(char)){
        return("%hhd");
    }else{
        return("%d");
    }
}

static const char* concat(const char* strings,...){
    if(strings==0){
        return("");
    }else{
        size_t length=strlen(strings);
        const char* current;
        va_list ap;
        va_start(ap,strings);
        while(0!=(current=va_arg(ap,const char*))){
            length+=strlen(current);
        }
        va_end(ap);
        {
            char* buffer=malloc(length+1);
            char* pos=buffer;
            strcpy(pos,strings); pos+=strlen(strings);
            va_start(ap,strings);
            while(0!=(current=va_arg(ap,const char*))){
                strcpy(pos,current); pos+=strlen(current);
            }
            va_end(ap);
            return(buffer);
        }}}

static CutT* Cut_Alloc(const char* str,BOOLEAN after)
{
    CutT*   cut;
    cut=malloc(sizeof(CutT));
    if(cut==NIL){
        const char* format_string=concat("%s: malloc(",print_format(sizeof(sizeof(CutT))),") returned NIL.\n",0);
        fprintf(stderr,format_string,pname,sizeof(CutT));
        exit(EX_UNAVAILABLE);
    }
    cut->next=NIL;
    cut->cutstr=str;
    cut->after=after;
    cut->cutlen=(CARD32)strlen(str);
    return(cut);
}/*Cut_Alloc*/


static void Cut_AddObject(CutT** list,CutT* obj)
{
    obj->next=(*list);
    (*list)=obj;
}/*Cut_AddObject*/

    


static CARD32 Segment_Copy(FILE* src,FILE* dst,CARD32 size,
                           const char* srcname,CARD32* srcsize,
                           const char* dstname)
/*

  DOES:       Copies at most `size' bytes from `src' stream 
  to the `dst' stream. 
  May copy less when EOF is encountered in `src'.
  RETURNS:    The number of byte written.
*/
{
    char        buffer[BufferSize];
    CARD32      segsize;    /* dst size */
    CARD32      bsize;      /* buffer  size */
    CARD32      csize;      /* current size */

    clearerr(dst);
    if(size>BufferSize){
        bsize=BufferSize;
    }else{
        bsize=size;
    }
    segsize=0;
    if(debug){
        fprintf(stdout,"$ bsize=%"FMT_CARD32"\n",bsize);
        fflush(stdout);
    }
    while((segsize<size)&&(!feof(src))){
        size_t r;
        if(bsize>size-segsize){
            bsize=size-segsize;
        }
        csize=(CARD32)fread(buffer,1,bsize,src);
        (*srcsize)+=csize;
        if(debug){
            fprintf(stdout,"$ in while\n");
            fprintf(stdout,"$ bsize=%"FMT_CARD32"\n",bsize);
            fprintf(stdout,"$ csize=%"FMT_CARD32"\n",csize);
            fflush(stdout);
        }
        if(ferror(src)){
            fprintf(stderr,"### Read error: skipping %s.\n",
                    srcname);
            fflush(stderr);
            fclose(dst);
            break;
        }
        r=fwrite(buffer,1,csize,dst);
        if(ferror(dst)){
            fprintf(stderr,"### Write error on %s: skipping %s.\n",
                    dstname,srcname);
            fflush(stderr);
            fclose(dst);
            break;
        }
        fflush(dst);
        segsize+=csize;
        if(debug){
            fprintf(stdout,"$ csize=%"FMT_CARD32"\n",csize);
            fprintf(stdout,"$ segsize=%"FMT_CARD32"\n",segsize);
            fflush(stdout);
        }
    }
    return(segsize);
}/*Segment_Copy*/

    
static void Segment_Size(char* format,int/*INT16*/ fcase,char* wholename,
                         CARD32 size,INT32 number)
/*
  DOES:       Segments the input file by size.
  If number<0, then segments the file as usual.
  If number>=0, then only the segment `number' 
  is written out.
*/
{
    FILE*       wholefile;
    FILE*       segfile;
    INT32       segcount;
    char        segname[FileNameSize];
    CARD32      wholesize;  /* wholefile size */
    CARD32      segsize;    /* the written size */

    if(progress){
        fprintf(stdout,"# segmenting %s\n",wholename);
        fflush(stdout);
    }
    wholefile=fopen(wholename,"rb");
    if(wholefile==NIL){
        fprintf(stderr,"### Cannot open the file %s: skipping.\n",
                wholename);
        fflush(stderr);
        fclose(wholefile);
        return;
    }

    if(number>=0){
        long offset=((long)size)*number;
        clearerr(wholefile);
        if(0!=fseek64(wholefile,offset,SEEK_SET)){
            perror("fseek64:");
            fclose(wholefile);
            return;
        }

        MakeSegName(segname,format,fcase,wholename,(unsigned)number);
        segfile=fopen(segname,"wb");
        if(segfile==NIL){
            fprintf(stderr,"### Cannot open the file %s: skipping %s.\n",
                    segname,wholename);
            fflush(stderr);
            return;
        }
        segsize=Segment_Copy(wholefile,segfile,size,
                             wholename,&wholesize,segname);
        fclose(segfile);

        if(segsize==0){
            remove(segname);
        }else{
            if(progress){
                fprintf(stdout,"# %s contains %"FMT_CARD32" bytes.\n",segname,segsize);
                fflush(stdout);
            }
        }

        fclose(wholefile);
        return;
    }

    segcount=0;
    wholesize=0;
    clearerr(wholefile);
    while(!feof(wholefile)){
        MakeSegName(segname,format,fcase,wholename,(unsigned)segcount);
        segfile=fopen(segname,"wb");
        if(segfile==NIL){
            fprintf(stderr,"### Cannot open the file %s: skipping %s.\n",
                    segname,wholename);
            fflush(stderr);
            fclose(wholefile);
            return;
        }
        segsize=Segment_Copy(wholefile,segfile,size,
                             wholename,&wholesize,segname);
        fclose(segfile);
        if(segsize==0){
            remove(segname);
        }else{
            if(progress){
                fprintf(stdout,"# %s contains %"FMT_CARD32" bytes.\n",segname,segsize);
                fflush(stdout);
            }
        }
        segcount++;
    }/*while*/
    fclose(wholefile);
    if(progress){
        fprintf(stdout,"# %s contains %"FMT_CARD32" bytes.\n",wholename,wholesize);
        fflush(stdout);
    }
}/*Segment_Size*/


static void Cut_ResetCount(CutT* list,CARD32 count)
{
    while(list!=NIL){
        list->count=count+((list->after)?0:1);
        list=list->next;
    }
}/*Cut_ResetCount*/


static BOOLEAN Cut_GetSlice(CutT* list,const char* line,CARD32* slice)
{
    CutT*       current;
    CARD32      cutlen;
    CARD32      curcut;
    const char* s;
    BOOLEAN     found;
    BOOLEAN     result=FALSE;
    if(debug){
        fprintf(stderr,"Cut_GetSlice \n>>%s",line);
    }

    cutlen=(CARD32)strlen(line);
    current=list;
    while(current!=NIL){
        s=line;
        found=TRUE;
        while(found&&(current->count>0)){
            s=strstr(s,current->cutstr);
            if(s==NULL){
                found=FALSE;
            }else{
                found=TRUE;
                s+=current->cutlen; /* INC */
                (current->count)--;
            }
        }
        if(current->count<=0){
            result=TRUE;
            curcut=(CARD32)(s-line);
            if(!(current->after)){
                /* if before then undo line INC */
                curcut-=current->cutlen;
            }
            if(curcut<cutlen){
                cutlen=curcut;
            }
        }
        current=current->next;
    }
    if(debug){
        fprintf(stderr,"GetSlice=%s cutlen=%"FMT_CARD32"\n",result?"TRUE ":"FALSE",cutlen);
    }
    (*slice)=cutlen;
    return(result);
}/*Cut_GetSlice*/


typedef struct {
    char            save[2];
}               CutStateT;


static void CutBuffer(char* buffer,CARD32 pos,CutStateT* state)
{
    if(buffer[pos-1]=='\n'){
        state->save[0]='\0';
        state->save[1]=buffer[pos];
        buffer[pos]='\0';
    }else{
        state->save[0]=buffer[pos];
        state->save[1]=buffer[pos+1];
        buffer[pos]='\n';
        buffer[pos+1]='\0';
    }
}/*CutBuffer*/


static void UncutBuffer(char* buffer,CARD32 pos,CutStateT* state)
{
    if(state->save[0]=='\0'){
        buffer[pos]=state->save[1];
    }else{
        buffer[pos]=state->save[0];
        buffer[pos+1]=state->save[1];
    }
}/*UncutBuffer*/


static FILE* sopen(const char* fname,const char* mode)
{
    FILE*       file;
    file=fopen(fname,mode);
    if(file==NIL){
        fprintf(stderr,"### Cannot open the file '%s'.\n",fname);
        fflush(stderr);
    }
    return(file);
}/*sopen*/


static void sclose(FILE* file)
{
    fclose(file);
}/*sclose*/

#if 0
static BOOLEAN sread(FILE* file,char* buffer,int size)
{
    int         count;
    char        message[BUFSIZ];
        
    if(feof(file)){
        return(FALSE);
    }
    clearerr(file);
    while(size>0){
        count=fread(buffer,sizeof(char),size,file);
        if(count<=0){
            switch(ferror(file)){
            case EAGAIN:
            case EINTR:
                continue;
            default:
                sprintf(message,"%s: error while reading",pname);
                perror(message);
                return(FALSE);
            }
        }else{
            buffer+=count;
            size-=count;
        }
    }
    return(TRUE);
}/*sread*/


static BOOLEAN swrite(FILE* file,char* buffer,int size)
{
    int         count;
    char        message[BUFSIZ];
        
    if(feof(file)){
        return(FALSE);
    }
    clearerr(file);
    while(size>0){
        count=fwrite(buffer,sizeof(char),size,file);
        if(count<=0){
            switch(ferror(file)){
            case EAGAIN:
            case EINTR:
                continue;
            default:
                sprintf(message,"%s: error while writting",pname);
                perror(message);
                return(FALSE);
            }
        }else{
            buffer+=count;
            size-=count;
        }
    }
    return(TRUE);
}/*swrite*/
#endif

static BOOLEAN sgets(char* buffer,int size,FILE* file)
{
    char        message[BUFSIZ];
    char* r;
    if(feof(file)){
        return(FALSE);
    }
    clearerr(file);
    r=fgets(buffer,size,file);
    if(ferror(file)!=0){
        sprintf(message,"%s: error while reading",pname);
        perror(message);
        return(FALSE);
    }else{
        return(TRUE);
    }
}/*sgets*/



static BOOLEAN sputs(char* buffer,FILE* file)
{
    char        message[BUFSIZ];
        
    if(feof(file)){
        return(FALSE);
    }
    clearerr(file);
    fputs(buffer,file);
    if(ferror(file)!=0){
        sprintf(message,"%s: error while writting",pname);
        perror(message);
        return(FALSE);
    }else{
        return(TRUE);
    }
}/*sputs*/


static void copystring(char* src,char* dst,int cnt)
{
    /* strncpy does not guarantee anything when src and dst overlap */
    while(((*src)!='\0')&&(cnt-->0)){
        (*dst++)=(*src++);
    }
    (*dst)='\0';
}/*copystring*/


static void Segment_CutFile(FILE* wholefile,FILE* segfile,
                            CutT* cuts,CARD32 repcnt)
{
    static char         buffer[BufferSize];
    static CARD32       buflen=0;   /* # of data bytes in the buffer */

    char*       curbuf;     /* where in buffer to load the next line */
    CARD32      wholesize;  /* wholefile size */
    CARD32      segsize;    /* segfile size */
    CARD32      cutlen; 
    CutStateT   cutstate;
    BOOLEAN     endofsegment;

    Cut_ResetCount(cuts,repcnt);
    segsize=0;
    endofsegment=FALSE;
    while(!(endofsegment||feof(wholefile))){
        curbuf=buffer+buflen;
        if(sgets(curbuf,(signed)(BufferSize-buflen),wholefile)){
            wholesize+=(CARD32)strlen(curbuf);
            buflen=(CARD32)strlen(buffer);
            if(Cut_GetSlice(cuts,buffer,&cutlen)){
                endofsegment=TRUE;
                if(0<cutlen){
                    CutBuffer(buffer,cutlen,&cutstate);
                    if(segfile!=0){
                        sputs(buffer,segfile);
                    }
                    UncutBuffer(buffer,cutlen,&cutstate);
                    buflen-=cutlen;
                    copystring(buffer+cutlen,buffer,(signed)buflen);
                }
            }else{
                if(segfile!=0){
                    sputs(buffer,segfile);
                }
                buflen=0;
            }
        }else{
            endofsegment=TRUE;
        }
        segsize+=cutlen;
    }/*while*/
}/*Segment_CutFile*/


static void Segment_Cut(char* format,int/*INT16*/ fcase,char* wholename,
                        CutT* cuts,CARD32 repcnt,INT32 number)
/*
  DO:         Segment the input file by cutting strings.
*/
{
    FILE*       wholefile;
    INT32       segcount=0;

    wholefile=sopen(wholename,"r");     if(wholefile==NULL){ return; }
    while(!feof(wholefile)){
        FILE*       segfile;
        char        segname[FileNameSize];
        
        if((number<0)||(number==segcount)){
            MakeSegName(segname,format,fcase,wholename,(unsigned)segcount++);
            segfile=sopen(segname,"w"); if(segfile==NULL){ break; }
            Segment_CutFile(wholefile,segfile,cuts,repcnt);
            sclose(segfile);
        }else{
            Segment_CutFile(wholefile,0,cuts,repcnt);
        }
    }
    sclose(wholefile);
}/*Segment_Cut*/


#if 0 
static void Segment_Cut(char* format,INT16 fcase,char* wholename,
                        CutT* cuts,CARD32 repcnt)
/*
  DO:         Segment the input file by cutting strings.
*/
{
    FILE*       wholefile;
    FILE*       segfile;
    CARD16      segcount;
    char        segname[FileNameSize];
    char        buffer[BufferSize];
    char*       curbuf;     /* where in buffer to load the next line */
    CARD32      wholesize;  /* wholefile size */
    CARD32      segsize;    /* segfile size */
    CARD32      buflen;     /* number of data bytes in the buffer */
    CARD32      cutlen; 
    char*       s;
    char*       d;
    CARD32      c;
    CutStateT   cutstate;
    BOOLEAN     endofsegment;

    if(progress){
        fprintf(stdout,"# segmenting %s\n",wholename);
        fflush(stdout);
    }
    /* Segment: Open input file. */
    wholefile=fopen(wholename,"r");
    if(wholefile==NIL){
        fprintf(stderr,"### Cannot open the file %s: skipping.\n",wholename);
        fflush(stderr);
    }else{
        segcount=0;
        wholesize=0;
        buflen=0;
        clearerr(wholefile);
        /* segment the file */
        while(!feof(wholefile)){
            /* create a segment */
            MakeSegName(segname,format,fcase,wholename,segcount);
            segfile=fopen(segname,"w");
            if(segfile==NIL){
                fprintf(stderr,"### Cannot open the file %s: skipping %s.\n",segname,wholename);
                fflush(stderr);
                break;
            }else{
                clearerr(segfile);
                Cut_ResetCount(cuts,repcnt);
                segsize=0;
                endofsegment=FALSE;
                while(!(endofsegment||feof(wholefile))){
                    curbuf=buffer+buflen;
                    fprintf(stderr,"buflen=%d\n",buflen);
                    if(fgets(curbuf,BufferSize-buflen,wholefile)==NIL){
                        /* error or end-of-file */
                        if(!feof(wholefile)){
                            char        message[BUFSIZ];
                            sprintf(message,
                                    "%s: error while reading a line",pname);
                            perror(message);
                        }else{
                            /* should not occur since we tested for feof */
                        }
                    }else{
                        wholesize+=strlen(curbuf);
                        buflen=strlen(buffer);
                        cutlen=Cut_GetSlice(cuts,buffer);
                        fprintf(stderr,"cutlen=%d\n",cutlen);
                        if(cutlen<buflen){
                            endofsegment=TRUE;
                            if(0<cutlen){
                                CutBuffer(buffer,cutlen,&cutstate);
                                fputs(buffer,segfile);
                                UncutBuffer(buffer,cutlen,&cutstate);
                                buflen-=cutlen;
                                /* strncpy does not guarantee anything
                                   when source and destination overlap */
                                for(s=buffer+cutlen,d=buffer,c=buflen;
                                    (*s!='\0')&&(c>0);s++,d++,c--){
                                    (*d)=(*s);
                                }
                                (*d)='\0';
                            }
                        }else{
                            fputs(buffer,segfile);
                            buflen=0;
                        }
                        if(ferror(segfile)){
                            fprintf(stderr,"### Write error on %s: skipping %s.\n",segname,wholename);
                            fflush(stderr);
                            fclose(segfile);
                            break;
                        }
                    }/*if fgets*/
                    fflush(segfile);
                    segsize+=cutlen;
                    if(debug){
                        fprintf(stdout,"$ cutlen =%d\n",cutlen);
                        fprintf(stdout,"$ segsize=%d\n",segsize);
                        fflush(stdout);
                    }
                }/*while copying segment*/
                fclose(segfile);
                if(segsize==0){
                    /* this can occur only for the last segment */
                    remove(segname);
                }else{
                    if(progress){
                        fprintf(stdout,"# %s contains %d bytes.\n",segname,segsize);
                        fflush(stdout);
                    }
                }
            }/*if can open output file*/
            segcount++;
        }/*while !feof(wholefile)*/
        fclose(wholefile);
        if(progress){
            fprintf(stdout,"# %s contains %d bytes.\n",wholename,wholesize);
            fflush(stdout);
        }
    }/*if can open input file*/
}/*Segment_Cut*/
    
#endif

    
/* getparams - main */
    
    
static void usage(const char* upname)
{
    fprintf(stderr,
            "%s usage:\n\t%s [-p] [-n 'format'] [-u number]\n"\
            "\t\t( [-s size] | ( -a|-b 'cut' | -r count )... ) file...\n",
            upname,upname);
}/*usage*/


static int getnextparam(int argc,char* argv[],INT32 params,
                        INT32* v1,char* v2,
                        const char* name,const char* Name,
                        const char* scanformat,const char* message)
{
    int     res;

    if(params>=argc){
        fprintf(stderr,"### %s - %s is missing.\n",argv[0],Name);
        usage(argv[0]);
        exit(EX_USAGE);
    }

    res=sscanf(argv[params],scanformat,v1,v2);
    if(res<=0){
        fprintf(stderr,"### %s - Syntax error in %s:%s\n%s\n",
                argv[0],name,argv[params],message);
        usage(argv[0]);
        exit(EX_USAGE);
    }
    return(res);
}/*getnextparam*/


static INT16 ComputeFormatCase(char* format)
{
    INT16           fcase=0;
    INT16           bit=0x20;
    BOOLEAN         dexists=FALSE;
            
    if(format==NIL){
        fprintf(stderr,"Warning: missing a format string. \n"
                "Will use the default format '%%s.%%03d'\n");
        return(-1);
    }
    while(*format!=0){
        if(*format=='%'){
            format++;
            switch(*format){
            case '%':
                format++;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                do{
                    format++;
                }while(isdigit(*format));
                if(*format!='d'){
                    fprintf(stderr,"Warning: only %%d or %%[0-9]*s "
                            "specifiers allowed. \n"
                            "Will use the default format '%%s.%%03d'\n");
                    return(-1);
                }
                /* no break;*/
            case 'd':   /* decimal*/
                if(bit==0){
                    fprintf(stderr,"Warning: too many format "
                            "specifiers (max=6). \n"
                            "Will use the default format '%%s.%%03d'\n");
                    return(-1);
                }
                fcase|=bit;
                bit>>=1;
                dexists=TRUE;
                break;
            case 's':   /* string*/
                if(bit==0){
                    fprintf(stderr,"Warning: too many format "
                            "specifiers (max=6). \n"
                            "Will use the default format '%%s.%%03d'\n");
                    return(-1);
                }
                bit>>=1;
                break;
            default:
                fprintf(stderr,"Warning: only %%d or %%[0-9]*s "
                        "specifiers allowed. \n"
                        "Will use the default format '%%s.%%03d'\n");
                return(-1);
                break;
            }/*switch*/
        }else{
            format++;
        }
    }
    if(dexists){
        return(fcase);
    }else{
        fprintf(stderr,"Warning: missing a %%d specifier. \n"
                "Will use the default format '%%s.%%03d'\n");
        return(-1);
    }
}/*ComputeFormatCase;*/
    


static void error_exclusive_options(void)
{
    fprintf(stderr,
            "%s: -s and -a|-b|-r options are mutually exclusive\n",pname);
    usage(pname);
    exit(EX_USAGE);
}/*error_exclusive_options*/


int main(int argc,char* argv[])
{
    const CARD32    defaultsize=    65536;
    CARD32          lastsize;
    CARD32          size=           0;
    CutT*           cuts=           NIL;
    CARD32          count=          1;
    INT16           fcase=          -1;
    char*           format=         NIL;
    CARD16          param;
    CARD16          length;
    BOOLEAN         file;
    char**          fnames;
    CARD32          fcount;
    INT32           number=         -1;

    fnames=malloc(sizeof(char*)*(size_t)argc);
    fcount=0;

    pname=argv[0];
    file=FALSE;
    if(argc<=1){
        usage(argv[0]);
        exit(EX_USAGE);
    }

    param=1;
    while(param<argc){

        if(argv[param][0]!='-'){
            /* file ... */
            fnames[fcount++]=argv[param++];
            continue;
        }

        length=(CARD16)strlen(argv[param]);
        if(length!=2){
        illegal_option:
            fprintf(stderr,
                    "### %s - Illegal option: \"%s\"!\n",
                    argv[0],argv[param]);
            usage(argv[0]);
            exit(EX_USAGE);
        }

        switch(argv[param][1]){
        case 'a':
            if(size!=0){
                error_exclusive_options();
            }
            param++;
            Cut_AddObject(&cuts,Cut_Alloc(argv[param],TRUE));
            param++;
            break;
        case 'b':
            if(size!=0){
                error_exclusive_options();
            }
            param++;
            Cut_AddObject(&cuts,Cut_Alloc(argv[param],FALSE));
            param++;
            break;
        case 'r':
            if(size!=0){
                error_exclusive_options();
            }
            param++;
            getnextparam(argc,argv,param,(INT32*)&count,0,
                         "count","Count","%lu",
                         "# It must be a decimal integer. Example: 10240");
            if(count==0){
                count=1;
            }
            param++;
            break;
        case 's':
            if(cuts!=NIL){
                error_exclusive_options();
            }
            param++;
            {
                char    factorstr[4];
                int     numfields;
                numfields=getnextparam(argc,argv,param,
                                       (INT32*)&lastsize,factorstr,
                                       "size","Size","%lu%1[KMGkmg]",
                                       "# It must be a decimal integer. "\
                                       "Example:  10240");
                if(numfields==2){
                    switch(factorstr[0]){
                    case 'K': case 'k': lastsize*=1024;           break;
                    case 'M': case 'm': lastsize*=1024*1024;      break;
                    case 'G': case 'g': lastsize*=1024*1024*1024; break;
                    default:
                        fprintf(stderr,
                                "### %s - the factor must be either "\
                                "K, M or G, not %c\n",
                                argv[0],factorstr[0]);
                        usage(argv[0]);
                        exit(EX_USAGE);
                    }
                }
            }
            if(size==0){
                size=lastsize;
            }else{
                size=min(size,lastsize);
            }
            param++;
            break;
        case 'u':
            param++;
            getnextparam(argc,argv,param,(INT32*)&number,0,
                         "number","Number","%lu",
                         "# It must be a decimal integer. Example: 4");
            param++;
            break;
        case 'p':
            progress=TRUE;
            param++;
            break;
        case 'd':
            debug=TRUE;
            param++;
            break;
        case 'n':
            param++;
            format=argv[param];
            param++;
            fcase=ComputeFormatCase(format);
            if(debug){
                fprintf(stderr,
                        "fcase=%x\n",fcase);
            }
            break;
        default:
            goto illegal_option;
            break;
        }
    }/*while argv*/

    if((size==0)&&(cuts==NIL)){
        size=defaultsize;
    }

    if(fcount==0){
        fprintf(stderr,
                "### %s - you must give a file name!\n",argv[0]);
        usage(argv[0]);
        exit(EX_USAGE);
    }
    if(debug){
        progress=TRUE;
    }
    if(size==0){
        /* -a or -b cuts */
        CARD32  i;
        for(i=0;i<fcount;i++){
            Segment_Cut(format,fcase,fnames[i],cuts,count,number);
        }
    }else{
        /* fixed size segments */
        CARD32  i;
        for(i=0;i<fcount;i++){
            Segment_Size(format,fcase,fnames[i],size,number);
        }
    }

    fflush(stdout);
    fflush(stderr);
    return(EX_OK);
}/*main*/

/**** THE END ****/

/******************************************************************************
FILE:               encoding.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program converts text files to and from several encodings.
USAGE
    nema [-t] [-r] [-v] [-xC] [-a|-e [-ELR]] -from code -to code <input >output
    nema [-t] [-r] [-v] [-xC] [-a|-e [-ELR]] -from code -to code file...
    nema [-t] [-r] [-v] [-xC] [-a|-e [-ELR]] -n|-m               <input >output
    nema [-t] [-r] [-v] [-xC] [-a|-e [-ELR]] -n|-m file...
    nema [-t] [-r] [-v] [-xC] -A < input > output
    nema [-t] [-r] [-v] [-xC] -A file...

        -from   specify the code used for the input file.

        -to     specify the code used for the output file.

            This version translates between any of those codes:
                ascii       (7-bit ASCII)
                isolatin1
                macintosh
                msdos
                mswindows
                next
                standard    (Postscript standard encoding)
                symbol      (Postscript Symbol font encoding)
                honeywell200bcd
                honeywell600bcd
                univacfielddata
                ibmbcd
                ebcdic

            Future versions may also implement translations with:
                ebcdic, hpux, sun, aix, ...

        -t      first dumps the conversion table to stderr.
        
        -n      is a shortcut for "-from macintosh -to next"

        -m      is a shortcut for "-from next -to macintosh"

        -a      use an approximation table to convert characters without 
                output encoding to plain ASCII.  This option may change
                the file size. Usually used with '-to ascii'.
        
        -e      use the character name for characters without
                output encoding. This option may change the file size.

        -ELR    the character names are quoted with L and R characters.
                Default: -E{}
                                    
        -A      simply filter out the 8th bit of each character, thus
                producing a pure ASCII output. (Same functionnality as
                text(1) filter).

        -r      specify the files are Rich Text Format files.
                in this case, the sequences \'xx where xx is an
                hexadecimal number are interpreted as source codes 
                and translated too; output codes greater than 127 
                are converted into \'xx sequences.

        -v      prints on stderr the list of missing characters.

        -xC     specify the character C to be used when no 
                corresponding character exists (default is space).
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    $Id: encoding.c,v 1.2 2004/06/24 18:19:37 pjbpjb Exp $
    $Log: encoding.c,v $
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
    
    1992/04/30 <PJB> Creation.
    1992/05/06 <PJB> Added -r option for RTF files.
    1992/07/04 <PJB> Added -a option to remove accents
                        (converts to a pure ASCII file).
                        Added -from, -to and code options.
    1992/10/09 <PJB> Added -A option to reset the 8th bit to 0.
    1993/02/12 <PJB> New conversion algorithm: now using encoding vectors.
                        Added msdos encoding.
                    NOT COMPLETED!
    1996/12/05 <PJB> Corrected a bug in BuildConversionTable with approx mode.
    1999/09/14 <PJB> Extracted the encodings into a separate library.
LEGAL
    Copyright Pascal J. Bourguignon 1991 - 2011
    All Rights Reserved.
    This program may not be included in any commercial product without the 
    author written permission. It may be used freely for any non-commercial 
    purpose, provided that this header is always included.
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(NeXT)
#include <sys/types.h>
#include <sys/stat.h>
#include <libc.h>
#endif
#if defined(AIX)
#include <sys/types.h>
#include <sys/stat.h>
#endif
#if defined(linux)
#include <unistd.h>
#include <sys/stat.h>
#endif

#include <concatenate.h>
#include <BcTypes.h>
#include <Bencoding.h>


static void printerr(int err)
{
    fprintf(stderr,"Error: %s (%d)\n",strerror(err),err);
}/*printerr*/
    

static void MakeTempName(char* src,char* temp)
{
    strcpy(temp,src);
    strcat(temp,".nema[ ]");
}/*MakeTempName*/
    

static BOOLEAN ishexadigit(/*char*/int c)
{
    return((('0'<=c)&&(c<='9'))||
           (('a'<=c)&&(c<='f'))||(('A'<=c)&&(c<='F')));
}/*ishexadigit;*/
    
    
static int hexatoint(/*char*/int c)
{
    if(c<='9'){
        return(c-'0');
    }else if(c<='F'){
        return(c-'A'+10);
    }else{
        return(c-'a'+10);
    }
}/*hexatoint;*/
    
#if 0   
static char inttohexa(int d)
{
    if(d<10){
        return('0'+d);
    }else{
        return('a'+d-10);
    }
}/*inttohexa;*/
#endif  


static void usage(char* pname)
{
    INT16        code;
    INT16        col;
    const char*  message="# Implemented codes are: ";
    const char** codeNames=BenEncodingTableNames();

    fprintf(
            stderr,
            concatenate(
                        "# %s usage:\n",
                        "   %s -from code -to code ",
                        "[-a | -e [-ELR]] [-t] [-r] [-v] [-xC] < in > out \n",
                        "   %s -from code -to code ",
                        "[-a | -e [-ELR]] [-t] [-r] [-v] [-xC] file... \n",
                        "   %s -n | -m [-a | -e [-ELR]] [-t] [-r] ",
                        "[-v] [-xC] < in > out \n",
                        "   %s -n | -m [-a | -e [-ELR]] [-t] [-r] ",
                        "[-v] [-xC] file... \n",
                        "   %s -A [-t] [-r] [-v] [-xC] < in > out \n",
                        "   %s -A [-t] [-r] [-v] [-xC] file... \n",
                        "     -t   = dumps the conversion table,\n",
                        "     -a   = approximate (use it with -to ascii),\n",
                        "     -n   = -from macintosh -to next\n",
                        "     -m   = -from next -to macintosh\n",
                        "     -e   = replace missing characters in ",
                        "'to' set with their name,\n",
                        "     -xC  = replace missing characters in ",
                        "'to' set with C,\n",
                        "     -ELR = encasulate missing characters ",
                        "in 'to' set with L and R,\n",
                        "     -v   = prints on stderr the list of ",
                        "missing characters,\n",
                        "     -r   = the files are Rich Text Format files,\n",
                        "     -A   = filters all remaining 8-ths bits.\n",
                        0),pname,pname,pname,pname,pname,pname,pname);
    code=0;
    col=(INT16)(strlen(message)+strlen(codeNames[code])+2);
    fprintf(stderr,"%s%s",message,codeNames[code]);
    code++;
    while(codeNames[code]!=0){
        col=(INT16)((size_t)col+strlen(codeNames[code])+2);
        if(col>72){
            fprintf(stderr,",\n    %s",codeNames[code]);
            col=(INT16)(col-68);
        }else{
            fprintf(stderr,", %s",codeNames[code]);
        }
        code++;
    }
    fprintf(stderr,".\n");
}/*usage*/
    


/* ConvertEncoding arguments */
static BenConversionTableT      Conversion;
static BOOLEAN                  rtf;


static int ConvertEncoding(FILE* in,FILE* out)
{
    int             c;
            
    if(rtf){
        c=fgetc(in);
        while(c!=EOF){
            if(c=='\\'){
                c=fgetc(in);
                if(c=='\''){
                    int             h,l;
                    h=fgetc(in);
                    l=fgetc(in);
                    if(ishexadigit(h)&&ishexadigit(l)){
                        c=hexatoint(h)*16+hexatoint(l);
                        fputs(Conversion[c],out);
                        /*** SEE: hexa escapes are not kept!
                             buffer[i+2]=inttohexa(new/16);
                             buffer[i+3]=inttohexa(new%16);
                        ***/
                    }else{
                        fputc('\\',out);
                        fputc('\'',out);
                        fputc(h,out);
                        fputc(l,out);
                    }
                }else{
                    fputc('\\',out);
                    fputc(c,out);
                }
            }else{
                fputs(Conversion[c],out);
            }
            c=fgetc(in);
        }
    }else{
        c=fgetc(in);
        while(c!=EOF){
            fputs(Conversion[c],out);
            c=fgetc(in);
        }
    }
    return(0);
}/*ConvertEncoding*/
    

#define BufSize (128*1024)
static int Filter8thBit(FILE* in,FILE* out)
{
    unsigned char   buffer[BufSize];
    size_t          rlength;
    size_t          wlength;
    int             i;
            
    while(!feof(in)){
        rlength=fread(buffer,1,BufSize,in);
        i=0;
        while(i<rlength){
            buffer[i]=(char)(buffer[i]&0x7F);
            i++;
        }
        if(rlength>0){
            wlength=fwrite(buffer,1,(unsigned)rlength,out);
            if(wlength!=rlength){
                fprintf(stderr,
                        "### Written length is different from read length:"
                        " %ld != %ld\n",wlength,rlength);
            }
        }
    }
    return(0);
}/*Filter8thBit*/


int main(int argc,char** argv,char** envp)
{
    FILE*               fin;
    FILE*               fout;
    int                 i;
    int                 err;
    struct stat         filestatus;
    char                TempName[256];
    BOOLEAN             nooption;
    BOOLEAN             fromIsDetermined;
    BOOLEAN             toIsDetermined;
    BOOLEAN             eight2seven;
    BOOLEAN             dumpTable;
    BenModeT            mode;
    const char*         fromname=0;
    const char*         toname=0;
    BenEncodingTableT*  fromtable=(BenEncodingTableT*)0;
    BenEncodingTableT*  totable=(BenEncodingTableT*)0;
    char                replacement[2]=" "; /* space is default replacement */
    int (*convert)(FILE*,FILE*);
        
    rtf=FALSE;
    nooption=TRUE;
    fromIsDetermined=FALSE;
    toIsDetermined=FALSE;
    eight2seven=FALSE;
    dumpTable=FALSE;
    mode=BenMode_Normal;
    i=1;
    while((i<argc)&&(argv[i][0]=='-')){
        if(strcmp(argv[i],"-n")==0){
            nooption=FALSE;
            fromtable=BenEncodingTableNamed("macintosh");
            totable=BenEncodingTableNamed("nextstep");
            fromIsDetermined=TRUE;
            toIsDetermined=TRUE;
        }else if(strcmp(argv[i],"-t")==0){
            dumpTable=TRUE;
        }else if(strcmp(argv[i],"-m")==0){
            nooption=FALSE;
            fromtable=BenEncodingTableNamed("nextstep");
            totable=BenEncodingTableNamed("macintosh");
            fromIsDetermined=TRUE;
            toIsDetermined=TRUE;
        }else if(strcmp(argv[i],"-a")==0){
            mode=BenMode_Approximation;
        }else if(strcmp(argv[i],"-A")==0){
            nooption=FALSE;
            eight2seven=TRUE;
        }else if(strcmp(argv[i],"-from")==0){
            nooption=FALSE;
            i++;
            if(i<argc){
                fromtable=BenEncodingTableNamed(argv[i]);
                if(fromtable==(BenEncodingTableT*)0){
                    fprintf(stderr,"### Invalid from code: <%s>.\n",argv[i]);
                    usage(argv[0]);
                    return(1);
                }
                fromname=argv[i];
                fromIsDetermined=TRUE;
            }else{
                fprintf(stderr,
                        "### A code name is expected following -from option.\n");
                usage(argv[0]);
                return(1);
            }
        }else if(strcmp(argv[i],"-to")==0){
            nooption=FALSE;
            i++;
            if(i<argc){
                totable=BenEncodingTableNamed(argv[i]);
                if(totable==(BenEncodingTableT*)0){
                    fprintf(stderr,"### Invalid to code: <%s>.\n",argv[i]);
                    usage(argv[0]);
                    return(1);
                }
                toname=argv[i];
                toIsDetermined=TRUE;
            }else{
                fprintf(stderr,
                        "### A code name is expected following -to option.\n");
                usage(argv[0]);
                return(1);
            }
        }else if(strcmp(argv[i],"-r")==0){
            rtf=TRUE;
        }else if(argv[i][1]=='x'){
            replacement[0]=argv[i][2];
        }else{
            fprintf(stderr,"### Bad option: <%s>.\n",argv[i]);
            usage(argv[0]);
            return(1);
        }
        i++;
    }

    if(nooption){
        fprintf(stderr,"### Either -A or -from and -to is mandatory.\n");
        usage(argv[0]);
        return(1);
    }

    if(eight2seven){
        if(fromIsDetermined||toIsDetermined||(mode!=BenMode_Normal)){
            fprintf(stderr,
                    "### -A option is incompatible with -from, -to, -a and -e\n");
            usage(argv[0]);
            return(1);
        }
        convert=Filter8thBit;
    }else if(fromIsDetermined&&toIsDetermined){
        BenBuildConversionTable(mode,fromtable,totable,
                                BenApproximationTableNamed("approximation"),
                                replacement,&Conversion);
        convert=ConvertEncoding;
    }else{
        fprintf(stderr,"### Both -from and -to are mandatory.\n");
        usage(argv[0]);
        return(1);
    }

    if(dumpTable){
        int ii;
        fprintf(stderr,"static const char* Conversion_%s_to_%s[256]={\n",
                fromname,toname);
        for(ii=0;ii<256;ii++){
            fprintf(stderr,"        /* %3d */ \"%s\",\n",
                    ii,BcString_String(BenEscapeString(Conversion[ii])));
        }
        fprintf(stderr,"    };\n");
    }
    
    if(i<argc){
        while(i<argc){
            fin=fopen(argv[i],"r");
            if(fin==NULL){
                fprintf(stderr,
                        "### I cannot open the input file \"%s\";"
                        " skipping it.\n",argv[i]);
            }else{
                MakeTempName(argv[i],TempName);
                fout=fopen(TempName,"w");
                if(fout==NULL){
                    fprintf(stderr,
                            "### I cannot open the output file \"%s\";"
                            " skipping \"%s\".\n",TempName,argv[i]);
                    fclose(fin);
                }else{
                    err=convert(fin,fout);
                    fclose(fin);
                    fclose(fout);
                    if(err==0){
                        err=rename(argv[i],"temp[nema]");
                        if(err==0){
                            err=link(TempName,argv[i]);
                            if(err==0){
                                err=stat("temp[nema]",&filestatus);
                                if(err==0){
                                    int r=chown(argv[i],filestatus.st_uid,
                                                filestatus.st_gid);
                                    (void)r;
                                    chmod(argv[i],filestatus.st_mode);
                                }
                                unlink("temp[nema]");
                            }else{
                                perror("Error while linking: ");
                                fprintf(stderr,
                                        "### I cannot link the output file "
                                        "\"%s\" to the input file \"%s\"; "
                                        "skipping it.\n",TempName,argv[i]);
                                rename("temp[nema]",argv[i]);
                            }
                        }else{
                            printerr(err);
                            fprintf(stderr,
                                    "### I cannot rename the input file \"%s\";"
                                    " skipping it.\n",argv[i]);
                        }
                        unlink(TempName);
                    }else{
                        fprintf(stderr,"error after convert\n");
                        printerr(err);
                        fprintf(stderr,
                                "### Error while converting the input file "
                                "\"%s\"; skipping it.\n",argv[i]);
                    }
                }
            }
            i++;
        }
    }else{
        err=convert(stdin,stdout);
        if(err!=0){
            printerr(err);
            fprintf(stderr,
                    "### Error while converting the input file \"%s\";"
                    " skipping it.\n","stdin");
        }
    }
    return(0);
}/*main*/
    

/**** THE END ****/

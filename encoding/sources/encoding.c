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
    nema -T code
    nema --help

        -from   specify the code used for the input file.

        -to     specify the code used for the output file.

            This version translates between any of those codes:
                ascii           (7-bit)
                isolatin1       (8-bit, ASCII extension)
                macintosh       (8-bit)
                msdos           (8-bit)
                mswindows       (8-bit, isolatin1 extension)
                nextstep        (8-bit)
                standard        (8-bit, Postscript standard encoding)
                symbol          (8-bit, Postscript Symbol font encoding)
                honeywell200bcd (6-bit)
                honeywell600bcd (6-bit)
                univacfielddata (6-bit)
                ibmbcd          (6-bit)
                ebcdic          (8-bit)

            Future versions may also implement translations with:
                ebcdic, hpux, sun, aix, ...

        -t      dumps the conversion table. (no conversion done).

        -T code dumps the code table, displaying the character names.
                (no conversion done).

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
    2000-02-24 <PJB> Renamed 'next' code to 'nextstep'.
    1996-12-05 <PJB> Corrected a bug in BuildConversionTable with approx mode.
    1993-02-12 <PJB> New conversion algorithm: now using encoding vectors.
                     Added msdos encoding.
                     NOT COMPLETED!
    1992-10-09 <PJB> Added -A option to reset the 8th bit to 0.
    1992-07-04 <PJB> Added -a option to remove accents
                     (converts to a pure ASCII file).
                     Added -from, -to and code options.
    1992-05-06 <PJB> Added -r option for RTF files.
    1992-04-30 <PJB> Creation.
LEGAL
    GPL
    
    Copyright Pascal J. Bourguignon 1991 - 2003
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version
    2 of the License, or (at your option) any later version.
    
    This program is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE.  See the GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public
    License along with this program; if not, write to the Free
    Software Foundation, Inc., 59 Temple Place, Suite 330,
    Boston, MA 02111-1307 USA
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include <sys/types.h>
#include <sys/stat.h>

#if defined(NeXT)
#include <libc.h>
#endif

#if defined(AIX)
#include <sys/types.h>
#include <sys/stat.h>
#endif

#if defined(linux) || defined(MacOSX)
#include <unistd.h>
#include <errno.h>
#else
    extern int      errno;
    extern char*    sys_errlist[];
    extern int      sys_nerr;
#endif

        /* #include <enctypes.h> */
        /* #include <approxim.h> */
        /* #include <encascii.h> */
        /* #include <encisla1.h> */
        /* #include <encmacos.h> */
        /* #include <encmsdos.h> */
        /* #include <encmswin.h> */
        /* #include <encnxstp.h> */
        /* #include <encpsstd.h> */
        /* #include <encsymbl.h> */
        /* #include <ench2bcd.h> */
        /* #include <ench6bcd.h> */
        /* #include <encunifd.h> */
        /* #include <encibbcd.h> */
        /* #include <encebcdi.h> */

#include <bjstring.h>
#include <bjarray.h>

#include <Bencoding.h>



#define STR_SWITCH(str)      {const char* str_switch_string=(str);if(0){
#define STR_CASE(str)        }else if(0==strcmp(str_switch_string,(str))){
#define STR_CASE2(str1,str2) }else if((0==strcmp(str_switch_string,(str1)))\
                                    ||(0==strcmp(str_switch_string,(str2)))){
#define STR_DEFAULT          }else{
#define STR_END              }}




    static void printerr(int err)
    {
        fprintf(stderr,"Error: %s (%d)\n",strerror(err),err);
    }/*printerr*/
    

    static void MakeTempName(char* src,char* temp)
    {
        strcpy(temp,src);
        strcat(temp,".nema[ ]");
    }/*MakeTempName*/
    

    static BOOLEAN ishexadigit(int/*char*/ c)
    {
        return((('0'<=c)&&(c<='9'))||
               (('a'<=c)&&(c<='f'))||(('A'<=c)&&(c<='F')));
    }/*ishexadigit;*/
    
    
    static int hexatoint(int/*char*/ c)
    {
        if(c<='9'){
            return(c-'0');
        }else if(c<='F'){
            return(c-'A'+10);
        }else{
            return(c-'a'+10);
        }
    }/*hexatoint;*/
    
    

    static void print_words(FILE* out,bjarray_t* words,unsigned int column,
                            unsigned int left,unsigned int right)
        /*
            PRE:    left<right
            DO:     Prints words, separated by one space, flowing them
            from left margin to right margin.
        */
    {
        unsigned int count=bjarray_count(words);
        unsigned int i=0;
        while(i<count){
            bjstring_t* word=bjarray_element_at(words,i++);
            unsigned int length=bjstring_length(word);
            if(column<left){
                fprintf(out,"%*s",left,"");
                column=left;
            }else if(column+length>right){
                fprintf(out,"\n%*s",left,"");
                column=left;
            }
            fprintf(out,"%s ",bjstring_string(word));
            column+=length+1;
        }
    }/*print_words*/


    static bjarray_t* make_array_of_string(const char** argv)
    {
        bjarray_t* result;
        unsigned int count=0;
        unsigned int i;
        while(argv[count]!=0){
            count++;
        }
        result=bjarray_new(count);
        for(i=0;i<count;i++){
            bjarray_set_element_at(result,i,bjstring_new_string(argv[i]));
        }
        return(result);
    }/*make_array_of_string*/


    static void usage(const char* pname)
    {
        fprintf(
            stdout,
            "# %s usage:\n"\
            "   %s -from code -to code"\
            " [-a | -e [-ELR]] [-t] [-r] [-v] [-xC] < in > out \n"\
            "   %s -from code -to code"\
            " [-a | -e [-ELR]] [-t] [-r] [-v] [-xC] file... \n"\
            "   %s -n | -m [-a | -e [-ELR]] [-t] [-r] [-v] [-xC] < in > out \n"\
            "   %s -n | -m [-a | -e [-ELR]] [-t] [-r] [-v] [-xC] file... \n"\
            "   %s -A [-t] [-r] [-v] [-xC] < in > out \n"\
            "   %s -A [-t] [-r] [-v] [-xC] file... \n"\
            "   %s -T code \n",
            pname,pname,pname,pname,pname,pname,pname,pname);
        fprintf(
            stdout,
            "\t-t   = dumps the conversion table (no conversion),\n"\
            "\t-T   = dumps the code table (no conversion),\n"\
            "\t-a   = approximate (use it with -to ascii),\n"\
            "\t-n   = -from macintosh -to next\n"\
            "\t-m   = -from next -to macintosh\n"\
            "\t-e   = replace missing characters in 'to' set with their name,\n"
            );
        fprintf(
            stdout,
            "\t-xC  = replace missing characters in 'to' set with C,\n"\
            "\t-ELR = encasulate missing characters in 'to' set with L and R,\n"\
            "\t-v   = prints on stderr the list of missing characters,\n"\
            "\t-r   = the files are Rich Text Format files,\n"\
            "\t-A   = filters all remaining 8-ths bits.\n"\
            "# Implemented codes are: \n");
        print_words(stdout,make_array_of_string(BenEncodingTableNames()),
                    0,4,72);
        fprintf(stdout,"\n\n");
    }/*usage*/
    

    typedef const char*         ConversionTableT[256];
    static ConversionTableT     Conversion;
    static BOOLEAN              rtf;


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
        char            buffer[BufSize];
        int             rlength;
        int             wlength;
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
                            " %d != %d\n",wlength,rlength);
                }
            }
        }
        return(0);
    }/*Filter8thBit*/


    static const char* escapestr(const char* string)
    {
        static char buffer[1024];
        int i=0;
        int j=0;
        while(string[i]!=0){
            if((string[i]<32)||(126<string[i])){
                buffer[j++]='\\';
                sprintf(buffer+j,"%03o",(unsigned char)(string[i]));
                j+=3;
            }else if((string[i]=='\\')||(string[i]=='"')){
                buffer[j++]='\\';
                buffer[j++]=string[i];
            }else{
                buffer[j++]=string[i];
            }
            i++;
        }
        buffer[j]='\0';
        return(buffer);
    }/*escapestr;*/
    
    

    static BenEncodingTableT* safe_get_enctable(const char* tab_name)
    {
        BenEncodingTableT* table=BenEncodingTableNamed(tab_name);
        if(table==0){
            fprintf(stderr,"There is no table named '%s'.\n",tab_name);
            exit(1);
        }
        return(table);
    }/*safe_get_enctable*/



    int main(int argc,char** argv,char** envp)
    {
        const char*         pname=basename(argv[0]);
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
        BOOLEAN             dumpCode;
        BenModeT            mode;
        const char*         from_name="unnamed";
        const char*         to_name="unnamed";
        BenEncodingTableT*  from_table=0;
        BenEncodingTableT*  to_table=0;
        char                replacement[2]=" "; /* space is default replacement */
        int (*convert)(FILE*,FILE*);

        bjobject_initialize();
        rtf=FALSE;
        nooption=TRUE;
        fromIsDetermined=FALSE;
        toIsDetermined=FALSE;
        eight2seven=FALSE;
        dumpTable=FALSE;
        dumpCode=FALSE;
        mode=BenMode_Normal;
        i=1;
        while((i<argc)&&(argv[i][0]=='-')){
            STR_SWITCH(argv[i])
            STR_CASE("-n"){
                nooption=FALSE;
                from_table=safe_get_enctable(from_name="macintosh");
                to_table=safe_get_enctable(to_name="nextstep");
                fromIsDetermined=TRUE;
                toIsDetermined=TRUE;
            }
            STR_CASE("-m"){
                nooption=FALSE;
                from_table=safe_get_enctable(from_name="nextstep");
                to_table=safe_get_enctable(to_name="macintosh");
                fromIsDetermined=TRUE;
                toIsDetermined=TRUE;
            }
            STR_CASE("-t") dumpTable=TRUE;
            STR_CASE("-a") mode=BenMode_Approximation;
            STR_CASE("-A"){
                nooption=FALSE;
                eight2seven=TRUE;
            }
            STR_CASE("-T"){
                dumpCode=TRUE;
                i++;
                if(i<argc){
                    from_table=BenEncodingTableNamed(from_name=argv[i]);
                    if(from_table==0){
                        fprintf(stderr,"### Invalid from table name: '%s'.\n",
                                argv[i]);
                        usage(pname);
                        return(1);
                    }
                    fromIsDetermined=TRUE;               
                }else{
                    fprintf(stderr,
                            "### A code name is expected following -T option.\n");
                    usage(pname);
                    return(1);
                }}
            STR_CASE2("-f","--from"){
                nooption=FALSE;
                i++;
                if(i<argc){
                    from_table=BenEncodingTableNamed(from_name=argv[i]);
                    if(from_table==0){
                        fprintf(stderr,"### Invalid from table: '%s'.\n",argv[i]);
                        usage(pname);
                        return(1);
                    }
                    fromIsDetermined=TRUE;
                }else{
                    fprintf(stderr,
                            "### A code name is expected after -from option.\n");
                    usage(pname);
                    return(1);
                }}
            STR_CASE2("-t","--to"){
                nooption=FALSE;
                i++;
                if(i<argc){
                    to_table=BenEncodingTableNamed(to_name=argv[i]);
                    if(to_table==0){
                        fprintf(stderr,"### Invalid to table: '%s'.\n",argv[i]);
                        usage(pname);
                        return(1);
                    }
                    toIsDetermined=TRUE;
                }else{
                    fprintf(stderr,
                            "### A code name is expected following -to option.\n");
                    usage(pname);
                    return(1);
                }}
            STR_CASE("-r")  rtf=TRUE;
            STR_DEFAULT{
                if(argv[i][1]=='x'){
                    replacement[0]=argv[i][2];
                }else{
                    fprintf(stderr,"### Bad option: <%s>.\n",argv[i]);
                    usage(pname);
                    return(1);
                }}
            STR_END
            i++;
        }/*while*/


        if(dumpCode){
            int ii;
            int codeSize=256-1;
            while((codeSize>=0)&&((*from_table)[codeSize]==NULL)){
                codeSize--;
            };
            /* codeSize==-1 || from_table[codeSize]!=NULL.*/
            codeSize++;
            /* from_table[codeSize]==NULL, codeSize==0||from_table[codeSize-1]!=NULL*/
            /* 2^(k-1)<codeSize<=2^k       // 1<<ii = 2^ii*/
            ii=1;
            while((ii<8)&&((1<<ii)<codeSize)){
                ii++;
            }
            codeSize=(1<<ii);

            to_table=BenEncodingTableNamed("ascii");
            BenBuildConversionTable(
                mode,from_table,to_table,
                BenApproximationTableNamed(BenApproximationNames()[0]),
                replacement,&Conversion);
            convert=ConvertEncoding;

            fprintf(stdout,"+-----+-----+------+-----------"
                    "-----------------------+\n");
            fprintf(stdout,"| DEC | HEX | CHR  | %-32s |\n","NAME");
            fprintf(stdout,"+-----+-----+------+-----------"
                    "-----------------------+\n");
            for(ii=0;ii<codeSize;ii++){
                fprintf(stdout,"| %3d |  %02x | %-4s | %-32s |\n",
                        ii,ii,escapestr(Conversion[ii]),
                        (*from_table)[ii]?(*from_table)[ii]:"");
            }
            fprintf(stdout,"+-----+-----+------+-----------"
                    "-----------------------+\n");
            return(0);
        }

        if(nooption){
            fprintf(stderr,"### Either -A or -from and -to is mandatory.\n");
            usage(pname);
            return(1);
        }

        if(eight2seven){
            if(fromIsDetermined||toIsDetermined||(mode!=BenMode_Normal)){
                fprintf(stderr,
                        "### -A option is incompatible with -from, -to, -a & -e\n");
                usage(pname);
                return(1);
            }
            convert=Filter8thBit;
        }else if(fromIsDetermined&&toIsDetermined){
            BenBuildConversionTable(
                mode,from_table,to_table,
                BenApproximationTableNamed(BenApproximationNames()[0]),
                replacement,&Conversion);
            convert=ConvertEncoding;
        }else{
            fprintf(stderr,"### Both -from and -to are mandatory.\n");
            usage(pname);
            return(1);
        }

        if(dumpTable){
            int ii;
            fprintf(stdout,"static const char* Conversion_%s_to_%s[256]={\n",
                    from_name,to_name);
            for(ii=0;ii<256;ii++){
                fprintf(stdout,"        /* %3d */ \"%s\",\n",
                        ii,escapestr(Conversion[ii]));
            }
            fprintf(stdout,"    };\n");
            return(0);
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
                                        chown(argv[i],filestatus.st_uid,
                                              filestatus.st_gid);
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



/*** encoding.c                       --                     --          ***/

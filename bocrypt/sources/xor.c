/******************************************************************************
FILE:               xor.c
LANGUAGE:           ANSI-C
SYSTEM:             POSIX
USER-INTERFACE:     POSIX
DESCRIPTION
    This is a simple tool that implement an xor of two binary files.
    One file is considered a key, and if shorter than the 'message' file,
    then it's used repetitively.
USAGE
    xor   -k|--key KEY_FILE \
        [ -m|--message MESSAGE_FILE | < MESSAGE_FILE ] \
        [ -o|--output  OUTPUT_FILE  | > OUTPUT_FILE ] 
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    2002-04-02 <PJB> Creation.
BUGS
LEGAL
    GPL
    Copyright Pascal J. Bourguignon 2002 - 2011

    This file is part of xor tool.

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
#include <stdio.h>
#include <string.h>
#include <sysexits.h>
#ifdef MACOSX
    static const char* basename(const char* path)
    {
        const char* slash=strrchr(path,'/');
        if(slash==0){
            return(path);
        }else{
            return(slash+1); /* c'est faux, mais tant pis ! */
        }
    }/*basename*/
#else
#include <libgen.h>
#endif

    typedef enum { no, yes } bool;

    const char* pname="xor";

    static int xor_files(FILE* keyf,FILE* inpf,FILE* outf)
    {
#define max_buf (65536)
        static unsigned char  key_buf[max_buf];
        static unsigned char  dat_buf[max_buf];
        int         key_len=0;
        int         key_idx=0;
        int         dat_len=0;
        int         dat_idx=0;
        int         dat_max=0;
        int         err_cnt=0;
        int         out_idx;
        int         out_len;
        bool        big_key=no;
        bool        first_time=yes;

        clearerr(keyf);
        clearerr(inpf);
        clearerr(outf);


        while(!feof(inpf)){

            dat_len=(int)fread(dat_buf,sizeof(unsigned char),sizeof(dat_buf),inpf);
            if(ferror(inpf)){
                fprintf(stderr,
                        "%s: error while reading input file. ABORTING.\n",
                        pname);
                return(EX_IOERR);
            }
            dat_idx=0;

            while(dat_idx<dat_len){
                if(key_idx==key_len){
                    if(big_key||first_time){
                        first_time=no;
                        if(feof(keyf)){
                            clearerr(keyf);
                            rewind(keyf);
                        }
                        key_len=(int)fread(key_buf,
                                           sizeof(unsigned char),
                                           sizeof(key_buf),
                                           keyf);
                        if(ferror(keyf)){
                            fprintf(stderr,
                                    "%s: error while reading key file. "\
                                    "ABORTING.\n",
                                    pname);
                            return(EX_IOERR);
                        }
                        if(key_len==0){
                            fprintf(stderr,
                                    "%s: error key file too small (0 bytes). "\
                                    "ABORTING.\n",
                                    pname);
                            return(EX_DATAERR);
                        }
                        if(key_len==sizeof(key_buf)){
                            big_key=yes; /* Hence we may read again key file */
                        }
                    }
                    key_idx=0;
                }
                dat_max=dat_idx+key_len-key_idx;
                if(dat_len<dat_max){
                    dat_max=dat_len;
                }

                while(dat_idx<dat_max){
                    dat_buf[dat_idx++]^=key_buf[key_idx++];
                }
            }
            
            err_cnt=0;
            out_idx=0;
            do{
                clearerr(outf);
                out_len=(int)fwrite(dat_buf+out_idx,
                                    sizeof(unsigned char),
                                    (size_t)(dat_len-out_idx),
                                    outf);
                out_idx+=out_len;
                if(ferror(outf)){
                    err_cnt++;
                }
            }while((err_cnt<3)&&(out_idx<dat_len));

            if(out_idx!=dat_len){
                fprintf(stderr,
                        "%s: error while writting output file. "\
                        "ABORTING.\n",
                        pname);
                return(EX_IOERR);
            }
        }
        return(EX_OK);
    }/*xor_files*/


    static void usage(const char* upname)
    {
        int len=(int)strlen(upname);
        fprintf(stderr,
                "%s usage:\n"\
                "    %s   -k|--key KEY_FILE \\\n"\
                "    %*s [ -m|--message MESSAGE_FILE | < MESSAGE_FILE ] \\\n"\
                "    %*s [ -o|--output  OUTPUT_FILE  | > OUTPUT_FILE ] \n",
                upname,upname,len,"",len,"");
    }/*usage*/

                
int main(int argc,char** argv)
{
    char* keyn=0;
    char* inpn=0;
    char* outn=0;
    FILE* keyf;
    FILE* inpf;
    FILE* outf;
    int i;
    pname=basename(argv[0]);
    for(i=1;i<argc;i++){
        if((strcmp(argv[i],"-k")==0)||(strcmp(argv[i],"--key")==0)){
            if(keyn!=0){
                fprintf(stderr,"%s: too many key files.\n",pname);
                usage(pname);
                return(EX_USAGE);
            }
            if(i==argc){
                fprintf(stderr,"%s: missing argument after %s.\n",
                        pname,argv[i]);
                usage(pname);
                return(EX_USAGE);                
            }
            i++;
            keyn=argv[i];
        }else if((strcmp(argv[i],"-m")==0)||(strcmp(argv[i],"--message")==0)){
            if(inpn!=0){
                fprintf(stderr,"%s: too many message files.\n",pname);
                usage(pname);
                return(EX_USAGE);
            }
            if(i==argc){
                fprintf(stderr,"%s: missing argument after %s.\n",
                        pname,argv[i]);
                usage(pname);
                return(EX_USAGE);                
            }
            i++;
            inpn=argv[i];
        }else if((strcmp(argv[i],"-o")==0)||(strcmp(argv[i],"--output")==0)){
            if(outn!=0){
                fprintf(stderr,"%s: too many output files.\n",pname);
                usage(pname);
                return(EX_USAGE);
            }
            if(i==argc){
                fprintf(stderr,"%s: missing argument after %s.\n",
                        pname,argv[i]);
                usage(pname);
                return(EX_USAGE);                
            }
            i++;
            outn=argv[i];
        }else if((strcmp(argv[i],"-h")==0)||(strcmp(argv[i],"--help")==0)){
            usage(pname);
            return(EX_OK);
        }
    }
    if(keyn==0){
        fprintf(stderr,"%s: missing key file.\n",pname);
        usage(pname);
        return(EX_USAGE);
    }

    keyf=fopen(keyn,"r");
    if(keyf==0){
        fprintf(stderr,"%s: can't open '%s' for reading.\n",pname,keyn);
        return(EX_NOINPUT);
    }

    if(inpn==0){
        inpf=stdin;
    }else{
        inpf=fopen(inpn,"r");
        if(inpf==0){
            fprintf(stderr,"%s: can't open '%s' for reading.\n",pname,inpn);
            return(EX_NOINPUT);
        }
    }

    if(outn==0){
        outf=stdout;
    }else{
        outf=fopen(outn,"w");
        if(outf==0){
            fprintf(stderr,"%s: can't open '%s' for writting.\n",pname,outn);
            return(EX_CANTCREAT);
        }
    }

    return(xor_files(keyf,inpf,outf));
}/*main*/



/*** xor.c                            -- 2003-12-02 10:35:24 -- pascal   ***/

/******************************************************************************
FILE:               lgetvalue.c
LANGUAGE:           ANSI-C
SYSTEM:             POSIX
USER-INTERFACE:     POSIX
DESCRIPTION

    Takes a file of key:value pairs, and a file of keys, and vfile a
    file of values.

    The two input files must be sorted on the keys.

    Addition arguments may specify the field separator and the key
    field position in the kv file.

    The default separator is '|', and  the default key field position
    is 1 (starting from 1), and  the default value field position is 2.

    A null value field position may be given, in which case the whole
    key-value  line is output.

AUTHORS
    <PJB> Pascal J. Bourguignon <pjb@informatimago.com>
MODIFICATIONS
    2000-09-16 <PJB> Creation.
BUGS
    Please report them to <pjb@informatimago.com>
LEGAL
    Copyright Pascal J. Bourguignon 2000 - 2000

    GPL
    This file is part of the lgetvalue utility.

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

    Commercial licenses are available. 
    Send your requests to <pjb@informatimago.com>.
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "lines.h"
#include "fields.h"
#include "arguments.h"

#define BUFFER_SIZE (1024)

#undef DEBUG

    static int get_values(FILE* kvfile,FILE* kfile,FILE* vfile,
                          int/*char*/ field_separator,
                          unsigned int key_field,unsigned int value_field)
        /*
            PRE:        The lines in kvfile is sorted on 
                        the key field, and kfile is sorted.
                        key_field>0.

            DO:         Write on the vfile file the lines from kvfile 
                        whose key is not found on a line from kfile.
                                        are matched.
        */
    {
        char buffer1[BUFFER_SIZE];
        char buffer2[BUFFER_SIZE];
        char key[BUFFER_SIZE];
        char value[BUFFER_SIZE];
        short success;

#ifdef DEBUG
        fprintf(stderr,"FS=\"%c\"; KF=%d; VF=%d;\n",
                field_separator,key_field,value_field);
#endif
        success=(NULL!=fgets(buffer1,BUFFER_SIZE,kvfile));
        success&=(NULL!=fgets(buffer2,BUFFER_SIZE,kfile));
        if(success){
            chop_nl(buffer1);
            chop_nl(buffer2);
            get_field(key,buffer1,field_separator,key_field);
        }
        while(success){
            int order;
            order=strcmp(key,buffer2);
            /*
              We process only one line either from kvfile or from kfile in
              each iteration, to allow for duplicate lines in either files.
              Everytime, either key or buffer2 will have changed.
             */
            if(0<order){
                 /* buffer2 is not (or no more) in kvfile: let's skip buffer2 */
#ifdef DEBUG
                fprintf(stderr,"Key done    '%s'\n",buffer2);
#endif
                success=(NULL!=fgets(buffer2,BUFFER_SIZE,kfile));
                if(success){
                    chop_nl(buffer2);
                }
            }else{/* order<=0 */
                if(order==0){
                    /* key1 is in kfile: write out buffer1 */
                    get_field(value,buffer1,field_separator,value_field);
#ifdef DEBUG
                    fprintf(stderr,"Writting KV '%s' -> '%s'\n",key,value);
#endif
                    write_string(value,vfile);
                    fprintf(vfile,"\n");
#ifdef DEBUG
                }else{
                    fprintf(stderr,"Skipping K  '%s'\n",key);
#endif
                }/* else key1 is not in kfile: let's skip buffer1 */
                success=(NULL!=fgets(buffer1,BUFFER_SIZE,kvfile));
                if(success){
                    chop_nl(buffer1);
                    get_field(key,buffer1,field_separator,key_field);
                }
            }
        }
        /* no more kv or k.*/

        return(0);
    }/*get_values*/


    void usage(const char* pname)
    {
        int plen=strlen(pname);
        fprintf(stderr,"%s usage:\n",pname);
        fprintf(stderr,
                "  %s [-h|--help|--field-separator=C|-tC\n"\
                "  %*s |--key-position=N|--value-position=N]\n"\
                "  %*s kv-file k-file > v-file\n"\
                "\n"\
                " The default separator is '|', the default key position is 1,"\
                " and the default value position is 2 (counting from 1).\n",
                pname,plen," ",plen," ");
    }/*usage*/



int main(int argc,char** argv)
{
    const char*     pname=argv[0];
    const char*     kvname=0;
    const char*     kname=0;
    char            field_separator='|';
    int             key_position=1;
    int             value_position=2;
    FILE*           kvfile;
    FILE*           kfile;
    int             status=0;
    int             i;

    if(argc==1){
        usage(pname);
        return(9);
    }
    for(i=1;i<argc;i++){
        if(strncmp(argv[i],"--field-separator=",18)==0){
            status=get_separator(&field_separator,argv[i],18,pname);
            if(status!=0){
                return(status);
            }
        }else if(strncmp(argv[i],"-t",2)==0){
            status=get_separator(&field_separator,argv[i],2,pname);
            if(status!=0){
                return(status);
            }
        }else if(strncmp(argv[i],"--key-position=",15)==0){
            status=get_integer(&key_position,argv[i]+15,pname,2);
            if(status!=0){
                return(status);
            }
        }else if(strncmp(argv[i],"--value-position=",17)==0){
            status=get_integer(&value_position,argv[i]+17,pname,1);
            if(status!=0){
                return(status);
            }
        }else if((strcmp(argv[i],"--help")==0)
                 ||(strcmp(argv[i],"-h")==0)){
            usage(pname);
            return(0);
        }else if(argv[i][0]=='-'){
            fprintf(stderr,"%s error: Invalid option '%s'.\n",
                    pname,argv[i]);
            usage(pname);
            return(8);        
        }else if(kvname==0){
            kvname=argv[i];
        }else if(kname==0){
            kname=argv[i];
        }else{
            fprintf(stderr,"%s error: Only two file name are accepted.\n"
                    "  I already have: '%s', \n"
                    "             and: '%s'; \n"
                    "                  '%s' is too many.\n",
                    pname,kvname,kname,argv[i]);
            usage(pname);
            return(3);
        }
    }

    kvfile=fopen(kvname,"r");
    kfile=fopen(kname,"r");
    if(kvfile==NULL){
        fprintf(stderr,"%s: cannot open file '%s'.\n",pname,kvname);
        status|=1;
    }
    if(kfile==NULL){
        fprintf(stderr,"%s: cannot open file '%s'.\n",pname,kname);
        status|=2;
    }

    if((kvfile!=NULL)&&(kfile!=NULL)){
        status=get_values(kvfile,kfile,stdout,field_separator,
                          (unsigned int)key_position,
                          (unsigned int)value_position);
    }
    
    return(status);
}/*main*/


/*** lgetvalue.c                      --                     --          ***/

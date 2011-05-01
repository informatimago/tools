/*****************************************************************************
FILE:               fdiff.c
LANGUAGE:           c
SYSTEM:             POSIX
USER-INTERFACE:     NONE
DESCRIPTION
    
    Outputs the field-differences between two files, that is
    the two input files must be sorted on the same field.

    This tools compare these given fields and outputs the line from the
    first file containing a such field that does not appear in the
    second file.

    The default separator is '|', and the default field position is 1.
    
AUTHORS
    <PJB> Pascal Bourguignon <pjb@informatimago.com>
MODIFICATIONS
    2003-12-03 <PJB> Added this header.
BUGS
LEGAL
    GPL
    
    Copyright Pascal Bourguignon 2003 - 2011
    
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
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "lines.h"
#include "fields.h"
#include "arguments.h"

#define BUFFER_SIZE (1024)


    static int difference(FILE* file1,FILE* file2,FILE* output,
                          int/*char*/ field_separator,unsigned int key_field)
        /*
            PRE:        The lines in file1 and file2 are sorted on 
                        the key field.

            DO:         Write on the output file the lines from file1 
                        whose key is not found on a line from file2.
                        key_field=0 ==> the whole lines are matched.
                        key_field>0 ==> only the fields numbered key_field
                                        are matched.
        */
    {
        char buffer1[BUFFER_SIZE];
        char buffer2[BUFFER_SIZE];
        char key1[BUFFER_SIZE];
        char key2[BUFFER_SIZE];
        int success;

        success=(NULL!=fgets(buffer1,BUFFER_SIZE,file1));
        success&=(NULL!=fgets(buffer2,BUFFER_SIZE,file2));
        get_field(key1,buffer1,field_separator,key_field);
        get_field(key2,buffer2,field_separator,key_field);
        while(success){
            int order;
            order=strcmp(key1,key2);
            /*
              We process only one line either from file1 or from file2 in
              each iteration, to allow for duplicate lines in either files.
              Everytime, either key1 or key2 will have changed.
             */
            if(0<order){
                 /* key2 is not (or no more) in file1: let's skip buffer2 */
                success=(NULL!=fgets(buffer2,BUFFER_SIZE,file2));
                if(success){
                    get_field(key2,buffer2,field_separator,key_field);
                }
            }else{/* order<=0 */
                if(order<0){
                    /* key1 is not in file2: write out buffer1 */
                    write_string(buffer1,output);
                }/* else key1 is in file2: let's skip buffer1 */
                success=(NULL!=fgets(buffer1,BUFFER_SIZE,file1));
                if(success){
                    get_field(key1,buffer1,field_separator,key_field);
                }
            }
        }

        /* Write out the remaining lines from file1 to output. */
        while(!feof(file1)){
            if(NULL!=fgets(buffer1,BUFFER_SIZE,file1)){ 
                write_string(buffer1,output);
            }
        }

        return(0);
    }/*difference*/



    void usage(const char* pname)
    {
        fprintf(stderr,"%s usage:\n",pname);
        fprintf(stderr,
                "  %s [-h|--help|--field-separator=C|-tC"
                "|--field-position=N|+N]\n"
                "  %*s file1 file2 > difference\n",
                pname,(int)(strlen(pname))," ");
    }/*usage*/



int main(int argc,char** argv)
{
    const char*     pname=argv[0];
    const char*     name1=0;
    const char*     name2=0;
    char            field_separator='|';
    int             field_position=1;
    FILE*           file1;
    FILE*           file2;
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
        }else if(strncmp(argv[i],"--field-position=",13)==0){
            status=get_integer(&field_position,argv[i]+13,pname,1);
            if(status!=0){
                return(status);
            }
        }else if(strncmp(argv[i],"+",1)==0){
            status=get_integer(&field_position,argv[i]+1,pname,1);
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
        }else if(name1==0){
            name1=argv[i];
        }else if(name2==0){
            name2=argv[i];
        }else{
            fprintf(stderr,"%s error: Only two file name are accepted.\n"
                    "  I already have: '%s', \n"
                    "             and: '%s'; \n"
                    "                  '%s' is too many.\n",
                    pname,name1,name2,argv[i]);
            usage(pname);
            return(3);
        }
    }

    file1=fopen(name1,"r");
    file2=fopen(name2,"r");
    if(file1==NULL){
        fprintf(stderr,"%s: cannot open file '%s'.\n",pname,name1);
        status|=1;
    }
    if(file2==NULL){
        fprintf(stderr,"%s: cannot open file '%s'.\n",pname,name2);
        status|=2;
    }

    if((file1!=NULL)&&(file2!=NULL)){
        status=difference(file1,file2,stdout,field_separator,
                          (unsigned int)field_position);
    }
    
    return(status);
}/*main*/

/*** fdiff.c                          --                     --          ***/

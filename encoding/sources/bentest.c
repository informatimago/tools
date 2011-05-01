/*****************************************************************************
FILE:               bentest.c
LANGUAGE:           c
SYSTEM:             POSIX
USER-INTERFACE:     NONE
DESCRIPTION
    
    Testing Bencoding library.
    
AUTHORS
    <PJB> Pascal Bourguignon <pjb@informatimago.com>
MODIFICATIONS
    2003-12-03 <PJB> Created.
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
/* GNU_SOURCE */
#include <libgen.h>    
/* LINUX */
#include <sysexits.h>
/* PJB */
#include <Bencoding.h>
#include <bjstring_list.h>
#include <BcImplementation.h>

#define STR_SWITCH(str)      {const char* str_switch_string=(str);if(0){
#define STR_CASE(str)        }else if(0==strcmp(str_switch_string,(str))){
#define STR_CASE2(str1,str2) }else if((0==strcmp(str_switch_string,(str1)))\
                                    ||(0==strcmp(str_switch_string,(str2)))){
#define STR_DEFAULT          }else{
#define STR_END              }}


    static void print_usage(FILE* out,const char* pname)
    {
        fprintf(out,"%s usage:\n\t%s [-h|--help\n"\
                "\t\t|-t|--tables|-a|--approximations\n"\
                "\t\t|-b|--bjstring_split]\n",
                pname,pname);
    }/*print_usage*/


    static void string_array_print(FILE* out,const char* const* array)
    {
        unsigned int i;
        for(i=0;array[i];i++){
            fprintf(out,"%s\n",array[i]);
        }
    }/*string_array_print*/


int main(int argc,char** argv)
{
    unsigned int i;
    const char* pname=basename(argv[0]);
    bjobject_initialize();
    for(i=1;i<(unsigned)argc;i++){
        STR_SWITCH(argv[i])
        STR_CASE2("-h","--help"){
            print_usage(stdout,pname);
            return(0);
        }
        STR_CASE2("-a","--approximations"){
            string_array_print(stdout,BenApproximationNames());
        }
        STR_CASE2("-t","--tables"){
            string_array_print(stdout,BenEncodingTableNames());
        }
        STR_CASE2("-b","--bjstring_split"){
            bjstring_test_split_unsplit();
        }
        STR_DEFAULT{
            fprintf(stderr,"%s: Invalid argument '%s'.\n",pname,argv[i]);
            print_usage(stderr,pname);
            return(EX_USAGE);
        }
        STR_END
    }
    return(0);
}

/*** bentest.c                        --                     --          ***/

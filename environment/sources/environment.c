/******************************************************************************
FILE:               environment.c
LANGUAGE:           ANSI-C
SYSTEM:             POSIX
USER-INTERFACE:     POSIX
DESCRIPTION

    Generate sh or csh environment variable definitions from a common file.
USAGE
    sh  'environment -sh  < ~/.environment > env.sh  ; . env.sh'
    csh 'environment -csh < ~/.environment > env.csh ; source env.csh'
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    19??-??-?? <PJB> Creation.
BUGS
LEGAL
    GPL
    Copyright Pascal J. Bourguignon 19?? - 2002

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
#include <malloc.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

    const char* pname="environment"; /*default name*/

    static void report_error(const char* message)
    {
        fprintf(stderr,"%s: %s\n",pname,message);
    }/*report_error*/




    typedef void (*print_environment_pr)(const char* name,const char* value);
    typedef void (*print_environment_header_pr)(void);

    print_environment_pr        print_environment=(print_environment_pr)0;
    print_environment_header_pr print_environment_header
                                          =(print_environment_header_pr)0;


    static void print_environment_header_csh(void)
    {
        printf("#!/bin/csh\n");
    }/*print_environment_header_csh*/


    static void print_environment_csh(const char* name,const char* value)
    {
        if(value[0]=='\0'){
            printf("unsetenv %s\n",name);
        }else{
            printf("setenv %s %s\n",name,value);
        }
    }/*print_environment_csh*/


    static void print_environment_header_sh(void)
    {
        printf("#!/bin/sh\n");
    }/*print_environment_header_sh*/


    static void print_environment_sh(const char* name,const char* value)
    {
        if(value[0]=='\0'){
            printf("unset %s\n",name);
        }else{
            printf("export %s=%s\n",name,value);
        }
    }/*print_environment_sh*/




    static char* string_new(const char* string)
    {
        char* that=(char*)malloc(sizeof(char)*(strlen(string)+1));
        strcpy(that,string);
        return(that);
    }/*string_new*/



    typedef struct environment_list_s {
        struct environment_list_s*    next;
        const char*                   name;
        const char*                   value;
    }            environment_list_t;



    static environment_list_t* environment_list_new(const char* name,
                                             const char* value)
    {
        environment_list_t* that;
        that=(environment_list_t*)malloc(sizeof(environment_list_t));
        that->next=(environment_list_t*)0;
        that->name=string_new(name);
        that->value=string_new(value);
        return(that);
    }/*environment_list_new*/


    static void environment_list_append(environment_list_t* that,
                                 environment_list_t* tail)
    {
        while(that->next!=(environment_list_t*)0){
            that=that->next;
        }
        that->next=tail;
    }/*environment_list_append*/


    static void environment_list_print(environment_list_t* that,
                                print_environment_pr print_env)
    {
        while(that!=(environment_list_t*)0){
            print_env(that->name,that->value);
            that=that->next;
        }
    }/*environment_list_print*/



    typedef FILE environment_file_t;
#if 0
    static environment_file_t* environment_file_open(const char* filename)
    {
        struct passwd*      pentry=getpwuid(getuid());
        if(pentry==(struct passwd*)0){
            report_error("cannot get home directory.");
            return((environment_file_t*)0);
        }else{
            char filepath[1024];
            if(strlen(pentry->pw_dir)+strlen(filename)+2>sizeof(filepath)){
                report_error("Path to the environment file is too long.");
                return((environment_file_t*)0);
            }
            sprintf(filepath,"%s/%s",pentry->pw_dir,filename);
            return(fopen(filepath,"r"));
        }
    }/*environment_file_open*/
#endif

    static environment_list_t* environment_file_load(environment_file_t* file)
    {
        environment_list_t* list=(environment_list_t*)0;
        char line[1024];
        int  linum=1;
        while((char*)0!=fgets(line,sizeof(line)-1,file)){
            char name[1024];
            char value[1024];
            environment_list_t* current=(environment_list_t*)0;
            int i=0;
            int j=0;
            enum {before_name,in_name,before_value,in_value_single_quote,
                  in_value_double_quote,in_value_back_quote,
                  in_value_unquoted,between_value,after_value} state=before_name;
            short escaped=0;
            char  quote=' ';

            i=(int)strlen(line)-1;
            if(line[i]=='\n'){
                line[i]='\0';
            }

            name[0]='\0';
            value[0]='\0';
            i=0;
            while((state!=after_value)&&(line[i]!='\0')){
                switch(state){
                case before_name:
                    if(isalpha(line[i])||(line[i]=='_')){
                        state=in_name;
                        j=0;
                        name[j++]=line[i];
                        name[j]='\0';
                    }else if(/*GNU extension*/isblank(line[i])){
                        /* Let's skip it */
                    }else if(line[i]=='#'){
                        state=after_value;
                    }else{
                        char message[1024];
                        sprintf(message,"Invalid character '%c' (0x%02x) "
                                "at line %d",line[i],line[i],linum);
                        report_error(message);
                        exit(1);
                    }
                    break;
                case in_name:
                    if(isalnum(line[i])||(line[i]=='_')){
                        name[j++]=line[i];
                        name[j]='\0';
                    }else if(/*GNU extension*/isblank(line[i])){
                        state=before_value;
                        j=0;
                    }else if(line[i]=='#'){
                        state=after_value;
                    }else{
                        char message[1024];
                        sprintf(message,"Invalid character '%c' (0x%02x) "
                                "at line %d",line[i],line[i],linum);
                        report_error(message);
                        exit(1);
                    }
                    break;
                case before_value:
                    /* fall thru */
                case between_value:
                    if(line[i]=='\''){
                        state=in_value_single_quote;
                        value[j++]=line[i];
                        value[j]='\0';
                        quote=line[i];
                    }else if(line[i]=='"'){
                        state=in_value_double_quote;
                        value[j++]=line[i];
                        value[j]='\0';
                        quote=line[i];
                    }else if(line[i]=='`'){
                        state=in_value_back_quote;
                        value[j++]=line[i];
                        value[j]='\0';
                        quote=line[i];
                    }else if(line[i]=='\\'){
                        state=in_value_unquoted;
                        value[j++]=line[i];
                        value[j]='\0';
                        escaped=1;
                    }else if(/*GNU extension*/isblank(line[i])){
                        if(state==between_value){
                            state=after_value;
                        }else{
                            /* Let's skip it */
                        }
                    }else if(line[i]=='#'){
                        state=after_value;
                    }else{
                        state=in_value_unquoted;
                        value[j++]=line[i];
                        value[j]='\0';
                    }
                    break;
                case in_value_single_quote:
                    if(escaped){
                        value[j++]=line[i];
                        value[j]='\0';
                        escaped=0;
                    }else{
                        if(line[i]=='\''){
                            value[j++]=line[i];
                            value[j]='\0';
                            state=between_value;
                        }else if(line[i]=='\\'){
                            value[j++]=line[i];
                            value[j]='\0';
                            escaped=1;
                        }else{
                            value[j++]=line[i];
                            value[j]='\0';
                        }
                    }
                    break;
                case in_value_double_quote:
                     if(escaped){
                        value[j++]=line[i];
                        value[j]='\0';
                        escaped=0;
                    }else{
                        if(line[i]=='"'){
                            value[j++]=line[i];
                            value[j]='\0';
                            state=between_value;
                        }else if(line[i]=='\\'){
                            value[j++]=line[i];
                            value[j]='\0';
                            escaped=1;
                        }else{
                            value[j++]=line[i];
                            value[j]='\0';
                        }
                    }
                    break;
                case in_value_back_quote:
                    if(escaped){
                        value[j++]=line[i];
                        value[j]='\0';
                        escaped=0;
                    }else{
                        if(line[i]=='`'){
                            value[j++]=line[i];
                            value[j]='\0';
                            state=between_value;
                        }else if(line[i]=='\\'){
                            value[j++]=line[i];
                            value[j]='\0';
                            escaped=1;
                        }else{
                            value[j++]=line[i];
                            value[j]='\0';
                        }
                    }
                    break;
                case in_value_unquoted:
                     if(escaped){
                        value[j++]=line[i];
                        value[j]='\0';
                        escaped=0;
                    }else{
                        if(line[i]=='\\'){
                            value[j++]=line[i];
                            value[j]='\0';
                            escaped=1;
                        }else if(line[i]=='\''){
                            value[j++]=line[i];
                            value[j]='\0';
                            state=in_value_single_quote;
                        }else if(line[i]=='"'){
                            value[j++]=line[i];
                            value[j]='\0';
                            state=in_value_double_quote;
                        }else if(line[i]=='`'){
                            value[j++]=line[i];
                            value[j]='\0';
                            state=in_value_back_quote;
                        }else if((/*GNU extension*/isblank(line[i]))
                                 ||(line[i]=='#')){
                            state=after_value;
                        }else{
                            value[j++]=line[i];
                            value[j]='\0';
                        }
                    }
                    break;
               default:
                    report_error("Internal error: invalid state.");
                    exit(1);
                }
                i++;
            }/*while line[i]*/
            switch(state){
            case before_name:
                break;
            case in_name:
                /* fall thru */
            case before_value:
                /* fall thru */
            case in_value_unquoted:
                /* fall thru */
            case between_value:
                /* fall thru */
            case after_value:
                if(name[0]!='\0'){
                    current=environment_list_new(name,value);
                    if(list==(environment_list_t*)0){
                        list=current;
                    }else{
                        environment_list_append(list,current);
                    }
                }
                break;
            case in_value_single_quote:
            case in_value_double_quote:
            case in_value_back_quote:
                {
                    char message[1024];
                    sprintf(message,"Missing closing quote (%c) at line %d.",
                            quote,linum);
                    report_error(message);
                    exit(1);
                }
                break;
            default:
                report_error("Internal error: invalid state.");
                exit(1);
            }
            linum++;
        }/*while fgets*/
        return(list);
    }/*environment_file_load*/


    static void environment_file_close(environment_file_t* file)
    {
        fclose(file);
    }/*environment_file_close*/


 



    static void usage(const char* upname)
    {
        fprintf(stderr,"%s usage:\n"
                "    %s --sh|-s|--csh|-c--help|-h \n",upname,upname);
    }/*usage*/


int main(int argc,char** argv)
{
    environment_file_t* envfile;
    environment_list_t* envlist;
    int                 i;

    pname=argv[0];
    i=1;
    while(i<argc){
        if((strcmp(argv[i],"--csh")==0)
           ||(strcmp(argv[i],"-c")==0)){
            print_environment=print_environment_csh;
            print_environment_header=print_environment_header_csh;
        }else if((strcmp(argv[i],"--sh")==0)
           ||(strcmp(argv[i],"-s")==0)){
            print_environment=print_environment_sh;
            print_environment_header=print_environment_header_sh;
        }else if((strcmp(argv[i],"--help")==0)
           ||(strcmp(argv[i],"-h")==0)){
            usage(pname);
            exit(1);
        }else{
            usage(pname);
            exit(1);
        }
        i++;
    }

    if(print_environment==(print_environment_pr)0){
        fprintf(stderr,"%s: Please select either --sh or --csh.\n",pname);
        usage(pname);
        exit(1);
    }

    /*envfile=environment_file_open(".environment");*/
    envfile=(environment_file_t*)stdin;
    envlist=environment_file_load(envfile);
    environment_file_close(envfile);
    print_environment_header();
    environment_list_print(envlist,print_environment);
    return(0);
}/*main*/

/*** environment.c                    -- 2003-12-02 11:28:07 -- pascal   ***/

/******************************************************************************
FILE:               makeaccessors.c
LANGUAGE:           ANSI-C
SYSTEM:             NEXTSTEP_3.3
USER-INTERFACE:     NEXTSTEP_3.3
DESCRIPTION
    Takes on its input lines containing: 
        type attribute;
    and outputs Objective-C declarations and implementations
    for accessor methods (writer and reader).
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1995-09-10 <PJB> Creation.
BUGS
LEGAL
    GPL
    
    Copyright Pascal Bourguignon 1995 - 2011
    
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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


    typedef struct attribute_s {
        struct attribute_s*     next;
        const char*             type;
        const char*             attribute;
    }                       attribute_t;

    typedef struct {
        attribute_t*            head;
        attribute_t*            tail;
    }                       attribute_list_t;
    

    static void attribute_list_initialize(attribute_list_t* list)
    {
        list->head=NULL;
        list->tail=NULL;
    }/*attribute_list_initialize*/
    
    
    static char* string_new(const char* string)
    {
            char*       newstring;
        newstring=(char*)malloc(strlen(string)+1);
        strcpy(newstring,string);
        return(newstring);
    }/*string_new*/
    
    
    static attribute_t* attribute_new(const char* attribute,const char* type)
    {
            attribute_t*        elem;
        elem=(attribute_t*)malloc(sizeof(attribute_t));
        elem->attribute=string_new(attribute);
        elem->type=string_new(type);
        return(elem);
    }/*attribute_new*/
    
    
    static void attribute_list_add(attribute_list_t* list,attribute_t* elem)
    {
        if(list->head==NULL){
            list->head=elem;
            list->tail=elem;
        }else{
            list->tail->next=elem;
            list->tail=elem;
        }
    }/*attribute_list_add*/
    

    
    static void attribute_generate_interface(const char* attribute,const char* type)
    {
        printf("    -(%s)%s;\n    -(void)set%c%s:(%s)n%c%s;\n",
            type,attribute,toupper(attribute[0]),attribute+1,
            type,toupper(attribute[0]),attribute+1);
    }/*attribute_generate_interface*/
    
    
    static void attribute_generate_implementation
                                (const char* attribute,const char* type)
    {
        printf("\n    -(%s)%s\n    {\n        return(%s);\n    }//%s;\n\n",
            type,attribute,attribute,attribute);
        printf(
        "\n    -(void)set%c%s:(%s)n%c%s\n    {\n        %s=n%c%s;\n    }//set%c%s:;\n\n",
            toupper(attribute[0]),attribute+1,
            type,toupper(attribute[0]),attribute+1,
            attribute,toupper(attribute[0]),attribute+1,
            toupper(attribute[0]),attribute+1
            );
    }/*attribute_generate_implementation*/
    
    
    static void attribute_list_generate(attribute_list_t* list)
    {
            attribute_t*        elem;
        
        elem=list->head;
        while(elem!=NULL){
            attribute_generate_interface(elem->attribute,elem->type);
            elem=elem->next;
        }
        elem=list->head;
        while(elem!=NULL){
            attribute_generate_implementation(elem->attribute,elem->type);
            elem=elem->next;
        }
        
    }/*attribute_list_generate*/
    
    
    static void attribute_parse(char* line,char** rAttribute,char** rType)
    {
            char*       last;
            char*       attribute=0;
            
        while((*line==' ')||(*line=='\t')){
            line++;
        }
        last=line;while(*last++!='\0');last--;
        
        while((last>line)&&((last[-1]==' ')||(last[-1]=='\t'))){
            last--;
        }
        if((last>line)&&(last[-1]==';')){
            last--;
            *last='\0';
        }
        while((last>line)&&((*last!=' ')&&(*last!='\t'))){
            last--;
        }
        if((last>line)&&((*last==' ')||(*last=='\t'))){
            attribute=last+1;
        }
        while((last>line)&&((last[-1]==' ')||(last[-1]=='\t'))){
            last--;
        }
        *last='\0';
        *rAttribute=attribute;
        *rType=line;
    }/*attribute_parse*/
    

int main(void)
{
        char                line[BUFSIZ];
        attribute_list_t    list;
        char*               attribute;
        char*               type;
        
    attribute_list_initialize(&list);
    while(fgets(line,sizeof(line)-1,stdin)!=0){
        {
            int len=(int)strlen(line);
            if((len>0)&&('\n'==line[len-1])){
                line[len-1]='\0';
            }
        }
        attribute_parse(line,&attribute,&type);
        if((attribute==NULL)||(type==NULL)||(*attribute=='\0')||(*type=='\0')){
            printf("    // Cannot parse '%s'.\n",line);
        }else{
            attribute_list_add(&list,attribute_new(attribute,type));
        }
    }
    attribute_list_generate(&list);
    return(0);
}/*main*/

/*** objc-make-accessors.c            -- 2003-12-02 12:05:43 -- pascal   ***/

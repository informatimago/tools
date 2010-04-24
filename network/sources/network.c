/******************************************************************************
FILE:               network.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This programs takes from its standard input an ordered network as a 
    list of adjacent lists in the form: node neighbor...
    Adjacent lists with the same node on different lines will be merged.
    Then it finds for each node its level, that is the maximum distance of
    this node from the initial nodes, and outputs a line for each node in
    the form:  level node neighbor...
    This can be used for example to find the layers to which belong modules 
    in modularized software, by feeding it the graph of dependencies.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1992-12-18 <PJB> Added this comment.
LEGAL
    GPL
    
    Copyright Pascal Bourguignon 1991 - 1992
    
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
#include <string.h>
#include <stdlib.h>


/********************/
/*** BOOLEAN type ***/
/********************/
    
    typedef int                 BOOLEAN;
#define FALSE                   ((BOOLEAN)(0==1))
#define TRUE                    ((BOOLEAN)(0==0))


/*****************/
/*** List type ***/
/*****************/

    typedef struct List_T {
        struct List_T*              next;
        void*                       data;
    }                           List_T;
    typedef struct List_T*  List_P;

    static List_P       List_ElementCache=NULL;
    static List_P       List_ElementPreviousCache=NULL;
    static List_P       List_ElementListCache=NULL;
    static BOOLEAN      List_ElementResultCache=FALSE;
    static void*        List_DataCache=NULL;
    static List_P       List_DataElementCache=NULL;
    static List_P       List_DataListCache=NULL;
    static BOOLEAN      List_DataResultCache=FALSE;
    
    static void List_ClearCaches(List_P list)
    {
        if(list==List_DataListCache){
            List_DataCache=NULL;
            List_DataElementCache=NULL;
            List_DataListCache=NULL;
            List_DataResultCache=FALSE;
        }
        if(list==List_ElementListCache){
            List_ElementCache=NULL;
            List_ElementPreviousCache=NULL;
            List_ElementListCache=NULL;
            List_ElementResultCache=FALSE;
        }
    }/*List_ClearCaches;*/
    
#if 0   
    static BOOLEAN List_ContainsElement(List_P list,List_P element)
    {
        if((list==List_ElementListCache)&&(element==List_ElementCache)){
            return(List_ElementResultCache);
        }
        List_ElementListCache=list;
        List_ElementCache=element;
        if(element==NULL){
            List_ElementResultCache=FALSE;
            return(FALSE);
        }
        List_ElementPreviousCache=NULL;
        while((list!=NULL)&&(element!=list)){
            List_ElementPreviousCache=list;
            list=list->next;
        }
        List_ElementResultCache=(list!=NULL);
        return(List_ElementResultCache);
    }/*List_ContainsElement;*/
    
    static void List_Add(List_P* list,List_P element)
    {
        if(!List_ContainsElement(*list,element)){
            List_ClearCaches(*list);
            element->next=(*list);
            (*list)=element;
        }
    }/*List_Add;*/
#endif

    static List_P List_FindData(List_P list,void* data)
    {
        if((list==List_DataListCache)&&(data==List_DataCache)){
            return(List_DataElementCache);
        }
        List_DataListCache=list;
        List_DataCache=data;
        while((list!=NULL)&&(list->data!=data)){
            list=list->next;
        }
        List_DataElementCache=list;
        List_DataResultCache=(list!=NULL);
        return(List_DataElementCache);
    }/*List_FindData;*/
    
    
    static BOOLEAN List_ContainsData(List_P list,void* data)
    {
        return(List_FindData(list,data)!=NULL);
    }/*List_ContainsElement;*/
    
    
    static void List_Include(List_P* list,List_P element)
    {
        if(!List_ContainsData(*list,element->data)){
            List_ClearCaches(*list);
            element->next=(*list);
            (*list)=element;
        }
    }/*List_Include;*/


#if 0
    static void List_Exclude(List_P* list,List_P element)
    {
            List_P      previous;
            List_P      current;
            
        if(((*list)==List_ElementListCache)&&(element==List_ElementCache)){
            if(List_ElementResultCache){
                if(List_ElementPreviousCache==NULL){
                    (*list)=element->next;
                }else{
                    List_ElementPreviousCache->next=element->next;
                }
                element->next=NULL;
            }
            return;
        }
        List_ClearCaches(*list);
        if(element==NULL){
            return;
        }
        previous=NULL;
        current=(*list);
        while((current!=NULL)&&(element!=current)){
            previous=current;
            current=previous->next;
        }
        if(current!=NULL){
            if(previous==NULL){
                (*list)=current->next;
            }else{
                previous->next=current->next;
            }
            current->next=NULL;
        }
    }/*List_Exclude;*/
#endif  

    static List_P List_New(void)
    {
            List_P      e;
        e=malloc(sizeof(List_T));
        e->next=NULL;
        e->data=NULL;
        return(e);
    }/*List_New;*/
    

    typedef BOOLEAN (*List_ActionPr)(List_P,void*);
    
    static List_P List_While(List_P list,List_ActionPr proc,void* param)
    {
            List_P          current;
        
        current=list;
        while((current!=NULL)&&(proc(current,param))){
            current=current->next;
        }
        if(current!=NULL){
            List_DataCache=current->data;
            List_DataElementCache=current;
            List_DataListCache=list;
            List_DataResultCache=TRUE;
        }
        return(current);
    }/*List_While;*/
    

/*******************/
/*** String type ***/
/*******************/


    typedef char*               String_P;
    
    static String_P String_New(char* string)
    {
        String_P s=malloc(strlen(string)+1);
        strcpy(s,string);
        return(s);
    }/*String_New;*/
    
    
/************************/
/*** String Pool type ***/
/************************/
    
    typedef List_P              StrPool_T;

    
    static StrPool_T StrPool_New(void)
    {
        return(NULL);
    }/*StrPool_New;*/
    
    static BOOLEAN StrPool_StringsDiffer(List_P element,void* param)
    {
        return(strcmp(element->data,param)!=0);
    }/*StrPool_StringsDiffer;*/
    
    static String_P StrPool_IncludeString(StrPool_T* pool,char* string)
    {
            List_P      new;
        
        new=List_While(*pool,StrPool_StringsDiffer,string);
        if(new==NULL){
            new=List_New();
            new->data=String_New(string);
            List_Include(pool,new);
        }
        return(new->data);
    }/*StrPool_IncludeString;*/
    
    
    
    
/********************/
/*** Network type ***/
/********************/
    
    typedef struct {
        List_P                      neighbors;
        void*                       data;
        int                         counter;
        BOOLEAN                     walked;
    }                           Network_NodeT;
    typedef Network_NodeT*      Network_NodeP;
    
    static Network_NodeP Network_NewNode(void)
    {
        Network_NodeP       node;
        node=malloc(sizeof(Network_NodeT));
        node->neighbors=NULL;
        node->data=NULL;
        node->counter=0;
        node->walked=FALSE;
        return(node);
    }/*Network_NewNode;*/
    
    
    
    typedef List_P              Network_T;
    
    static Network_T Network_New(void)
    {
        return(NULL);
    }/*Network_New;*/
    


    static void List_IncludeNode(List_P* list,Network_NodeP node)
    {
            List_P          new;
            
        if(!List_ContainsData(*list,node)){
            new=List_New();
            new->data=node;
            List_Include(list,new);
        }
    }/*List_IncludeNode;*/


    
    static BOOLEAN Network_DataDiffer(List_P element,void* param)
    {
            Network_NodeP       node;
        
        node=element->data;
        return(node->data!=param);
    }/*Network_DataDiffer;*/
        

    static Network_NodeP Network_IncludeData(Network_T* network,void* data)
    {
            List_P              new;
            Network_NodeP       node;
        
        new=List_While(*network,Network_DataDiffer,data);
        if(new==NULL){
            node=Network_NewNode();
            node->data=data;
            new=List_New();
            new->data=node;
            List_Include(network,new);
        }
        return(new->data);
    }/*Network_IncludeData; */


#if 0
    static BOOLEAN Network_ResetNode(List_P element,void* param)
    {
            int*                count;
            Network_NodeP       node;

        count=param;
        (*count)++;
        node=element->data;
        node->walked=FALSE;
        node->counter=0;
        return(TRUE);
    }/*Network_ResetNode;*/
    

    static int Network_Reset(Network_T network)
    {
            int         count;
        
        count=0;
        List_While(network,Network_ResetNode,&count);
        return(count);
    }/*Network_Reset;*/

    static BOOLEAN Network_RemoveReflexivityOfNode(List_P element,void* param)
    {
            Network_NodeP       node;

        node=element->data;
        List_Exclude(&(node->neighbors),element);
        return(TRUE);
    }/*Network_ResetNode;*/
    

    static void Network_RemoveReflexivity(Network_T network)
    {
        List_While(network,Network_RemoveReflexivityOfNode,NULL);
    }/*Network_RemoveReflexivity;*/
#endif
    
    
    static BOOLEAN Network_NumberLevelOfNode(List_P element,void* param)
    {
            Network_NodeP       node;
            int*                level;
            int                 sublevel;
            
        level=param;
        node=element->data;
        if((node->counter<(*level))&&(!(node->walked))){
            node->counter=(*level);
            node->walked=TRUE;
            sublevel=(*level)+1;
            List_While(node->neighbors,Network_NumberLevelOfNode,&sublevel);
            node->walked=FALSE;
        }
        return(TRUE);
    }/*Network_NumberLevelOfNode;*/
    

    static void Network_NumberLevels(Network_T network)
    {
            int         level;
            
        level=1;
        List_While(network,Network_NumberLevelOfNode,&level);
    }/*Network_NumberLevels;*/


    static BOOLEAN Network_PrintNodeLabel(List_P element,void* param)
    {
        FILE*               output;
        Network_NodeP       node;
        
        output=param;
        node=element->data;
        fprintf(output,"\t%s",(const char*)(node->data));
        return(TRUE);
    }/*Network_PrintNodeLabel;*/
    

    static BOOLEAN Network_PrintNode(List_P element,void* param)
    {
            FILE*               output;
            Network_NodeP       node;
        
        output=param;
        node=element->data;
        if(!(node->walked)){
            node->walked=TRUE;
            fprintf(output,"%6d",node->counter);
            Network_PrintNodeLabel(element,output);
            List_While(node->neighbors,Network_PrintNodeLabel,output);
            fprintf(output,"\n");
        }
        return(TRUE);
    }/*Network_PrintNode;*/
    
    
    static void Network_Print(Network_T network,FILE* output)
    {
        List_While(network,Network_PrintNode,output);
    }/*Network_Print;*/
    

    static BOOLEAN Network_NextWord(char** start,char** end)
    {           
        while(((**start)==' ')||((**start)=='\t')){
            (*start)++;
        }
        if(((**start)!='\n')&&((**start)!=(char)0)){
            (*end)=(*start);
            while(((**end)!=' ')&&((**end)!='\t')
                    &&((**end)!='\n')&&((**end)!=(char)0)){
                (*end)++;
            }
            if(((**end)==' ')||((**end)=='\t')){
                (**end)=(char)0;
                (*end)++;
            }else{
                (**end)=(char)0;
            }
            return(TRUE);
        }else{
            return(FALSE);
        }
    }/*Network_NextWord;*/
    
    
    static Network_T Network_Read(FILE* input,StrPool_T pool)
    {
#define Network_LineSize        (4096)
        Network_T   network;
        char        line[Network_LineSize];
            
        network=Network_New();
        while(fgets(line,Network_LineSize-1,input)!=NULL){
                char*       label;
                char*       endlabel;
                
            label=line;
            if(Network_NextWord(&label,&endlabel)){
                    String_P            string;
                    Network_NodeP       node;
                
                string=StrPool_IncludeString(&(pool),label);
                node=Network_IncludeData(&(network),string);
                label=endlabel;
                while(Network_NextWord(&label,&endlabel)){
                        Network_NodeP       neighbor;
                    string=StrPool_IncludeString(&(pool),label);
                    neighbor=Network_IncludeData(&(network),string);
                    List_IncludeNode(&(node->neighbors),neighbor);
                    label=endlabel;
                }
            }
        }
        return(network);
    }/*Network_Read;*/
    

int main(int argc,char** argv)
{
    StrPool_T pool=StrPool_New();
    Network_T network=Network_Read(stdin,pool);
    Network_NumberLevels(network);
    Network_Print(network,stdout);
    return(0);
}/*main;*/

/*** network.c                        --                     --          ***/

/******************************************************************************
FILE:               cplusinherited.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program prints the inheritance tree for a set of C++ classes.
USAGE
    cplusinherited <inherit_relation_file> \
        [ <interface_list_file> <source_list_file> ]
    The <inherit_relation_file> is a file containing couples of subclass class,
    each on one tab-separacted line.
    The <interface_list_file> and <source_list_file> are files containing
    one each line the path name of a class header or class source file.
    If these list of files are present, then the C++ files are corrected:
        the
            class <subclassname> : public <classname>
        lines are replaced with:
            #define <subclassname>_SUPER <classname>
            class <subclassname> : public <subclassname>_SUPER
        .
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1992-??-?? <PJB> Creation.
    1993-12-09 <PJB> Added file parameters.
LEGAL
    GPL
    
    Copyright Pascal Bourguignon 1992 - 1993
    
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
#include <ctype.h>

typedef struct {
    char*           classname;          /*  "TView" */
    char*           supername;          /*  "TObject" */
}               EntryT;


    static void LoadRelations(const char* filename,char* hbuffer,int hbufsize,
                    EntryT* hierarchy,int* hcount)
    {
            FILE*               f;
            int                 size;
            char*               p;

        f=fopen(filename,"r");
        if(f==NULL){
            perror("LoadRelations fopen");
            exit(1);
        }
        size=fread(hbuffer,sizeof(char),(unsigned)hbufsize,f);
        if(size==0){
            perror("LoadRelations fread");
            exit(1);
        }
        fclose(f);
        
        hbuffer[size]=(char)0;
        hbuffer[size+1]=(char)0;
        (*hcount)=0;
        p=hbuffer;
        while((*p)!=(char)0){
            hierarchy[(*hcount)].classname=p;
            while(((*p)!='\n')&&((*p)!=(char)9)){
                p++;
            }
            if((*p)==(char)9){
                (*p)=(char)0;
                p++;
                hierarchy[(*hcount)].supername=p;
                while(((*p)!='\n')&&((*p)!=(char)0)){
                    p++;
                }
            }else{
                hierarchy[(*hcount)].supername=NULL;
            }
            (*hcount)++;
            (*p)=(char)0;
            p++;
        }
    }/* LoadRelations; */



    typedef struct classdesc {
        const char*             classname;
        struct classdesc*       brother;
        struct classdesc*       subclasses;
    }                       classdesc;
    typedef void (*classtree_action)(classdesc*,int);
        

    static classdesc* classtree_new(const char* classname)
    {
        classdesc*          newnode;
        newnode=malloc(sizeof(*newnode));
        newnode->classname=classname;
        newnode->brother=NULL;
        newnode->subclasses=NULL;
        return(newnode);
    }/* classtree_new; */
    
    
    static void classtree_free(classdesc* tree)
    {
        if(tree->subclasses!=NULL){
            classtree_free(tree->subclasses);
        }
        if(tree->brother!=NULL){
            classtree_free(tree->brother);
        }
        free(tree);
    }/* classtree_free; */
    
    
    static classdesc* classtree_find(classdesc* tree,const char* classname)
    {
            classdesc*      found;
        if(tree==NULL){
            return(tree);
        }else if(classname==NULL){
            return(NULL);
        }else if(strcmp(classname,tree->classname)==0){
            return(tree);
        }else{
            found=classtree_find(tree->subclasses,classname);
            if(!found){
                return(classtree_find(tree->brother,classname));
            }else{
                return(found);
            }
        }
    }/* classtree_find; */
    
#if 0   
    static void classtree_addbrother(classdesc* tree,classdesc* node)
    {
        node->brother=tree->brother;
        tree->brother=node;
    }/* classtree_addbrother; */
#endif
    
    static void classtree_addsubclass(classdesc* tree,classdesc* node)
    {
        node->brother=tree->subclasses;
        tree->subclasses=node;
    }/* classtree_addsubclass; */
    
    
    static classdesc* classtree_extract(classdesc* tree,classdesc* node)
    {
            classdesc*      extracted;
        
        if(tree==NULL){
            return(NULL);
        }
        if(node==tree->brother){
            tree->brother=node->brother;
            node->brother=NULL;
            return(node);
        }else{
            extracted=classtree_extract(tree->brother,node);
            if(extracted){
                return(extracted);
            }
        }
         if(node==tree->subclasses){
            tree->subclasses=node->brother;
            node->brother=NULL;
            return(node);
        }else{
            extracted=classtree_extract(tree->subclasses,node);
            if(extracted){
                return(extracted);
            }
        }
        return(NULL);
    }/* classtree_extract; */
    
    
    static void classtree_print(classdesc* node,int level)
    {
        printf("%*c %s\n",2+level*4,':',node->classname);
    }/* classtree_print; */
    
    
    static void classtree_prefixed(classdesc* tree,classtree_action action,int level)
    {
        action(tree,level);
        if(tree->subclasses!=NULL){
            classtree_prefixed(tree->subclasses,action,level+1);
        }
        if(tree->brother!=NULL){
            classtree_prefixed(tree->brother,action,level);
        }
    }/* classtree_prefixed; */
    
    
    static int classtree_numberOfChildren(classdesc* tree)
    {
            int         result=0;
            classdesc*  child;
        child=tree->subclasses;
        while(child!=NULL){
            result++;
            child=child->brother;
        }
        return(result);
    }/* classtree_numberOfChildren; */
    
    
    static int compareAlpha(const void* a,const void* b)
    {
        return(strcmp((*(const classdesc* const*)a)->classname,
                      (*(const classdesc* const*)b)->classname));
    }/* compareAlpha; */
    
    
    static void classtree_sortAlphabetically(classdesc* tree)
    {
            classdesc**     children;
            classdesc*      child;
            int             count;
            int             i;
            
        count=classtree_numberOfChildren(tree);
        if(count>1){
            children=(classdesc**)malloc(sizeof(classdesc*)*count);
            child=tree->subclasses;
            for(i=0;i<count;i++){
                children[i]=child;
                child=child->brother;
            }
            qsort((void*)children,(unsigned)count,
                  sizeof(classdesc*),compareAlpha);
            child=NULL;
            for(i=count-1;i>=0;i--){
                children[i]->brother=child;
                child=children[i];
            }
            tree->subclasses=child;
            free(children);
        }
        child=tree->subclasses;
        while(child!=NULL){
            classtree_sortAlphabetically(child);
            child=child->brother;
        }
    }/* classtree_sortAlphabetically; */
    

typedef struct {
    int             depth;
    classdesc*      subtree;
}               treeAndDepthT;


    static int compareDepth(const void* a,const void* b)
    {
        if(((const treeAndDepthT*)a)->depth < ((const treeAndDepthT*)b)->depth){
            return(-1);
        }else if(((const treeAndDepthT*)a)->depth
                 > ((const treeAndDepthT*)b)->depth){
            return(1);
        }else{
            return(strcmp(((const treeAndDepthT*)a)->subtree->classname,
                          ((const treeAndDepthT*)b)->subtree->classname));
        }
    }/* compareDepth; */
    
    
    static int classtree_sortDepth(classdesc* tree)
    {
            treeAndDepthT*  children;
            classdesc*      child;
            int             depth;
            int             count;
            int             i,d;
        
        depth=0;
        count=classtree_numberOfChildren(tree);
        if(count>1){
            children=(treeAndDepthT*)malloc(sizeof(treeAndDepthT)*count);
            child=tree->subclasses;
            for(i=0;i<count;i++){
                d=classtree_sortDepth(child);
                if(depth<d){
                    depth=d;
                }
                children[i].depth=d;
                children[i].subtree=child;
                child=child->brother;
            }
            qsort((void*)children,(unsigned)count,
                  sizeof(treeAndDepthT),compareDepth);
            child=NULL;
            for(i=count-1;i>=0;i--){
                children[i].subtree->brother=child;
                child=children[i].subtree;
            }
            tree->subclasses=child;
            free(children);
        }else if(count>0){
            depth=classtree_sortDepth(tree->subclasses);
        }else{
            depth=0;
        }
        return(depth+1);
    }/* classtree_sortDepth; */
    


    static classdesc* BuildTree(EntryT* hierarchy,int hcount)
    {
            int             i;
            classdesc*      tree;
            classdesc*      superclassnode;
            classdesc*      classnode;
            
        tree=classtree_new("<ROOT>");
        for(i=0;i<hcount;i++){
            if(hierarchy[i].supername==NULL){
                /*  hierarchy[i] is a new root class. */
                superclassnode=tree;
            }else{
                superclassnode=classtree_find(tree,hierarchy[i].supername);
                if(superclassnode==NULL){
                    /*  hierarchy[i] has a superclass yet unclassified. */
                    superclassnode=classtree_new(hierarchy[i].supername);
                    classtree_addsubclass(tree,superclassnode);
                    /* classtree_addbrother(tree,superclassnode); */
                }
            }
            classnode=classtree_find(tree,hierarchy[i].classname);
            if(classnode!=NULL){
                /*  we must extract for the implementation of classdesc */
                /*  use brother lists while a node may have several */
                /*  brother lists when multiple inheritance is allowed. */
                /*  (Brotherhood is actually defined according to a parent). */
                classtree_extract(tree,classnode);
            }else{
                classnode=classtree_new(hierarchy[i].classname);
            }
            classtree_addsubclass(superclassnode,classnode);
        }
        
        return(tree);
    }/* BuildTree; */
        

    static int CheckHierarchy(char* subclass,char* superclass,
                EntryT* hierarchy,int hcount)
    {
            int     i;
        i=0;
        while(i<hcount){
            if((strcmp(subclass,hierarchy[i].classname)==0)
            &&((superclass==NULL)
             ||(strcmp(superclass,hierarchy[i].supername)==0))){
                return(1);
            }
            i++;
        }
        return(0);
    }/* CheckHierarchy; */
    


static char         fbuffer[8192];
static char         dbuffer[256*1024];

    static int Pos(const char* string,const char* substring,unsigned int startpos)
    {
        unsigned int        l;
            
        l=strlen(substring);
        while(string[startpos]!=(char)0){   
            if(strncmp(string+startpos,substring,l)==0){
                return(startpos);
            }
            startpos++;
        }
        return(-1);
    }/* Pos; */

static char     wbuffer[1024];


    static void GetNextWords(char* string,int pos,char** words,int wcount)
    {
            int         i;
            int         w;
            
        i=0;
        w=0;
        while(w<wcount){
            words[w]=wbuffer+i;
            if(ispunct(string[pos])){
                wbuffer[i]=string[pos];
                i++;
                pos++;
            }else{
                while(isalnum(string[pos])){
                    wbuffer[i]=string[pos];
                    pos++;
                    i++;
                }
            }
            wbuffer[i]=(char)0;
            i++;
            while(isspace(string[pos])){
                pos++;
            }
            w++;
        }
    }/* GetNextWords; */
    
    
    static void MoveUp(char* from,char* to,int size)
    {
        to+=size;
        from+=size;
        while(size-->0){
            to--;
            from--;
            (*to)=(*from);
        }
    }/* MoveUp; */
    

    static int UpdateInterface(int size,EntryT* hierarchy,int hcount)
    {
        unsigned int    pos;
        char*           words[5];
        char            definition[128];
        int             inclen;
        int             incpos;
            
        pos=0;
        pos=Pos(dbuffer,"class",pos);
        while(pos>0){
            GetNextWords(dbuffer,(signed)pos,words,5);
            if((strcmp(words[2],":")==0)
            &&(strcmp(words[3],"public")==0)){
                if(!CheckHierarchy(words[1],words[4],hierarchy,hcount)){
                    printf("I don't have %s %s in class hierarchy!\n",
                            words[1],words[4]);
                }
                        
                sprintf(definition,"#define %s_SUPER %s\n",
                            words[1],words[4]);
                inclen=strlen(definition);
                incpos=pos;
                while((dbuffer[incpos])!='\n'){
                    incpos--;
                }
                incpos++;
                MoveUp(dbuffer+incpos,dbuffer+incpos+inclen,size-incpos+1);
                strncpy(dbuffer+incpos,definition,(unsigned)inclen);
                size+=inclen;
                pos+=inclen;
            }
            pos++;
            pos=Pos(dbuffer,"class",pos);
        }
        return(size);
    }/* UpdateInterface; */
    
    
    static void CorrectInterfaces(const char* filename,EntryT* hierarchy,int hcount)
    {
            FILE*       f;
            int         size,wsize;
            char*       p;
            char*       np;
        
        
        /*  load the file list: */
        f=fopen(filename,"r");
        if(f==NULL){
            perror("CorrectInterfaces fopen");
            exit(1);
        }
        size=fread(fbuffer,sizeof(char),sizeof(fbuffer),f);
        if(size==0){
            perror("CorrectInterfaces fread");
            exit(1);
        }
        fclose(f);
    
        /*  process all file in the file list: */
        fbuffer[size]=(char)0;
        fbuffer[size+1]=(char)0;
        p=fbuffer;
        np=fbuffer;
        while((*p)!=(char)0){
            /*  get the next file name in the file list: */
            while(((*np)!='\n')&&((*np)!=(char)0)){
                np++;
            }
            (*np)=(char)0;
            np++;
            /*  process the file: */
            f=fopen(p,"r+");
            if(f==NULL){
                    char        message[256];
                sprintf(message,"CorrectInterfaces fopen %s",p);
                perror(message);
                exit(1);
            }
            /*  load the file: */
            size=fread(dbuffer,sizeof(char),sizeof(dbuffer),f);
            if(size==0){
                    char        message[256];
                sprintf(message,"CorrectInterfaces fread %s",p);
                perror(message);
                exit(1);
            }
            dbuffer[size]=(char)0;
            dbuffer[size+1]=(char)0;
            
            /*  correct it: */
            size=UpdateInterface(size,hierarchy,hcount);
            
            /*  save the file: */
            if(fseek(f,0,SEEK_SET)<0){
                    char        message[256];
                sprintf(message,"CorrectInterfaces fseek %s",p);
                perror(message);
                exit(1);
            }
            
            wsize=fwrite(dbuffer,sizeof(char),(unsigned)size,f);
            if(wsize!=size){
                    char        message[256];
                sprintf(message,"CorrectInterfaces fwrite %s",p);
                perror(message);
                exit(1);
            }
            fclose(f);
            p=np;
        }
    }/* CorrectInterfaces; */


    static int UpdateImplementation(int size,EntryT* hierarchy,int hcount)
    {
            int     pos;
            int     i,j,l;
            char    classname[128];
            char    definition[128];
            
        pos=0;
        while(dbuffer[pos]!=(char)0){
            if(strncmp(dbuffer+pos,"::",2)==0){
                i=pos-1;
                while(!isalnum(dbuffer[i])){
                    i--;
                }
                j=i;
                while(isalnum(dbuffer[i])){
                    i--;
                }
                l=j-i;
                strncpy(classname,dbuffer+i+1,(unsigned)l);
                classname[l]=(char)0;
            }else if(strncmp(dbuffer+pos,"inherited",9)==0){
                if(!CheckHierarchy(classname,NULL,hierarchy,hcount)){
                    printf("I don't have %s in class hierarchy!\n",
                            classname);
                }
                sprintf(definition,"%s_SUPER",classname);
                l=strlen(definition);
                MoveUp(dbuffer+pos,dbuffer+pos+l-9,size-pos);
                strncpy(dbuffer+pos,definition,(unsigned)l);
                size+=l-9;
                pos+=l;
            }
            pos++;
        }
        return(size);
    }/* UpdateImplementation; */


    static void CorrectImplementation(const char* filename,
                        EntryT* hierarchy,int hcount)
    {
            FILE*       f;
            int         size,wsize;
            char*       p;
            char*       np;
            
        f=fopen(filename,"r");
        if(f==NULL){
            perror("CorrectImplementation fopen");
            exit(1);
        }
        size=fread(fbuffer,sizeof(char),sizeof(fbuffer),f);
        if(size==0){
            perror("CorrectImplementation fread");
            exit(1);
        }
        fclose(f);
        
        fbuffer[size]=(char)0;
        fbuffer[size+1]=(char)0;
        p=fbuffer;
        np=fbuffer;
        while((*p)!=(char)0){
            while(((*np)!='\n')&&((*np)!=(char)0)){
                np++;
            }
            (*np)=(char)0;
            np++;
            f=fopen(p,"r+");
            if(f==NULL){
                    char        message[256];
                sprintf(message,"CorrectImplementation fopen %s",p);
                perror(message);
                exit(1);
            }
            size=fread(dbuffer,sizeof(char),sizeof(dbuffer),f);
            if(size==0){
                    char        message[256];
                sprintf(message,"CorrectImplementation fread %s",p);
                perror(message);
                exit(1);
            }
            dbuffer[size]=(char)0;
            dbuffer[size+1]=(char)0;
            
            size=UpdateImplementation(size,hierarchy,hcount);
            
            if(fseek(f,0,SEEK_SET)<0){
                    char        message[256];
                sprintf(message,"CorrectImplementation fseek %s",p);
                perror(message);
                exit(1);
            }
            
            wsize=fwrite(dbuffer,sizeof(char),(unsigned)size,f);
            if(wsize!=size){
                    char        message[256];
                sprintf(message,"CorrectImplementation fwrite %s",p);
                perror(message);
                exit(1);
            }
            fclose(f);
            p=np;
        }
    }/* CorrectImplementation; */


    static void usage(const char* pname)
    {
        fprintf(stderr,"Usage:  %s [-a|-d] <inherit_relation_file>"
            " [ <interface_list_file> <source_list_file> ]\n",pname);
        fprintf(stderr,"    -a   sort each level alphabetically.\n");
        fprintf(stderr,"    -d   sort subtrees by increasing depth.\n");
        fprintf(stderr,"         (default is no sort).\n");
    }/* usage; */
    
        
int main(int argc,char**argv)
{
        const char*         relationFName=NULL;
        const char*         interfacesFName=NULL;
        const char*         implementationsFName=NULL;
        short               sortAlgorithm=0;
        int                 i;

        static EntryT       hierarchy[2000];
        static int          hcount;
        static char         hbuffer[81920];
        classdesc*          tree;
            
    for(i=1;i<argc;i++){
        if(argv[i][0]=='-'){
            switch(argv[i][1]){
            case 'a':
                sortAlgorithm=1;
                break;
            case 'd':
                sortAlgorithm=2;
                break;
            default:
                usage(argv[0]);
                return(1);
                break;
            }
        }else{
            if(relationFName==NULL){
                relationFName=argv[i];
            }else if(interfacesFName==NULL){
                interfacesFName=argv[i];
            }else if(implementationsFName==NULL){
                implementationsFName=argv[i];
            }else{
                usage(argv[0]);
                return(1);
            }
        }
    }
    if((relationFName==NULL)
    ||((interfacesFName!=NULL)&&(implementationsFName==NULL))){
        usage(argv[0]);
        return(1);
    }

    LoadRelations(relationFName,hbuffer,sizeof(hbuffer),hierarchy,&hcount);
    tree=BuildTree(hierarchy,hcount);
    switch(sortAlgorithm){
    case 1:
        classtree_sortAlphabetically(tree);
    case 2:
        classtree_sortDepth(tree);
        break;
    default:
        break;
    }
    classtree_prefixed(tree,classtree_print,0);
    classtree_free(tree);

    if(interfacesFName){
        CorrectInterfaces(interfacesFName,hierarchy,hcount);
        CorrectImplementation(implementationsFName,hierarchy,hcount);
    }
    return(0);
}

/*** cplusinherited.c                 -- 2003-12-02 11:16:55 -- pascal   ***/

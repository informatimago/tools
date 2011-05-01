/******************************************************************************
FILE:               make_person_aliases.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    Prints aliases for a person's name.
USAGE
    make_person_aliases Firstname Secondname ... Lastname : username
        
    Prints :
        Firstname.Secondname.Lastname:username
        FirstnameSecondnameLastname:username
        FirstnameSecondname_Lastname:username
        Firstname_SecondnameLastname:username
        Firstname_Secondname_Lastname:username
        FSLastname:username
        Firstname_Lastname:username
        ...
        FirstnameSecondname.Lastname:username
        Firstname.SecondnameLastname:username
        FSLastname:username
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1995-03-17 <PJB> Creation.
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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char*        pname;


static char* newstr(const char* str)
{
    char* result=malloc(strlen(str)+1);
    strcpy(result,str);
    return(result);
}

static const char* cseparators[]={"","-","_",".",0};
static char* separators[sizeof(cseparators)/sizeof(char*)];
static void init_separators(void)
{
    unsigned int i;
    for(i=0;cseparators[i];i++){
        separators[i]=newstr(cseparators[i]);
    }
    separators[i]=0;
}


static void usage(const char* upname)
{
    fprintf(stderr,
            "Usage: %s Firstname Secondname ... Lastname : username\n",
            upname);
}/*usage*/
    

    
static void* checkmalloc(size_t size)
{
    void* b=malloc(size);
    if(b==NULL){
        perror(pname);
        exit(1);
    }
    return(b);
}/*checkmalloc*/
    
    
/* list of string data type: */
    
typedef struct string {
    char*       string;
    struct string*  next;
}               string_t;
typedef struct {
    string_t*       head;
    string_t*       tail;
    int             count;
}               string_list_t;
    
    
static string_list_t* string_list_new(void)
{
    string_list_t*  list;
    list=(string_list_t*)checkmalloc(sizeof(string_list_t));
    list->head=NULL;
    list->tail=NULL;
    list->count=0;
    return(list);
}/*string_list_new*/
    
    
static void string_list_addstring(string_list_t* list,char* string)
{
    string_t*   node;
    node=(string_t*)checkmalloc(sizeof(string_t));
    node->string=string;
    node->next=NULL;
    if(list->tail==NULL){
        list->head=node;
    }else{
        list->tail->next=node;
    }
    list->tail=node;
    list->count++;
}/*string_list_addstring*/
    
    
#if 0
static void string_list_print(string_list_t* list)
{
    string_t*   node;
    node=list->head;
    while(node!=NULL){
        printf("%s\n",node->string);
        node=node->next;
    }
}/*string_list_print*/
#endif    
    
static char** string_list_to_vector(string_list_t* list)
{
    char**      vector;
    int         i;
    string_t*   node;
            
    vector=(char**)checkmalloc((sizeof(char*)*(size_t)(list->count+1)));
    for(i=0,node=list->head;i<list->count;i++,node=node->next){
        vector[i]=node->string;
    }
    vector[i]=NULL;
    return(vector);
}/*string_list_to_vector*/
    
    
static int string_list_count(string_list_t* list)
{
    return(list->count);
}/*string_list_count*/
    
    
/* array of binary digits data type: */
#if 0    
static char* binarray_new(unsigned int size)
{
    char*   n;
    unsigned int     i;
            
    n=(char*)checkmalloc(size+1);
    for(i=0;i<size;i++){
        n[i]=0;
    }
    n[i]=2;
    return(n);
}/*binarray_new*/
    
    
static void binarray_increment(char* n)
{
    while(*n==1){
        *n=0;
        n++;
    }
    if(*n==0){
        *n=1;
    }
}/*binarray_increment*/
    
    
static int binarray_is_max(char* n)
{
    while(*n==1){
        n++;
    }
    return(*n==2);
}/*binarray_is_max*/
#endif
    
/* string combinator: */

typedef struct {
    unsigned int        base; /* = number of values */
    char**              values;
    unsigned int        size; /* = number of digits */
    unsigned int*       digits;
    unsigned int        overflow; /* or underflow */
    char**              currentVector;
}                   combinator_t;

    
static void combinator_reset(combinator_t* comb)
{
    unsigned int i;
    for(i=0;i<comb->size;i++){
        comb->digits[i]=0;
    }
    comb->overflow=0;
}/*combinator_reset*/
    
    
static combinator_t* combinator_new(char** values,unsigned int size)
{
    combinator_t*   comb;
    unsigned int    i;
    comb=(combinator_t*)checkmalloc(sizeof(combinator_t));
    i=0;
    while(values[i]!=0){
        i++;
    }
    comb->base=i;
    comb->values=values;
    comb->size=size;
    comb->digits=checkmalloc((sizeof(comb->digits[0])*comb->size));
    combinator_reset(comb);
    comb->currentVector=(char**)checkmalloc((sizeof(char*)*comb->size+1));
    return(comb);
}/*combinator_new*/
    

#if 0
static void combinator_delete(combinator_t* comb)
{
    free(comb->digits);
    free(comb->currentVector);
    free(comb);
}/*combinator_delete*/
#endif
    
static unsigned int combinator_overflow(combinator_t* comb)
{
    return(comb->overflow);
}/*combinator_overflow*/
    

static void combinator_increment(combinator_t* comb)
{
    unsigned int i;
    i=comb->size-1;
    comb->digits[i]++;
    while((i>0)&&(comb->digits[i]>=comb->base)){
        comb->digits[i]=0;
        i--;
        comb->digits[i]++;
    }
    if((i==0/*optimization*/)&&(comb->digits[0]>=comb->base)){
        comb->digits[0]=0;
        comb->overflow=1;
    }else{
        comb->overflow=0;
    }
}/*combinator_increment*/
    
    
static char** combinator_vector(combinator_t* comb)
{
    unsigned int i;
    for(i=0;i<comb->size;i++){
        comb->currentVector[i]=comb->values[comb->digits[i]];
    }
    comb->currentVector[comb->size]=0;
    return(comb->currentVector);
}/*combinator_vector*/
    


/* vector of strings data type: */

static unsigned int vector_count(char** vector)
{
    unsigned int i;
    i=0;
    while(*vector++!=0){
        i++;
    }
    return(i);
}/*vector_count*/
    
    
static char** vector_intersperse(char** a,char** b)
/*
  PRE:    vector_count(a)=vector_count(b)+1
*/
{
    char** result=(char**)checkmalloc((sizeof(char*)*(vector_count(a)+vector_count(b)+1)));
    char** r;
    r=result;
    while((*a!=0)&&(*b!=0)){
        (*r++)=(*a++);
        (*r++)=(*b++);
    }
    while(*a!=0){   /* normaly there's only one element remaining */
        (*r++)=(*a++);
    }
    while(*b!=0){   /* just in case */
        (*r++)=(*b++);
    }
    *r=0;
    return(result);
}/*vector_intersperse*/
    
    
static char* vector_concatenate(char** vector)
{
    char* string;
    int len;
    char** v;
    char* s;
            
    len=0;
    v=vector;
    while(*v!=0){
        len+=(int)strlen(*v++);
    }
    string=(char*)checkmalloc(sizeof(char)*(size_t)(len+1));
    v=vector;
    s=string;
    while(*v!=0){   /* for each word */
        char* f=*v;
        while(*f!=0){ /* for each character */
            (*s++)=(*f++);
        }
        v++;
    }
    *s=0;
    return(string);
}/*vector_concatenate*/
    
    
#if 0
static void vector_print(const char** vector)
{
    while(*vector!=NULL){
        printf("%s\n",*vector++);
    }
}/*vector_print*/
#endif    

/* string data type: */

static void string_tolower(char* word)
{
    while(*word!='\0'){
        if(isupper(*word)){
            *word=tolower(*word);
        }
        word++;
    }
}/*string_tolower*/
    
    
static void string_capitalize(char* word)
{
    if(islower(*word)){
        *word=toupper(*word);
    }
    string_tolower(word+1);
}/*string_capitalize*/
    

/* make person aliases procedures: */

static void addname(string_list_t* list,char* name)
/*
  DO: add each word in name to the list. A word is a sequence
  of alphabetic characters. Non-alphabetic characters in
  name are forgotten.
*/
{
    char*       nextword;
            
    /*
      STOP CONDITION:
      *nextword=='\0'
            
      INVARIANT:
      (isalpha(*nextword) || (*nextword=='\0'))
      && (isalpha(*name) || (*name=='\0'))
      foreach char in name, isalpha(char)
      foreach char in [name+strlen(name) .. nextword-1], 
      !isalpha(char)
            
      INCREMENT:
      while(isalpha(*nextword)){
      nextword++;
      }
      if(*nextword!='\0'){
      *nextword='\0';
      nextword++;
      }
      string_list_addstring(list,name);
      while((*nextword!='\0')&&(!isalpha(*nextword))){
      nextword++;
      }
      INITIALISATION:
      while((*name!='\0')&&(!isalpha(*name))){
      name++;
      }
      nextword=name;
    */
        
    while((*name!='\0')&&(!isalpha(*name))){
        name++;
    }
    nextword=name;
    while(*nextword!='\0'){
        while(isalpha(*nextword)){
            nextword++;
        }
        if(*nextword!='\0'){
            *nextword='\0';
            nextword++;
        }
        string_list_addstring(list,name);
        while((*nextword!='\0')&&(!isalpha(*nextword))){
            nextword++;
        }
        name=nextword;
    }
}/*addname*/
    
    
static char** make_person_aliases(char** names,char** seps)
{
    string_list_t*      aliases;  /* the result: list of all aliases */

    string_list_t*      words;    /* the words in the names */
    char**              wordsvector;
            
    unsigned int        sepcount;
    unsigned int        maxseplen;
    unsigned int        aliaslen; /* the maximum length for an alias */

    combinator_t*       counter;
    
    /* make the aliases list */
    aliases=string_list_new();
    
    /* make the vector of words in the person's name */
    {
        int i;
        words=string_list_new();
        i=0;
        while(names[i]!=NULL){
            addname(words,names[i]);
            i++;
        }
        wordsvector=string_list_to_vector(words);
    }

    /* compute the length of the biggest separator (for aliaslen) */
    {
        char** sep=seps;
        maxseplen=0;
        while(*sep!=0){
            unsigned int seplen=(unsigned int)strlen(*sep++);
            if(seplen>maxseplen){
                maxseplen=seplen;
            }
        }
    }

    /* compute the maximum length for an alias, */
    /* and beautify the words in the name */
    {
        char** w;
        aliaslen=0;
        w=wordsvector;
        while(*w!=NULL){
            string_capitalize(*w);
            aliaslen+=(unsigned int)strlen(*w++);
        }
        sepcount=(unsigned int)(string_list_count(words)-1);
        aliaslen+=sepcount*maxseplen;
    }
    
    
    /* generate all aliases by taking each word in the name and */
    /* interspersing all combinaisons of seps */
        
    counter=combinator_new(seps,sepcount);
    combinator_reset(counter);
    while(!combinator_overflow(counter)){
        char** aliasvector=vector_intersperse(wordsvector,
                                              combinator_vector(counter));
        char* alias=vector_concatenate(aliasvector);
        string_list_addstring(aliases,alias);
        combinator_increment(counter);
    }
        
    /* beautify in another way the words in the name */
    {
        char** w;
        w=wordsvector;
        while(*w!=NULL){
            string_tolower(*w++);
        }
    }
        
    /* truncate all the names but the last to its initial */
    {
        char** w=wordsvector;
        while(*(w+1)!=NULL){ /* don't truncate the lastname */
            (*w)[1]='\0'; /* truncate the current name */
            string_list_addstring(aliases,
                                  vector_concatenate(wordsvector));
            w++;
        }
    }
#if 0
    /* 
       We no longer use the firstname alone;
       this is usually the username, or else it would be too ambiguous.
    */
    /* first name */
    char*               firstname;
    firstname=(char*)checkmalloc(aliaslen+1);
    strcpy(firstname,*wordsvector);
    string_list_addstring(aliases,firstname);
#endif

    return(string_list_to_vector(aliases));
}/*make_person_aliases*/
    
    
static void printaliases(char** aliases,const char* username)
{
    while(*aliases!=NULL){
        printf("%s:%s\n",*aliases++,username);
    }
}/*printaliases*/
    
    
int main(int argc,char** argv)
{
    char**      names;
    char**      aliases;
    char*       username;
    int         i;

    pname=argv[0];
    
    if(argc<4){
        usage(pname);
        return(1);
    }
    if(strcmp(":",argv[argc-2])!=0){
        usage(pname);
        return(2);
    }
    username=argv[argc-1];
    
    names=(char**)checkmalloc((sizeof(char*)*(size_t)argc));
    for(i=1;i<argc-2;i++){
        names[i-1]=argv[i];
    }
    init_separators();
    aliases=make_person_aliases(names,separators);
    printaliases(aliases,username);
    return(0);
}/*main*/
    
    
/**** THE END ****/

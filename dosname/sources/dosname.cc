/******************************************************************************
FILE:               dosname.cc
LANGUAGE:           C++
SYSTEM:             UNIX
USER-INTERFACE:     ANSI
DESCRIPTION
    This program recursively copies a directory, renaming the elements to
    respect the MS-DOS limitations of 8 unsigned characters, dot, 3 unsigned
    characters.
    It creates in the destination directories files.lst files containing 
    the access rights, the dos name, and the unix name of each element.
USAGE
    See the usage() function.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:36:35  pascal
   Initial revision
   
    1995-01-01 <PJB> Creation.
BUGS
LEGAL
    Copyright Pascal J. Bourguignon 1995 - 2011

    GPL

    This file is part of the dosname tool.

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
extern "C"{
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
}
#include <dosname.hh>
#include <Pressure.hh>
#include BpClass_hh
#include <BcImplementation.h>


const char*     pname;

/*
  makeDosName (from unix to dos)
    
  Load the name conversion table.
  If the curUnixName is in the table, 
  then
  use the dosName in the table;
  else 
  Analyze curUnixName with the following grammar :
        
  curUnixName     ::= [ dot ] wordSeq { dot wordSeq } .
  wordSeq      ::= capWord { capWord } .
  capWord      ::= {initers}{capletters}
  {smallletters|digit|trailers} .
            
  capletters    ::= ABCDEFGHIJKLMNOPQRSTUVWXYZ
  smallletters  ::= abcdefghijklmnopqrstuvwxyz
  digit        ::= 0123456789
  initers      ::=  !"#&')*+=>?]^`|}
  trailers     ::=  $%(,-/:;<@[\_{~

  If the curUnixName is composed of more than two wordSeq
  then
  while the length of the wordSeq but the last is more 
  than eight
  do
  reduce the length of each of the wordSeq but the last
  of one, starting
  endwhile
  else
  if the length of the first wordSeq is more than eight
  then
  reduce the length of the first wordSeq to eight
  endif
  endif
  if the length of the last wordSeq is more than three
  then
  reduce the length of the last wordSeq to three
  endif
        
  set the name to the concatenation of the wordSeq but the last
  removing initers and trailers.
  set the extension to the last wordSeq.
  set the name and extension to lowercase.
  set the dosName to the concatenation of name dot and extension.
  if the dosName alreaydy exists whether in the name table, or in
  already converted dosName, 
  then 
  increment the name part
  endif
  endif
*/


    
void test1(INT32 argc,char** argv)
{
    INT32       i;
    Renamer*    un;
    FILE*       fCache;
    MfMode*     mode;
        
    mode=NEW(MfMode);
    mode->modeSet(00664);
        
    un=NEW(Renamer);
    un->retain();
        
    fCache=fopen(".msdosnames","r");
    if(fCache!=NULL){
        un->loadUnixCache(fCache);
        fclose(fCache);
    }
        
        
    i=1;
    while(i<argc){
        un->setUnixName(argv[i],mode);
        printf("%-16s%s\n",un->dosName(),un->unixName());
        BpObject_ProcessDeletePool();
        i++;
    }
    
    fCache=fopen(".msdosnames","w");
    if(fCache!=NULL){
        un->saveDosCache(fCache);
        fclose(fCache);
    }
    un->release();
    BpObject_ProcessDeletePool();
}//test1;


void usage(const char* upname)
{
    int plength=(int)strlen(upname);
    fprintf(stderr,"%s usage:\n"
            "  %s -d|--to-dos|-u|--to-unix [-s|--silent] [-f|--force]\n"
            "  %*s [-c|--copy|-l|--hard-link|-y|--sym-link] [-p|--print]\n"
            "  %*s <from-dir> <to-dir>\n"
            "\t  -d --to-dos       rename from UNIX names to DOS  names.\n"
            "\t  -u --to-unix      rename from DOS  names to UNIX names.\n"
            "\t  -c --copy         make copies of files (default).\n"
            "\t  -h --hard-link    make hard links of files.\n"
            "\t  -y --sym-link     make symbolic links of files.\n"
            "\t  -p --print        write the commands instead of executing them.\n"
            "\t  -s --silent       silent.\n"
            "\t  -f --force        force.\n"
            "  When making copies, the UNIX sym-links are converted to or from \n"
            "  mere DOS files containing the linked path. When making links \n"
            "  (either hard or symbolic), the sym-links are linked as well as \n"
            "  plain files. In this case sym-links are not kept on a DOS FS.\n"
            ,upname,upname,plength," ",plength," ");
}//usage;


void check_direction_set(BOOLEAN set)
{
    if(set){
        fprintf(stderr,"%s: only one of -d, -u, --to-dos, or --to-unix "
                "may be specified.\n",pname);
        usage(pname);
        exit(1);
    }
}//check_direction_set;


void check_action_set(BOOLEAN set)
{
    if(set){
        fprintf(stderr,"%s: only one of -c, -y, -h, --copy, "
                "--sym-link, or --hard-link may be specified.\n",pname);
        usage(pname);
        exit(1);
    }
}//check_action_set;


static char* dosname_basename(char* path)
{
    char* result=strrchr(path,'/');
    if(result==0){
        result=path;
    }else{
        result++;
    }
    return(result);
}//dosname_basename;


int main(INT32 argc,char** argv)
{
    int             plength;
    MfDirectory*    fromDir=0;
    MfDirectory*    toDir=0;
    char*           dirbase=0;
    char*           dirname;
    MfMode*         mode=NEW(MfMode);
    BOOLEAN         directionSet=FALSE;
    BOOLEAN         todos=FALSE;
    BOOLEAN         silent=FALSE;
    BOOLEAN         force=FALSE;
    BOOLEAN         print=FALSE;
    BOOLEAN         actionSet=FALSE;
    typedef enum {action_hardlink,action_symlink,action_copy} action_t;
    action_t        action=action_copy;
    int             i;

    pname=dosname_basename(argv[0]);
    plength=(int)strlen(pname);
    i=1;
    while(i<argc){
        if(strcmp(argv[i],"--help")==0){
            usage(pname);
            exit(0);
        }else if((strcmp(argv[i],"-s")==0)||(strcmp(argv[i],"--silent")==0)){
            silent=TRUE;
        }else if((strcmp(argv[i],"-d")==0)||(strcmp(argv[i],"--to-dos")==0)){
            check_direction_set(directionSet);
            directionSet=TRUE;
            todos=TRUE;
        }else if((strcmp(argv[i],"-u")==0)||(strcmp(argv[i],"--to-unix")==0)){
            check_direction_set(directionSet);
            directionSet=TRUE;
            todos=FALSE;
        }else if((strcmp(argv[i],"-c")==0)||(strcmp(argv[i],"--copy")==0)){
            check_action_set(actionSet);
            actionSet=TRUE;
            action=action_copy;
        }else if((strcmp(argv[i],"-h")==0)
                 ||(strcmp(argv[i],"--hard-link")==0)){
            check_action_set(actionSet);
            actionSet=TRUE;
            action=action_hardlink;
        }else if((strcmp(argv[i],"-y")==0)||(strcmp(argv[i],"--sym-link")==0)){
            check_action_set(actionSet);
            actionSet=TRUE;
            action=action_symlink;
        }else if((strcmp(argv[i],"-f")==0)||(strcmp(argv[i],"--force")==0)){
            force=TRUE;
        }else if((strcmp(argv[i],"-p")==0)||(strcmp(argv[i],"--print")==0)){
            print=TRUE;
        }else if(argv[i][0]=='-'){
            fprintf(stderr,"%s: invalid option (%s)\n",pname,argv[i]);
            usage(pname);
            exit(1);
        }else if(fromDir==0){
            fromDir=NEW(MfDirectory);
            fromDir->nameSet(NEW(BpString(argv[i])));
        }else if(toDir==0){
            dirname=argv[i];
            do{
                dirbase=strrchr(dirname,'/');
                if(dirbase==NULL){          // "xxx"
                    dirbase=dirname;
                    dirname=newstr(".");
                    break;
                }else if(dirbase==dirname){ // "/xxx"
                    dirbase=newstr("/");
                    break;
                }else{
                    (*dirbase)='\0';
                    dirbase++;
                    if(*dirbase!='\0'){     // "xxx/yyy"
                        break;
                    }                 // else  "xxx/"
                }
            }while(1);
            toDir=NEW(MfDirectory);
            toDir->nameSet(NEW(BpString(dirname)));
        }else{
            fprintf(stderr,"%s: I already have a from directory (%s)\n"
                    "%*s  and a to directory (%s);\n"
                    "%*s  what should I do of this parameter (%s)?\n",
                    pname,fromDir->name()->string(),
                    plength," ",toDir->name()->string(),
                    plength," ",argv[i]);
            usage(pname);
            exit(1);
        }
        i++;
    }
    if(!directionSet){
        fprintf(stderr,"%s: at least one of -d, -u, --to-dos, or --to-unix "
                "must be specified.\n",pname);
        usage(pname);
        exit(1);
    }
    if((fromDir==0)||(toDir==0)){
        fprintf(stderr,"%s: both source directory and destination directory "
                "must be specified.\n",pname);
        usage(pname);
        exit(1);
    }
        
    mode->modeSet(0777);
    if(todos){
        fromDir->fileSystemSet(MfDirectory_FileSystem_UNIX);
        fromDir->loadFiles();
        switch(action){
        case action_copy:
            fromDir->copyToDos(toDir,NEW(BpString(dirbase)),mode,print);
            break;
        case action_hardlink:
            fromDir->hardLinkToDos(toDir,NEW(BpString(dirbase)),mode,print);
            break;
        case action_symlink:
            fromDir->symLinkToDos(toDir,NEW(BpString(dirbase)),mode,print);
            break;
        }
    }else{
        fromDir->fileSystemSet(MfDirectory_FileSystem_MSDOS);
        fromDir->loadFiles();
        switch(action){
        case action_copy:
            fromDir->copyToUnix(toDir,NEW(BpString(dirbase)),mode,print);
            break;
        case action_hardlink:
            fromDir->hardLinkToUnix(toDir,NEW(BpString(dirbase)),mode,print);
            break;
        case action_symlink:
            fromDir->symLinkToUnix(toDir,NEW(BpString(dirbase)),mode,print);
            break;
        }
    }
    BpObject_ProcessDeletePool();
    exit(0);
}//main;


//// THE END ////

/***ID-DEBUT***********************************************************
NOM:        lcut.c
LANGAGE:    ANSI-C
SYSTEME:    ANSI (stdio,string)
TYPE:       commande
DESCRIPTION:
    Cet utilitaire coupe les lignes d'un fichier texte a la longueur indiquee.
    (En fait, ca marche aussi sur un fichier binaire, mais les '\n' sont
    quand meme traites, sauf quand il y a un '\0' dans les 'width' caracteres
    les precedant).
UTILISATION:
    lcut [width] < in > out
AUTEURS:
    PBO Pascal Bourguignon  DSI\EI\MH\CCE
MODIFICATIONS:
    $Log: lcut.c,v $
    Revision 1.1  2003/12/04 03:46:07  pjbpjb
    Cleaned-up.
    Fix.

    Revision 1.1  1995/01/27  13:56:34  pbo
    Initial revision
BOGUES:
    9999 xxdescription_de_la_bogue.
LEGAL:
    Copyright ABEILLE VIE, 1994 - 1995
****ID-FIN************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define defaultwidth    (78)
#define bufact          (128)

    static char* rcsident="$Id: lcut.c,v 1.1 2003/12/04 03:46:07 pjbpjb Exp $";
    const char*     pname;


    int lcut(FILE* fin,FILE* fout,int width)
    {
            char*   buffer;
            char*   eolp;
            int     bufend;
            int     bufsize;
            int     bufstart;
            int     len;
            int     readcount;
            int     writtencount;


        bufsize=2*BUFSIZ*(width<bufact*BUFSIZ?bufact:(width+BUFSIZ-1)/BUFSIZ);
        buffer=malloc(bufsize+1);
        if(buffer==NULL){
            fprintf(stderr,"%s: cannot allocate %d bytes for buffer.\n",
                    pname,bufsize);
            return(3);
        }
        buffer[bufsize]='\0';
        bufstart=0;
        bufend=0;
        while(!feof(fin)){
            /* fill the buffer */
            readcount=fread(buffer+bufend,sizeof(char),bufsize-bufend,fin);
            bufend+=readcount;
            /* process the buffer */
            while(width<=bufend-bufstart){
                eolp=strchr(buffer+bufstart,'\n');
                len=bufend-bufstart;
                if((eolp!=NULL)&&(eolp-buffer<len)){
                    len=eolp-buffer+1;
                }
                while(len>width){
                    writtencount=fwrite(buffer+bufstart,sizeof(char),width,fout);
                    fputs("\n",fout);
                    bufstart+=width;
                    len-=width;
                }
            }
            /* pack the buffer */
            memcpy(buffer,buffer+bufstart,bufend-bufstart);
            bufend-=bufstart;
            bufstart=0;
        }
        if(bufend-bufstart>0){
            writtencount=fwrite(buffer+bufstart,sizeof(char),bufend-bufstart,fout);
        }
        return(0);
    }/*lcut*/

/*
fprintf(stderr,"W1.1: bufend=%4d, bufsize-bufend=%4d, readcount=%4d\n",bufend,bufsize-bufend,readcount); 
fprintf(stderr,"W2.1: width=%4d, bufstart=%4d, bufend=%4d, bufend-bufstart=%4d\n",width,bufstart,bufend,bufend-bufstart); 
fprintf(stderr,"W3.1: eolp=%p, end=%4d, width=%4d  bufstart=%4d, len=%4d\n",eolp,len,width,bufstart,len);
fprintf(stderr,"W3.2: bufstart=%4d, width=%4d, writtencount=%4d\n",bufstart,width,writtencount); 
fprintf(stderr,"W3.3: eolp=%p, end=%4d, width=%4d  bufstart=%4d, len=%4d\n",eolp,len,width,bufstart,len);
fprintf(stderr,"W1.2: width=%4d, bufstart=%4d, bufend=%4d, bufend-bufstart=%4d\n",width,bufstart,bufend,bufend-bufstart); 
*/

int main(int argc,char** argv)
{
        int     width;

    pname=argv[0];
    switch(argc){
    case 1:
        width=defaultwidth;
        break;
    case 2:
        if((1!=sscanf(argv[1],"%d",&width))||(width<=0)){
            fprintf(stderr,"%s: argument '%s' is an invalid width.\n",
                    pname,argv[1]);
            return(2);
        }
        break;
    default:
        fprintf(stderr,"%s usage:\n\t%s [width]\n",pname,pname);
        return(1);
    }
    return(lcut(stdin,stdout,width));
}
/*end lcut*/


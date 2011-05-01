/******************************************************************************
FILE:               romain.c
LANGUAGE:           ANSI-C
SYSTEM:             POSIX
USER-INTERFACE:     POSIX
DESCRIPTION
    Dumps all the roman numbers and writes which is the longest.

    (Only numbers between 1 and 4999 can be written in the roman
    numerical system).

USAGE

AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    2000-08-15 <PJB> Creation.
BUGS
LEGAL
    GPL
    Copyright Pascal J. Bourguignon 2000 - 2011

    This file is part of the romain tool.

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



typedef enum {false,true} boolean;

#define toobig  "HORS-LIMITE"

static boolean to_roman(int n,char* r)
{
    /*0101010*/
    /*0 1 2 3*/
    /*1234567*/
    const char  digits[]="IVXLCDM";
    int         u,d;
    int         j,k2;
            
    if((n<1)||(n>=5000)){
        strcpy(r,toobig);
        return(false);
    }
    for(u=1000,k2=6;k2>=0;u/=10,k2-=2){
        d=(n/u)%10;
        switch(d){
        case 5:
            (*r++)=digits[k2+1];
            break;
        case 9:
            if(k2<2){
                (*r++)=digits[k2];
                (*r++)=digits[k2+2];
            }else{
                /* digits[6] here crashes the NeXTSTEP 3.0 cc compiler. */
                (*r++)=digits[k2+1];
                (*r++)=digits[k2];
                (*r++)=digits[k2];
                (*r++)=digits[k2];
                (*r++)=digits[k2];
            }
            break;
        case 4:
            if(k2<6){
                (*r++)=digits[k2];
                (*r++)=digits[k2+1];
                break;
            }
            /*fall thru*/
        default:
            if(d>=5){
                (*r++)=digits[k2+1];
                d-=5;
            }
            for(j=1;j<=d;j++){
                (*r++)=digits[k2];
            }
            break;
        }
    }
    (*r++)='\0';
    return(true);
}/*to_roman*/

#if 0

/*
  Règles d'écriture des nombres romains :
  Les chiffres M, C, X, I ne peuvent pas être répétés plus de quatre fois.
  Les chiffres D, L, V ne peuvent pas être répétés.
  Les chiffres doivent être écris dans l'ordre : M, D, C, L, X, V, I ; 
  sauf que un I peut précéder V, X, L, ou C, et un X peut précéder L ou C.
*/

static boolean roman_chiffre(const char* r,int* i,int* n,char* erreur,
                             int chiffre,int value,int maxcount)
{
    int     m=0;
    while(r[*i]=='M'){
        (*i)++;m++;
    }
    if(m>maxcount){
        sprintf(erreur,"Il ne doit pas y avoir plus de %d '%c'.",
                maxcount,chiffre);
        return(false);
    }else{
        (*n)+=m*value;
        return(true);
    }
}/*roman_chiffre*/


#define roman_millier(r,i,n,e)      roman_chiffre(r,i,n,e,'M',1000,4)
#define roman_centaine4(r,i,n,e)    roman_chiffre(r,i,n,e,'C',100,4)
#define roman_dizaine4(r,i,n,e)     roman_chiffre(r,i,n,e,'X',10,4)
#define roman_dizaine3(r,i,n,e)     roman_chiffre(r,i,n,e,'X',10,3)
#define roman_unite3(r,i,n,e)       roman_chiffre(r,i,n,e,'I',1,3)
#define roman_unites(r,i,n,e)       roman_chiffre(r,i,n,e,'I',1,3)


static boolean roman_dizaines(const char* r,int* i,int* n,char* erreur);

static boolean roman_centaines(const char* r,int* i,int* n,char* erreur)
{
    if((r[*i]=='I')&&(r[(*i)+1]=='C')){
        (*i)+=2;
        (*n)+=99;
        return(true);
    }
    if(r[*i]=='D'){
        (*i)++;
        (*n)+=500;
    }
    if(roman_centaine4(r,i,n,erreur)){
        return(roman_dizaines(r,i,n,erreur));
    }else{
        return(false);
    }
}/*roman_centaines*/


static boolean roman_dizaines(const char* r,int* i,int* n,char* erreur)
{
    if((r[*i]=='I')&&(r[(*i)+1]=='X')){
        (*i)+=2;
        (*n)+=9;
        return(true);
    }else if((r[*i]=='I')&&(r[(*i)+1]=='L')){
        (*i)+=2;
        (*n)+=49;
        return(true);
    }else if(r[*i]=='L'){
        (*i)++;
        (*n)+=50;
        if(roman_dizaine3(r,i,n,erreur)){
            return(roman_unites(r,i,n,erreur));
        }else{
            return(false);
        }
    }else{
        if(roman_dizaine4(r,i,n,erreur)){
            return(roman_unites(r,i,n,erreur));
        }else{
            return(false);
        }
    }
}/*roman_dizaines*/
    

/*
  romain : milliers centaines .
  milliers : | M | M M | M M M | M M M M .
  centaines : centaine4 dizaines | D centaine4 dizaines | I C .
  centaine4 : | C | C C | C C C | C C C C .
  dizaines : dizaine4 unites | L dizaine3 unites | I L | I X .
  dizaine4 : dizaine3 | X L .
  dizaine3 : | X | X X | X X X .
  unites : unite4 | V unite3 .
  unite4 : unite3 | I V .
  unite3 : | I | I I | I I I .
*/

static boolean from_roman(int* n,const char* r,char* erreur)
{
    int     i=0;
    (*n)=0;
    if(roman_millier(r,&i,n,erreur)){
        if(roman_centaines(r,&i,n,erreur)){
            if(r[i]=='\0'){
                return(true);
            }else{
                sprintf(erreur,"Il y a des caractères invalides ou "
                        "des chiffres en trop : %s",r+i);
                return(false);
            }
        }else{
            return(false);
        }
    }else{
        return(false);
    }
}/*from_roman*/
#endif      

int main(void)
{
    int i,m=0,l=0,k;
    char r[64];
    for(i=0;i<=5000;i++){
        to_roman(i,r);
        k=(int)strlen(r);
        if(k>l){ m=i; l=k; }
        printf("%4d = %s\n",i,r);
    }
    printf("Le plus long est :\n");
    to_roman(m,r);
    printf("%4d = %s\n",m,r);
    return(0);
}/*main*/


/**** THE END ****/

/*
En voil’ du concret :

  B   I   L   L   G   A   T   E   S  3
 66+ 73+ 76+ 76+ 71+ 65+ 84+ 69+ 83+ 3 = 666

Pourquoi fÙt-il si important pour Microsoft de sortir son nouveau syst‹me en 1995, et surtout de ne pas arriver ’ 1996 ? 

Pour ne pas se trahir de fa€on trop ›vidente:

  W   I   N   D   O   W   S   9   6 
 87+ 73+ 78+ 68+ 79+ 87+ 83+ 57+ 54 = 666

Et parce que c'est cinq fois mieux:

  M   I   C   R   O   S   O   F   T   +   W   I   N   D   O   W   S   +  1995
 77+ 73+ 67+ 82+ 79+ 83+ 79+ 70+ 84+ 43+ 87+ 73+ 78+ 68+ 79+ 87+ 83+ 43+ 1995 = 3330

soit 5 fois 666 !
 
Essayez d'en tirer autant d'Apple, de Steve Jobs ou de MacOS!
-----------------------------------------------------
*/
#include <stdio.h>

int main(int argc,char** argv)
{
    int i,j;
    int sum=0;
    for(i=1;i<argc;i++){
        for(j=0;argv[i][j]!='\0';j++){
            printf("  %c ",argv[i][j]); 
        }
    }
    printf("\n");
    for(i=1;i<argc-1;i++){
        for(j=0;argv[i][j]!='\0';j++){
            sum+=(argv[i][j]);  
            printf("%3d+",argv[i][j]);  
        }
    }
    for(j=0;argv[i][j+1]!='\0';j++){
        sum+=(argv[i][j]);  
        printf("%3d+",argv[i][j]);  
    }
    sum+=(argv[i][j]);  
    printf("%3d = %d\n",argv[i][j],sum);    
    return(0);
}
/*
-----------------------------------------------------
*/

/*** sumascii.c                       -- 2003-12-02 13:20:19 -- pascal   ***/

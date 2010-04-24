/******************************************************************************
FILE:               ascii.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program prints ASCII tables.

    printables:
    -----------
    print the hex code                         -h -x
    print the dec code                         -d 
    print the oct code                         -o
    print the oct code                         -b
    print the name      (ETX for 0x03)         -n  (for printable = -l)
    print the notation  (^C  for 0x03)         -N  (for printable = -l)
    print the character literal                -l
    print raw (without nice formating)         -r
    print all                                  -A

    categories of characters:
    --------------------------
    control characters <32 or DEL              -c
    ascii characters between 32 and 127        -a
    control characters between 0x80 and 0x9f   -C
    iso characters between 0xa0 and 0xff       -i

    default : -a
    shortcut: -e = -a -i

AUTHOR
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    2003-11-18 <PJB> Default now always include -a when no category is present.
                     Added a binary mode.
    1991-04-30 <PJB> Creation.
    1993-11-23 <PJB> Added -C option.  
    1994-02-15 <PJB> Added RCS keywords.
    1994-07-13 <PJB> Transformed C++ comments into C comments.
    1995-09-08 <PJB> Some changes.
    1995-11-04 <PJB> Use unsigned char instead of char for array subscripts.
    2001-04-27 <PJB> Added to CVS.
    2001-04-30 <PJB> Initial entry into CVS.
    2002-08-12 <PJB> Updated for new common/makedir from MapTree.
    2003-01-07 <PJB> Cleaned-up the options and orthogonalized them.
                     Use of ctype macros to filter the characters
                     (so this may be locale dependent too).
LEGAL
    GPL
    Copyright Pascal J. Bourguignon 1991 - 2003

    This file is part of PJB ASCII UTILITIES.

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
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <locale.h>
#include <stdlib.h>    


    static void usage(char* name,const char* options)
    {
        printf("%s usage:\n\t%s %s\n",name,name,options);
    }/*usage;*/

    
    static char* byte_to_binary(int byte)
    {
        static char binary[10];
        int i;
        binary[8]='\0';
        for(i=7;0<=i;i--){
            binary[i]=(byte&1)?'1':'0';
            byte>>=1;
        }
        return(binary);
    }/*byte_to_binary*/


    static const char*    asciinames[]={
        "NUL ","SOH ","STX ","ETX ","EOT ","ENQ ","ACK ","BEL ",
        "BS  ","TAB ","LF  ","VT  ","FF  ","CR  ","SO  ","SI  ",
        "DLE ","DC1 ","DC2 ","DC3 ","DC4 ","NAK ","SYN ","ETB ",
        "CAN ","EM  ","SUB ","ESC ","FS  ","GS  ","RS  ","US  ",
        "SP  ","!   ","\"   ","#   ","$   ","%   ","&   ","'   ",
        "(   ",")   ","*   ","+   ",",   ","-   ",".   ","/   ",
        "0   ","1   ","2   ","3   ","4   ","5   ","6   ","7   ",
        "8   ","9   ",":   ",";   ","<   ","=   ",">   ","?   ",
        "@   ","A   ","B   ","C   ","D   ","E   ","F   ","G   ",
        "H   ","I   ","J   ","K   ","L   ","M   ","N   ","O   ",
        "P   ","Q   ","R   ","S   ","T   ","U   ","V   ","W   ",
        "X   ","Y   ","Z   ","[   ","\\   ","]   ","^   ","_   ",
        "`   ","a   ","b   ","c   ","d   ","e   ","f   ","g   ",
        "h   ","i   ","j   ","k   ","l   ","m   ","n   ","o   ",
        "p   ","q   ","r   ","s   ","t   ","u   ","v   ","w   ",
        "x   ","y   ","z   ","{   ","|   ","}   ","~   ","DEL ",
        "PAD ","HOP ","BPH ","NBH ","IND ","NEL ","SSA ","ESA ",
        "HTS ","HTJ ","VTS ","PLD ","PLU ","RI  ","SS2 ","SS3 ",
        "DCS ","PU1 ","PU2 ","STS ","CCH ","MW  ","SPA ","EPA ",
        "SOS ","SGCI","SCI ","CSI ","ST  ","OSC ","PM  ","APC ",
        "NBSP","¡   ","¢   ","£   ","¤   ","¥   ","¦   ","§   ",
        "¨   ","©   ","ª   ","«   ","¬   ","­   ","®   ","¯   ",
        "°   ","±   ","²   ","³   ","´   ","µ   ","¶   ","·   ",
        "¸   ","¹   ","º   ","»   ","¼   ","½   ","¾   ","¿   ",
        "À   ","Á   ","Â   ","Ã   ","Ä   ","Å   ","Æ   ","Ç   ",
        "È   ","É   ","Ê   ","Ë   ","Ì   ","Í   ","Î   ","Ï   ",
        "Ð   ","Ñ   ","Ò   ","Ó   ","Ô   ","Õ   ","Ö   ","×   ",
        "Ø   ","Ù   ","Ú   ","Û   ","Ü   ","Ý   ","Þ   ","ß   ",
        "à   ","á   ","â   ","ã   ","ä   ","å   ","æ   ","ç   ",
        "è   ","é   ","ê   ","ë   ","ì   ","í   ","î   ","ï   ",
        "ð   ","ñ   ","ò   ","ó   ","ô   ","õ   ","ö   ","÷   ",
        "ø   ","ù   ","ú   ","û   ","ü   ","ý   ","þ   ","ÿ   "};

/*

 00 0x00 NUL Null
 01 0x01 SOH Start of Heading
 02 0x02 STX Start of Text
 03 0x03 ETX End of Text
 04 0x04 EOT End of Transmission
 05 0x05 ENQ Enquiry
 06 0x06 ACK Acknowledge
 07 0x07 BEL Bell
 08 0x08 BS  Backspace
 09 0x09 HT  Horizontal Tab
 10 0x0A LF  Line Feed
 11 0x0B VT  Vertical Tab
 12 0x0C FF  Form Feed
 13 0x0D CR  Carriage Return
 14 0x0E SO  Shift Out
 15 0x0F SI  Shift In
 16 0x10 DLE Data Link Escape
 17 0x11 DC1 Device Control 1
 18 0x12 DC2 Device Control 2
 19 0x13 DC3 Device Control 3
 20 0x14 DC4 Device Control 4
 21 0x15 NAK Negative Acknowledge
 22 0x16 SYN Synchronous Idle
 23 0x17 ETB End of Transmission Block
 24 0x18 CAN Cancel
 25 0x19 EM  End of Medium
 26 0x1A SUB Substitute
 27 0x1B ESC Escape
 28 0x1C FS  File Separator
 29 0x1D GS  Group Separator
 30 0x1E RS  Record Separator
 31 0x1F US  Unit Separator
127 0x7F DEL Rubout/Delete

The compatible 8 bit ISO-8859-1 additionally maps the 32 codes from position 128 through 159, which are unused in ISO/IEC 8859-1, to control characters.
Control Characters in ISO-8859-* dec hex abbr Name

128 0x80 PAD  Padding Character
129 0x81 HOP  High Octet Preset
130 0x82 BPH  Break Permitted Here
131 0x83 NBH  No Break Here
132 0x84 IND  Index
133 0x85 NEL  Next Line
134 0x86 SSA  Start of Selected Area
135 0x87 ESA  End of Selected Area
136 0x88 HTS  Horizontal Tab Set
137 0x89 HTJ  Horizontal Tab Justified
138 0x8A VTS  Vertical Tab Set
139 0x8B PLD  Partial Line Forward
140 0x8C PLU  Partial Line Backward
141 0x8D RI   Reverse Line Feed
142 0x8E SS2  Single-Shift 2
143 0x8F SS3  Single-Shift 3
144 0x90 DCS  Device Control String
145 0x91 PU1  Private Use 1
146 0x92 PU2  Private Use 2
147 0x93 STS  Set Transmit State
148 0x94 CCH  Cancel Character
149 0x95 MW   Message Waiting
150 0x96 SPA  Start of Protected Area
151 0x97 EPA  End of Protected Area
152 0x98 SOS  Start of String
153 0x99 SGCI Single Graphic Char Intro
154 0x9A SCI  Single Char Intro
155 0x9B CSI  Control Sequence Intro
156 0x9C ST   String Terminator
157 0x9D OSC  Os Command
158 0x9E PM   Private Message
159 0x9F APC  App Program Command

       240   160   A0           NO-BREAK SPACE
       241   161   A1     ¡     INVERTED EXCLAMATION MARK
       242   162   A2     ¢     CENT SIGN
       243   163   A3     £     POUND SIGN
       244   164   A4     ¤     CURRENCY SIGN
       245   165   A5     ¥     YEN SIGN
       246   166   A6     ¦     BROKEN BAR
       247   167   A7     §     SECTION SIGN
       250   168   A8     ¨     DIAERESIS
       251   169   A9     ©     COPYRIGHT SIGN
       252   170   AA     ª     FEMININE ORDINAL INDICATOR
       253   171   AB     «     LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
       254   172   AC     ¬     NOT SIGN
       255   173   AD     ­     SOFT HYPHEN
       256   174   AE     ®     REGISTERED SIGN
       257   175   AF     ¯     MACRON
       260   176   B0     °     DEGREE SIGN
       261   177   B1     ±     PLUS-MINUS SIGN
       262   178   B2     ²     SUPERSCRIPT TWO
       263   179   B3     ³     SUPERSCRIPT THREE
       264   180   B4     ´     ACUTE ACCENT
       265   181   B5     µ     MICRO SIGN
       266   182   B6     ¶     PILCROW SIGN
       267   183   B7     ·     MIDDLE DOT
       270   184   B8     ¸     CEDILLA
       271   185   B9     ¹     SUPERSCRIPT ONE
       272   186   BA     º     MASCULINE ORDINAL INDICATOR
       273   187   BB     »     RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
       274   188   BC     ¼     VULGAR FRACTION ONE QUARTER
       275   189   BD     ½     VULGAR FRACTION ONE HALF
       276   190   BE     ¾     VULGAR FRACTION THREE QUARTERS
       277   191   BF     ¿     INVERTED QUESTION MARK
       300   192   C0     À     LATIN CAPITAL LETTER A WITH GRAVE
       301   193   C1     Á     LATIN CAPITAL LETTER A WITH ACUTE
       302   194   C2     Â     LATIN CAPITAL LETTER A WITH CIRCUMFLEX
       303   195   C3     Ã     LATIN CAPITAL LETTER A WITH TILDE
       304   196   C4     Ä     LATIN CAPITAL LETTER A WITH DIAERESIS
       305   197   C5     Å     LATIN CAPITAL LETTER A WITH RING ABOVE
       306   198   C6     Æ     LATIN CAPITAL LETTER AE
       307   199   C7     Ç     LATIN CAPITAL LETTER C WITH CEDILLA
       310   200   C8     È     LATIN CAPITAL LETTER E WITH GRAVE
       311   201   C9     É     LATIN CAPITAL LETTER E WITH ACUTE
       312   202   CA     Ê     LATIN CAPITAL LETTER E WITH CIRCUMFLEX
       313   203   CB     Ë     LATIN CAPITAL LETTER E WITH DIAERESIS
       314   204   CC     Ì     LATIN CAPITAL LETTER I WITH GRAVE
       315   205   CD     Í     LATIN CAPITAL LETTER I WITH ACUTE
       316   206   CE     Î     LATIN CAPITAL LETTER I WITH CIRCUMFLEX
       317   207   CF     Ï     LATIN CAPITAL LETTER I WITH DIAERESIS
       320   208   D0     Ð     LATIN CAPITAL LETTER ETH
       321   209   D1     Ñ     LATIN CAPITAL LETTER N WITH TILDE
       322   210   D2     Ò     LATIN CAPITAL LETTER O WITH GRAVE
       323   211   D3     Ó     LATIN CAPITAL LETTER O WITH ACUTE
       324   212   D4     Ô     LATIN CAPITAL LETTER O WITH CIRCUMFLEX
       325   213   D5     Õ     LATIN CAPITAL LETTER O WITH TILDE
       326   214   D6     Ö     LATIN CAPITAL LETTER O WITH DIAERESIS
       327   215   D7     ×     MULTIPLICATION SIGN
       330   216   D8     Ø     LATIN CAPITAL LETTER O WITH STROKE
       331   217   D9     Ù     LATIN CAPITAL LETTER U WITH GRAVE
       332   218   DA     Ú     LATIN CAPITAL LETTER U WITH ACUTE
       333   219   DB     Û     LATIN CAPITAL LETTER U WITH CIRCUMFLEX
       334   220   DC     Ü     LATIN CAPITAL LETTER U WITH DIAERESIS
       335   221   DD     Ý     LATIN CAPITAL LETTER Y WITH ACUTE
       336   222   DE     Þ     LATIN CAPITAL LETTER THORN
       337   223   DF     ß     LATIN SMALL LETTER SHARP S
       340   224   E0     à     LATIN SMALL LETTER A WITH GRAVE
       341   225   E1     á     LATIN SMALL LETTER A WITH ACUTE
       342   226   E2     â     LATIN SMALL LETTER A WITH CIRCUMFLEX
       343   227   E3     ã     LATIN SMALL LETTER A WITH TILDE
       344   228   E4     ä     LATIN SMALL LETTER A WITH DIAERESIS
       345   229   E5     å     LATIN SMALL LETTER A WITH RING ABOVE
       346   230   E6     æ     LATIN SMALL LETTER AE
       347   231   E7     ç     LATIN SMALL LETTER C WITH CEDILLA
       350   232   E8     è     LATIN SMALL LETTER E WITH GRAVE
       351   233   E9     é     LATIN SMALL LETTER E WITH ACUTE
       352   234   EA     ê     LATIN SMALL LETTER E WITH CIRCUMFLEX
       353   235   EB     ë     LATIN SMALL LETTER E WITH DIAERESIS
       354   236   EC     ì     LATIN SMALL LETTER I WITH GRAVE
       355   237   ED     í     LATIN SMALL LETTER I WITH ACUTE
       356   238   EE     î     LATIN SMALL LETTER I WITH CIRCUMFLEX
       357   239   EF     ï     LATIN SMALL LETTER I WITH DIAERESIS
       360   240   F0     ð     LATIN SMALL LETTER ETH
       361   241   F1     ñ     LATIN SMALL LETTER N WITH TILDE
       362   242   F2     ò     LATIN SMALL LETTER O WITH GRAVE
       363   243   F3     ó     LATIN SMALL LETTER O WITH ACUTE
       364   244   F4     ô     LATIN SMALL LETTER O WITH CIRCUMFLEX
       365   245   F5     õ     LATIN SMALL LETTER O WITH TILDE
       366   246   F6     ö     LATIN SMALL LETTER O WITH DIAERESIS
       367   247   F7     ÷     DIVISION SIGN
       370   248   F8     ø     LATIN SMALL LETTER O WITH STROKE
       371   249   F9     ù     LATIN SMALL LETTER U WITH GRAVE
       372   250   FA     ú     LATIN SMALL LETTER U WITH ACUTE
       373   251   FB     û     LATIN SMALL LETTER U WITH CIRCUMFLEX
       374   252   FC     ü     LATIN SMALL LETTER U WITH DIAERESIS
       375   253   FD     ý     LATIN SMALL LETTER Y WITH ACUTE
       376   254   FE     þ     LATIN SMALL LETTER THORN
       377   255   FF     ÿ     LATIN SMALL LETTER Y WITH DIAERESIS
*/


    static void print_char(int length,int hex,int dec,int oct,int bin,int name,
                           int notation,int literal,int all_chars,int i)
    {
        if(hex){ printf("%02x ",i); }
        if(dec){ printf("%3d ",i); }
        if(oct){ printf("%03o ",i); }
        if(bin){ printf("%8s ",byte_to_binary(i)); }
        if(name){ 
            if(all_chars||isgraph(i)){ 
                printf(" %c  ",i);  
                /* NOTE: iso-latin-1 char have names like Aacute... */
            }else{
                printf("%3s ",asciinames[i&0xff]);
            }
        }
        if(notation){
            if(iscntrl(i)){
                printf("^%c ",'@'+i);
            }else{
                printf(" %c ",i);
            }
        }
        if(literal){
            printf("%c",i);
        }
    }/*print_char*/


int main(int argc,char** argv)
{
    int             i;
    int             length;
    int             modulo;

    /*
     (generate-options '((-h print_hex) (-x print_hex) 
        (-d print_dec) (-o print_oct) (-b print_bin)
        (-n print_name) (-N print_notation) (-l print_literal) (-r print_raw)
        (-c controls) (-a ascii) (-C iso_controls) (-i iso) (-e ascii iso)
        (-A all_chars))
        '())



    */

    const char usage_options[]="[-h|-x] [-d] [-o] [-b] [-n] [-N] [-l] [-r] [-c] [-a] [-C] [-i] [-e] [-A] ";

    int         all_chars       =0;
    int         ascii           =0;
    int         controls        =0;
    int         iso             =0;
    int         iso_controls    =0;
    int         print_bin       =0;
    int         print_dec       =0;
    int         print_hex       =0;
    int         print_literal   =0;
    int         print_name      =0;
    int         print_notation  =0;
    int         print_oct       =0;
    int         print_raw       =0;

    for(i=1;i<argc;i++){
        if(strcmp(argv[i],"-h")==0){          print_hex=1;
        }else if(strcmp(argv[i],"-x")==0){    print_hex=1;
        }else if(strcmp(argv[i],"-d")==0){    print_dec=1;
        }else if(strcmp(argv[i],"-o")==0){    print_oct=1;
        }else if(strcmp(argv[i],"-b")==0){    print_bin=1;
        }else if(strcmp(argv[i],"-n")==0){    print_name=1;
        }else if(strcmp(argv[i],"-N")==0){    print_notation=1;
        }else if(strcmp(argv[i],"-l")==0){    print_literal=1;
        }else if(strcmp(argv[i],"-r")==0){    print_raw=1;
        }else if(strcmp(argv[i],"-c")==0){    controls=1;
        }else if(strcmp(argv[i],"-a")==0){    ascii=1;
        }else if(strcmp(argv[i],"-C")==0){    iso_controls=1;
        }else if(strcmp(argv[i],"-i")==0){    iso=1;
        }else if(strcmp(argv[i],"-e")==0){
            ascii=1;
            iso=1;
        }else if(strcmp(argv[i],"-A")==0){    all_chars=1;
        }else{
            usage(argv[0],usage_options);
            exit(1);
        }
    }

    if(argc<=1){ /* if no option is given then default is -a */
        ascii=1; 
        print_name=1;
    }
    if(!(controls||ascii||iso_controls||iso||all_chars)){
        ascii=1;
    }

    if(print_hex+print_dec+print_oct+print_name
       +print_notation+print_literal==0){
        if(all_chars){
            print_raw=1;
        }else{
            print_name=1;
        }
    }
    setlocale(LC_ALL, "" );

    if(print_raw){
        for(i=0;i<256;i++){
            if(all_chars
               ||(controls && (i<128) && iscntrl(i))
               ||(iso_controls && (128<=i) && iscntrl(i))
               ||(ascii && (i<128) && isprint(i))
               ||(iso && (128<=i) && isprint(i))){
                printf("%c",i);
            }
        }
        exit(0);
    }


    length=0;
    if(print_hex){      length+=3; }
    if(print_dec){      length+=4; }
    if(print_oct){      length+=4; }
    if(print_bin){      length+=9; }
    if(print_name){     length+=4; }
    if(print_notation){ length+=3; }
    if(print_literal){  length+=2; }
    if(16*length<=80){
        modulo=16;
    }else if(8*length<=80){
        modulo=8;
    }else if(4*length<=80){
        modulo=4;
    }else{
        modulo=2;
    }

    for(i=0;i<256;i++){
        if((i%modulo)==0){
            printf("\n");
        }
        if(all_chars
           ||(controls && (i<128) && iscntrl(i))
           ||(iso_controls && (128<=i) && iscntrl(i))
           ||(ascii && (i<128) && isprint(i))
           ||(iso && (128<=i) && isprint(i))){
            print_char(length,print_hex,print_dec,print_oct,print_bin,
                       print_name,print_notation,print_literal,all_chars,i);
        }else{
            printf("%*s",length,"");
        }
    }
    printf("\n");
    return(0);
}/*main*/



/*** ascii.c                          -- 2004-03-16 15:26:30 -- pascal   ***/

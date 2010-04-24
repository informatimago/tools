/******************************************************************************
FILE:               newmoon.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    (moon)
    Print the phase of the moon, for your safety and convenience.

    (newmoon)
    This program writes a date (to stdout) according to the French
    Revolutionary Calendar.  If no argument is specified, it calls a
    "ctime" function to use today's date.  Otherwise, it may be invoked
    with 3 arguments -- month, day, year (4-digit).
    
    The French Revolutionary Calendar featured 12 months of 30 days each
    plus 5 intercalary days (6 in leap years).  The calendar was adopted
    in October 1793, retroactive to 22 Sept 1792.  This date became
    1 Vendemiaire, An I de la Revolution.
    
    Note that the Convention adopted years of the form 4n+3 as leap years,
    not the 4n of the Gregorian calendar.  (These were marked by a 6th "jour
    sans-cullotide" at the end of the year.)  There is no reference I know
    of to any further correxions, such as the Gregorian calendar's non-leap
    centuries in years of the form 400n.  The calendar did not last past
    year XIII, however, so the point is moot.  Napoleon reinstated the
    Gregorian calendar on 1 January 1806.
    
    Most Revolutionary dates I have encountered in historical literature
    are translated correctly by this program.  Indeed, the few differences
    I have found are clearly arithmetic errors by the historian.   Any valid
    discrepancies should be due to irregularities in the calendars extant
    in 18th Century Europe.
USAGE
    newmoon [ month day year ]
AUTHORS
    <???>
    <BNH>
    <JPB> Jean Pierre Baccache
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    19??-??-?? <???> Creation. (moon.c)
    1983-08-28 <BNH> This version stolen from MIT-AMG and translated 
                     from the PL/I version (and a previously translated 
                     FORTRAN one) into C. (newmoon.c)
    1985-06-26 <???> Hacked to assume base new moon at 1985-03-21 11:59 GMT, 
                     and read terminal info by halazar and dead
    19??-??-?? <JPB> Adapted on XENIX sco SystemV.
    1993-10-21 <PJB> Added getmessage routine.
BUGS
LEGAL
    Public Domain
    
    This software is in Public Domain.
    You're free to do with it as you please.
******************************************************************************/


#include <stdio.h>
#include <math.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <curses.h>
#include <term.h>
#include <time.h>

#if defined(AIX) || defined(linux) || defined(MACOSX)
#include <unistd.h>
#endif

#if defined(NeXT)
#include <libc.h>
#endif

#if defined(UNIX)
#include <pwd.h>
#endif

/* BEGIN newmoon */

    static char* getmessage(void)
        /*
            RETURN:     a string used to draw the moon.
        */
    {
        static char buffer[1024];
#ifdef UNIX
        struct passwd* p=getpwuid(getuid());
        strcpy(buffer,p->pw_name);
        return(buffer);
#else
        strcpy(buffer,"MOON");
        return(buffer);
#endif
    }/* getmessage; */


#ifndef TRUE
#define TRUE     1
#define FALSE    0
#endif
#define MAXNUMS  7
#define MAXSIZE  20
#define FEB28    59
#define CALSTART 1792
#define JDAY1VEN 263
#define FDAY1JAN 102

static const char *numeral = "MDCLXVI";
static int romandiv[MAXNUMS] = {1000,500,100,50,10,5,1};
char buf[MAXSIZE];


    static const char *roman(int number) {

        int i, idx, digit;
        char *ptr;

        if (number < 1) return("????");
        ptr = buf;
        while(number >= romandiv[0]) {
            number -= romandiv[0];
            *ptr++ = numeral[0];
        }
        for(idx=2; idx<MAXNUMS; idx+=2) {
            digit = number / romandiv[idx];
            number -= digit * romandiv[idx];
            if (digit == 9) {
                *ptr++ = numeral[idx];
                *ptr++ = numeral[idx-2];
            }
            else if (digit >= 5) {
                *ptr++ = numeral[idx-1];
                for (i=5;i<digit;i++) *ptr++ = numeral[idx];
            }
            else if (digit == 4) {
                *ptr++ = numeral[idx];
                *ptr++ = numeral[idx-1];
            }
            else for(i=0;i<digit;i++) *ptr++ = numeral[idx];
        }
        *ptr = '\0';
        return(buf);
    }


    static int notvalid(int year,int julianday) {

        if (year > CALSTART) return(FALSE);
        if (year < CALSTART) return(TRUE);
        if (julianday > JDAY1VEN+1) return(FALSE);
        return(TRUE);
    }


    static int mlen[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

    static const char *mname[] = {
        "Jour Sans-culottide",
        "Vendemiaire",
        "Brumaire",
        "Frimaire",
        "Nivose",
        "Pluviose",
        "Ventose",
        "Germinal",
        "Floreal",
        "Prairial",
        "Messidor",
        "Thermidor",
        "Fructidor"
    };


    static void french_date(void)
    {
        int argc = 1;
        char **argv=0 ;
        long secs;
        int i, fr_dno, fr_day, fr_mo, fr_yr, jyr;
        int jday = 0;
        int jleap = FALSE;
        int fleap = FALSE;
        struct tm *tbuf;
        const char *filler;

        if (argc > 1 && argc != 4) {
            printf("Usage:  No arguments for today's date,");
            printf(" or '%s <mo> <da> <yr>'.\n", argv[0]);
            exit(1);
        }
        else if (argc == 1) {
            secs = time((long *)0);
            tbuf = localtime(&secs);
            jyr = tbuf->tm_year + 1900;
            jday = tbuf->tm_yday;
        }
        else {
            for(i=0; i<atoi(argv[1])-1; i++) jday += mlen[i];
            jyr = atoi(argv[3]);
            jday += (atoi(argv[2]) - (jyr%4 || !(jyr%400) || jday<=FEB28));
            if (notvalid(jyr, jday)) {
                printf("Date avant la revolution!\n");
                exit(1);
            }
        }
        if (jyr%4==0 && jyr%400)
            jleap = TRUE;
        else if (jyr%4==3)
            fleap = TRUE;
        fr_yr = jyr - CALSTART + (jday > (JDAY1VEN + fleap + jleap));
        fr_dno = (jday + FDAY1JAN - jleap) % (365 + fleap);
        if (!fr_dno) fr_dno = 365 + fleap;
        fr_mo = fr_dno/30 + (fr_dno%30!=0);
        if (fr_mo == 13) fr_mo = 0;
        fr_day = fr_dno % 30;
        if (!fr_day) fr_day = 30;
        if (!fr_mo) {
            if (fr_day == 1)
                filler = "ier ";
            else
                filler = "ieme ";
        }
        else
            filler = " ";
        printf("%d%s%s An %s\n", fr_day, filler, mname[fr_mo], roman(fr_yr));
    }

/* END newmoon */






#define BASEDAYS    79  /* Number of days since Jan 1 */    
#define BASEHOURS   11
#define BASEMINS    59
#define BASEYEAR    85


/* 
 *  POM - Phase-Of-Moon
 *
 *      If you can figure out what this routine does, then you deserve
 *      to be working on it, sucker!  (I just met her...)
 *      
 *      Here are two hints:
 *      (1)  the base time is 13:23 10/1/78
 *      (1a) the new base time is 15:19 GMT 2/5/85 
 *      (2)  the original was written to express everything in seconds,
 *           and now it uses minutes instead.
 */

    static void pom(double *fraction,
             int *oday, int* ohour, int* ominute,    /* offsets from the Nth quarter of... */
             char *string)
    {
        long        tick;
        struct tm   *tm;
        int         julian, year, hour, minute;
        long        base, offset, difference;
        long        phaselength, phase, hourdiff;

        static const char *phrases[] = {
            "la nouvelle lune.",
            "le premier quart de lune.",
            "la pleine lune.",
            "le dernier quart de lune."
        };

        tick = time(0);
        tm = gmtime(&tick);
        julian = tm->tm_yday;
        year = tm->tm_year-BASEYEAR;
        hour = tm->tm_hour;
        minute = tm->tm_min;

        base = (long)( (BASEDAYS*24 + BASEHOURS)*60 + BASEMINS );
        offset = (((long)year*365+(long)julian)*24+(long)hour)*60+(long)minute;
        difference = offset-base;

        phaselength = (double)( (29*24 + 12)*60 + 44.05 );
        phase = difference-difference/phaselength*phaselength;
        *fraction = (double)phase / (double)phaselength;
        phaselength = phaselength/4;
        phase = phase/phaselength;
        strcpy(string, phrases[phase]);
        difference = difference-difference/phaselength*phaselength;
        hourdiff = difference/60;
        *ominute = difference-hourdiff*60;
        *oday = hourdiff/24;
        *ohour = hourdiff-*oday*24;
    } /* pom */




/* PWRITE - Write out the phase in words and numbers */

    static void write_phase(int day,int hour,int minute,
                       char *string)
    {
        if (day == 0 && hour == 0 && minute == 0) {
            printf("C'est exactment a %s",string);
            return;
        }
        if (day != 0) {
            printf("%d jour", day);
            if (day != 1) printf("s");
            if (hour != 0 || minute != 0) printf(", ");
        }
        if (hour != 0) {
            printf("%d heure", hour);
            if (hour != 1) printf("s");
            if (minute != 0) printf(", ");
        }
        if (minute != 0) {
            printf("%d minute",minute);
            if (minute != 1) printf("s");
        }
        printf(" depuis %s", string);
    } /* write_phase */



    static int charpos(
        double x,
        double xscale,
        int xoffset)
    {
        int i;

        i = x*xscale+0.5;
        i = i+xoffset;
        return(i<1 ? 1 : i);
    }/* charpos */


/* PDRAW - Draw the lunar phase in a shaded moon representation */

    static void pdraw(double phase,char *name)
    {
#define PI      3.14159
#define LOPTHT      5   /* login prompt height  */
#define FULLMOON    0.5 /* the new moon is 0.0 */
#define FUZZ        0.03    /* how far off we must be from an even 0.0 */

        int i=0;
        int namemod;    /* lgth of name string, used for mod arithmetic to get char */
        int xmin;   /* x position of leftmost edge of moon (not line) */
        int xoffset;    /* character offset */
        double moonhgt; /* number of lines in moon */
        double cheight; /* character height */
        double xscale;  /* x stretch factor */
        double y;
        double squisher, horizon, terminator, screenaspect;
        double left, right;
        char buffer[256];
        char *leterm;
        char dummy[1024];

        if((leterm=(char *)getenv("TERM")) == NULL){
            fprintf(stderr,
                    "Environment variable TERM not defined !");
            exit(1);
        }
        if ((tgetent(dummy, leterm)) != 1){
            fprintf(stderr,
                    "Unknow terminal type %s... To bad !",
                    leterm);
            exit(1);
        }
        initscr();
        clear();
        refresh();
        namemod = strlen(name);

        /* 3 is # of lines used by text under moon */
        moonhgt = LINES - LOPTHT - 3;   

        /* Should be 2 to 3 for a vt240, but this looks better */
        if(strcmp(leterm,"twist")==0)
            screenaspect = 1.05;
        else    screenaspect = 1.95/3.0;    

        /* Make square pixels */
        xscale = moonhgt*screenaspect*COLS/LINES/2.0;

        /* Circle of radius 1 */
        xoffset = COLS/2;
        cheight = 2.0/moonhgt;  

        /* These 3 lines just align name on left edge of moon (yeh, sure)*/
        for (y=1.0-cheight/2.0; y>-1.0 && y>fabs(y-cheight); y=y-cheight);
        horizon = sqrt(1.0-y*y);
        xmin = charpos(-horizon,xscale,xoffset);

        if (phase<FULLMOON) squisher = cos(2.0*PI*phase);
        else squisher = cos(2.0*PI*(phase-FULLMOON));

        for (y=1.0-cheight/2.0; y>-1.0; y=y-cheight) {
            for (i=0; i<256; buffer[i++]=' ');
            horizon = sqrt(1.0-y*y);
            i=charpos(horizon,xscale,xoffset);
            buffer[i] = name[(i-xmin)%namemod];
            i=charpos(-horizon,xscale,xoffset);
            buffer[i] = name[(i-xmin)%namemod];
            terminator = horizon*squisher;
            if (phase<FULLMOON) {
                left = terminator;
                right = horizon;
            }
            else {
                left = -horizon;
                right = terminator;
            }
            if (phase>FUZZ && phase<(1.0-FUZZ))
                for (i=charpos(left,xscale,xoffset);
                     i<=charpos(right,xscale,xoffset);
                     buffer[i]=name[(i-xmin)%namemod],i++);
            buffer[charpos(horizon,xscale,xoffset)+1] = '\0';
            printf(" %s\n",buffer);
        }
    }/* pdraw */



/* BEGIN moon */
/*
 *  moon.C - Print the phase of the moon, for your safety and convenience.
 *
 *  This version stolen from MIT-AMG and translated from the PL/I
 *  version (and a previously translated FORTRAN one) into C on
 *  8/28/83 by BNH.
 *
 *  Hacked to assume base new moon at 3/21/85 11:59 GMT, and
 *  read terminal info by halazar and dead  6/26/85
 */


    int main(int argc, char *argv[])
    {
        int day, hour, minute;
        double  fraction;
        char    string[32];
        char    buffer[512];
        char*   MSG;

        MSG=getmessage();
        pom(&fraction, &day, &hour, &minute, string);
        if (argc == 1)
            pdraw(fraction, MSG);
        else if (argc == 2)
            pdraw(fraction, argv[1]);
        else {
            int i;

            strcpy (buffer, argv[1]);

            strcat(buffer, " ");
            for (i=2; i<argc; i++) {
                strcat(buffer, argv[i]);
                strcat(buffer, " ");
            }
            pdraw(fraction, buffer);
        }
        printf("\n\n");
        write_phase(day, hour, minute, string);
        printf("\n");
        printf("nous sommes le ") ;
        french_date() ;
        printf("\n");
        return(0);
    } /* main */


/*** newmoon.c                        -- 2003-12-02 12:18:32 -- pascal   ***/

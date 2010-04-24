/******************************************************************************
FILE:               beep.h
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This file prints an ASCII_BEL char to stdout, which should beep at the
    terminal.
USAGE
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1992/??/?? <PJB> Creation.
BUGS
LEGAL
    Public Domain.
******************************************************************************/
#include <stdio.h>
int main(void)
{
    printf("%c",7);
    return(0);
}

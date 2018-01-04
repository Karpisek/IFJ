/**
 *  Fakutla informacnich technologii, Vysoke uceni technicke v Brne
 *	Projekt IFJ
 *  Implementace prekladace imperativniho jazyka IFJ17
 *
 *  Tym:    Jakub Trubka        (xtrubk00)
 *          Roman Zajic         (xzajic16)
 *          Miroslav Karpisek   (xkarpi05)
 *          Jakub Samek         (xsamek07)
 *
 *  Soubor: error.c
 *  Autori: Miroslav Karpisek
 *          Jakub Trubka
 *
 *	Datum:  20-10-2017
 *  Uprava: 03-12-2017
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "error.h"

int line = 1, column = 0;

cString lineBuffer;

//warning message
bool warningMsg(int columnToken, const char *fmt, ...){

    char *messageHead = lineBuffer->str;

    //zbaveni mezer
    while(*messageHead == ' ' && *messageHead != '\0') {
        messageHead++;
        columnToken--;
    }

    char memHelper;

    memHelper = lineBuffer->str[lineBuffer->actSize - 1];
    lineBuffer->str[lineBuffer->actSize - 1] = '\0';

    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "\n");
    fprintf(stderr, BOLT);
    fprintf(stderr, CYEL);
    fprintf(stderr,"WARRNING: %s:%d:%d:\n\t", "input", line, columnToken);
    fprintf(stderr, DEF);
    fprintf(stderr, CYEL);
    vfprintf(stderr,fmt, args );
    fprintf(stderr, DEF);
    fprintf(stderr, "\n");

    fprintf(stderr, SHADOW);
    fprintf(stderr, BOLT);
    fprintf(stderr, CYEL);
    fprintf(stderr, "      >");
    fprintf(stderr, DEF);

    fprintf(stderr, SHADOW);
    fprintf(stderr, "\t%s\n\t", messageHead);

    if(columnToken >= 0) {
        for(int i = 0; i < columnToken; i++) {
            fprintf(stderr, " ");
        }

        fprintf(stderr, DEF);
        fprintf(stderr, BOLT);
        fprintf(stderr, CYEL);
        fprintf(stderr, "^");
    }


    fprintf(stderr, DEF);
    fprintf(stderr, "\n\n");
    va_end(args);

    lineBuffer->str[lineBuffer->actSize - 1] = memHelper;

    return true;
}

//error message
bool errorMsg(int errCode, int columnToken, const char *fmt, ...){

    char *messageHead = lineBuffer->str;

    //zbaveni mezer
    while(*messageHead == ' ' && *messageHead != '\0') {
        messageHead++;
        columnToken--;
    }


    lineBuffer->str[lineBuffer->actSize - 1] = '\0';

    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "\n");
    fprintf(stderr, BOLT);
    fprintf(stderr, CRED);
    fprintf(stderr, "ERROR: %s:%d:%d:\n\t", "input", line, columnToken);
    fprintf(stderr, DEF);
    fprintf(stderr, CRED);
    vfprintf(stderr, fmt, args );
    fprintf(stderr, DEF);
    fprintf(stderr, "\n");

    fprintf(stderr, SHADOW);
    fprintf(stderr, BOLT);
    fprintf(stderr, CRED);
    fprintf(stderr, "      >");
    fprintf(stderr, DEF);

    fprintf(stderr, SHADOW);
    fprintf(stderr, "\t%s\n\t", messageHead);

    if(columnToken >= 0) {
        for(int i = 0; i < columnToken; i++) {
            fprintf(stderr, " ");
        }

        fprintf(stderr, DEF);
        fprintf(stderr, BOLT);
        fprintf(stderr, CRED);
        fprintf(stderr, "^");
    }



    fprintf(stderr, DEF);
    fprintf(stderr, "\n\n");
    va_end(args);
    //TODO pred volanim je potreba zavolat funkci ktera vycisti alokovanou pamet!!!!
    cleanUp();

    exit(errCode);
    return false;
}

//error message
bool error(int errCode, const char *fmt, ...){

    va_list args;
    va_start(args, fmt);
    va_start(args, fmt);
    fprintf(stderr, "\n");
    fprintf(stderr, BOLT);
    fprintf(stderr, CRED);
    fprintf(stderr, "ERROR:\n\t");
    fprintf(stderr, DEF);
    fprintf(stderr, CRED);
    vfprintf(stderr, fmt, args );
    fprintf(stderr, DEF);
    fprintf(stderr, "\n");
    va_end(args);



    //TODO pred volanim je potreba zavolat funkci ktera vycisti alokovanou pamet!!!!
    cleanUp();

    exit(errCode);

    return false;
}

void addLine() {
    line++;
    column = 0;
}

void addColumn() {
    column++;
}

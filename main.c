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
 *  Soubor: parser.h
 *  Autori: Jakub Samek
 *          Miroslav Karpisek
 *          Jakub Trubka
 *          Roman Zajic
 *
 *	Datum:  05-10-2017
 *  Uprava: 20-11-2017
 */

#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int main(int argc, char** argv) {

    (void)argv;
    if(argc != 1) {
        error(ERROR_INTER, "Neplatny pocet parametru, zadano %d, ocekavano 0", argc - 1);
    }
    parse();
    return 0;
}

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
 *  Soubor: custom_string.c
 *  Autori: Miroslav Karpisek
 *          Roman Zajic
 *          Jakub Trubka
 *
 *	Datum:  21-10-2017
 *  Uprava: 05-12-2017
 */

#ifndef CUSTOMSTRING_H
#define CUSTOMSTRING_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "enums.h"
#include "lex.h"

/**
 * alokoani struktury retezce a inicializace jednotlivych promnenych struktury
 * @param   cString *               ukazatel na strukturu kde ma byt alokovana
 */
void createString(cString *);

/**
 * odmapovani struktury z pameti
 * @param   cString *               ukazatel na strukturu kde ma byt smazana
 */
void destroyString(cString *);

/**
 * vycisteni stringu
 * @param   cString *               ukazatel na strukturu kde ma byt vycisten string
 */
void clearStringStr(cString *);

/**
 * vlozi znak na konec retezce
 * @param   cString *               ukazazatel na retezec kam ma byt znak vlozen
 * @param   char                    vkladany znak
 */
void pushChar(cString *, char );

/**
 * porovna retezec slovem zadanem jako pole znaku
 * @param   cString                 ukazatel na prozkoumavany retezec
 * @param   char *                  porovnavane slovo zadane polem znaku
 * @return  bool                    true v pripade ze jsou porovnavana slova totozne
 */
bool stringKeywordCmp(cString, char *);

/**
 * porovna dva retezce
 * @param   cString                 prvni porovnavany retezec
 * @param   cString                 druhy porovnavany retezec
 * @return  bool                    true v pripade uspechu
 */
bool stringStringCmp(cString, cString);

/**
 * urcit typ, ktery dany string representuje
 * dle typu provede danou normalizaci (uprevi escape sekvence, prevede na des. soustavu, ....)
 * _ret_lit
 * _celoc_lit_bin
 * _celoc_lit_hex
 * _celoc_lit_oct
 *
 * @param   cString                 ukazatel na string
 * @param   int *                   typ retezce
 */
void normalizeStr(cString, int *);

/**
 * upravi escape sekvence ve stringu na pozadovany format
 *
 * @param   cString                 ukazatel na string
 * @return  bool                    v pripade uspechu
 */
bool strEscape(cString);

/**
 * prevede cele cislo z base soustavy do desitkove
 *
 * @param   cString                 ukazatel na string
 * @param   int                     aktualni oustava intu
 * @param   int *                   hodnota v des soustave
 * @return  bool                    true v pripade uspechu
 */
bool baseToDec(cString, int, int *);

/**
 * prevede cele cislo do stringu
 *
 * @param  cString                  ukazatel na string
 * @param  int                      prevadene cislo
 */
void decToStr(cString, int);

 #endif  //CUSTOMSTRING_H

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
 *  Soubor: lex.h
 *  Autori: Miroslav Karpisek
 *          Roman Zajic
 *			Jakub Samek
 *
 *	Datum:  20-10-2017
 *  Uprava: 24-11-2017
 */

#ifndef LEX_H
#define LEX_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "error.h"
#include "enums.h"
#include "custom_string.h"
#include "garbage_collector.h"

/**
 * inicializuje lexikalni analyzator, vytvori lineBuffer, vytvori sadu primitivnich
 * tokenu (EOL, etc), jejiz vyznamova hodnota je jednoducha zapomoci createTokenSet()
 */
void initLex();

/**
 * ziskani nasledujiciho tokenu ze vstupu
 * @return                  ukazatel na struru tokenu v pripade chyby
 *                          (napr.: prazdny soubor) vraci NULL
 */
cToken getToken();

/**
 * vytvori a nasledne prvotne inicializaje novy token
 * @return                  ukazatel na novy token
 */
cToken createToken();

/**
 * slouzi pro modul Error, pri nacteni noveho radku na vstupu je tato informace
 * ulozena
 */
void addLine();

/**
 * slouzi pro modul Error, pri nacteni noveho znaku na vstupu je tato informace
 * ulozena
 */
void addColumn();

/**
 * slouzi pro nacteni nasledujiciho radku do lineBufferu
 */
void loadLine();

/**
 * vytvori sadu primitivnich tokenu (EOL, EOF, etc.)
 */
void createTokenSet();

#endif

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
 *  Soubor: error.h
 *  Autori: Miroslav Karpisek
 *          Jakub Trubka
 *
 *	Datum:  20-10-2017
 *  Uprava: 01-12-2017
 */

#ifndef ERROR_H
#define ERROR_H

#include "enums.h"
#include "garbage_collector.h"
#include <stdbool.h>

//errors
#define ERROR_LEX 1
#define ERROR_SYN 2
#define ERROR_SEM_UNDEF 3
#define ERROR_SEM_TYPE 4
#define ERROR_SEM_ELSE 6
#define ERROR_INTER 99

//barevny text
#define SHADOW "\033[2m"
#define BOLT  "\033[1m"
#define DEF   "\x1B[0m"
#define CRED  "\x1B[31m"
#define CGRN  "\x1B[32m"
#define CYEL  "\x1B[33m"
#define CBLU  "\x1B[34m"
#define CMAG  "\x1B[35m"
#define CCYN  "\x1B[36m"
#define CWHT  "\x1B[37m"

int line, column;
const char* file;
cString lineBuffer;
cString inputBuffer;


/**
 * tisknuti formatovaneho upozorneni
 * @param   int                 informace na ktery znak v lineBuffureu ma byt poukazano
 * @param   const char *        promnene parametry (chybova hlaska)
 * @return  bool                vzdy false (pouze pro spravne ukonceni not-void funkce)
 */
bool warningMsg(int, const char *, ...);

/**
 * tisknuti formatovane chyby
 * @param   int                 kod chyby (dle zadani)
 * @param   int                 informace na ktery znak v lineBuffureu ma byt poukazano
 * @param   const char *        promnene parametry (chybova hlaska)
 * @return  bool                vzdy false (pouze pro spravne ukonceni not-void funkce)
 */
bool errorMsg(int, int, const char *, ...);

/**
 * tisknuti formatovane chyby bez tisknuti problemoveho radku (zejmena tehdy,
 * kdy doslo k chybe kdy neni mozne radek nacist, pripadne nejde presne urcit)
 * @param   int                 kod chyby (dle zadani)
 * @param   const char *        promnene parametry (chybova hlaska)
 * @return  bool                vzdy false (pouze pro spravne ukonceni not-void funkce)
 */
bool error(int, const char *, ...);

/**
 * slouzi pro inkrementaci citace poctu radku
 */
void addLine();

/**
 * slouzi pro inkrementaci citace poctu znaku v radku
 */
void addColumn();

#endif  //ERROR_H

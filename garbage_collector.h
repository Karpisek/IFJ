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
 *  Soubor: garbage_collector.h
 *  Autori: Miroslav Karpisek
 *
 *	Datum:  21-11-2017
 *  Uprava: 02-12-2017
 */

#ifndef H_GARBAGE
#define H_GARBAGE

#include <stdio.h>
#include <stdint.h>
#include "dlist.h"
#include "enums.h"
#include "error.h"
#include "symtable.h"


enum pointerFlag{
    pUNDEF,
    pTOKEN,
    pLIST_ITEM,
    pLIST,
    pSTRING, //meli by se mazat automaticky v lexu
    pTERM,
};

struct GCHash{
    unsigned int arrSize;
    struct GCItem **fPtr;
};

struct GCItem{
    char *type;
    void *addr;
    struct GCItem *pPrev;
    struct GCItem *pNext;
};

/**
 * inicializuje garbage collector (dale jen GC)
 */
void initGC();

/**
 * pridani noveho prvku do GC
 * @param   void *              novy ukazatel na alokovany prvek
 * @param   cahr *              pojmenovani prvku (pro snazsi debuggovani)
 */
void initGCItem(void *, char *);

/**
 * odebrani prvku ze seznamu GC
 * @param   void *              ukazatel na prvek jez ma byt vymazan
 */
void GCRemove(void *);

/**
 * odebrani prvku ze zaznamu GC pro pripad vlastniho uvolneni pameti nezavisleho na GC
 * @param   void *              ukazatel na prvek jez ma byt odebran
 */
void GCErase(void *);

/**
 * vytisknuti GC pro snazsi debuggovani
 */
void printGC();

/**
 * vycisteni alokovane pameti (pro uvolneni veskere dosud neuvolnene pameti)
 */
void cleanUp();

/**
 * pro zjisteni velikosti GC (pro jednodussi debuggovani)
 * @return  int                 velikost GC
 */
int sizeofGC();



#endif

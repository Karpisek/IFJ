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
 *  Soubor: dlist.h
 *  Autori: Jakub Trubka
 *
 *	Datum:  20-10-2017
 *  Uprava: 05-12-2017
 */

#ifndef PROJEKT_DLIST_H
#define PROJEKT_DLIST_H

#include "lex.h"
#include "enums.h"
#include <stdbool.h>


//GC
typedef struct dItem{
    struct Term *term;
    struct dItem *rightP;
    struct dItem *leftP;
} dItem;

//GC
typedef struct dList{
    struct dItem *firstIt;
    struct dItem *lastIt;
    struct dItem *actuaIt;
} *dListPointer;



/**
 * alokuje a nastavi dlist
 * @param  dListPointer *               ukazatel list
 */
bool initDList(dListPointer *);

/**
 * vlozi novy prvek na zacatek
 * @param  dListPointer                 ukazatel na seznam
 * @param  struct Term *                prvek k vlozeni
 */

void insertFirstTerm(dListPointer, struct Term *);
/**
 * smaze item ze seznamu a vrati jeho token
 * @param  dListPointer                 ukazatel na seznam
 * @return struct Term *                popnuty prvek
 */
struct Term *popFirstTerm(dListPointer);

/**
 * vlozi prvek pred prvni vyskyt id
 * @param   dListPointer                ukazatel na seznam
 * @param   struct Term *               ukazatel na prvek pred ktery ma byt vlozeno
 * @param   enumTerm                    porovnavane id
 * @return  bool                        true pokud byla polozka nalezena a vlozena
 */
bool preInsertTerm(dListPointer, struct Term *, enumTerm);

/**
 * najde prvni prvek s id
 * @param   dListPointer                ukazatel na seznam
 * @param   enumTerm                    porovnavane id
 * @return  dItem *                     dItem ktery byl nalezen jinak NULL
 */
dItem *findFirstDItem(dListPointer, enumTerm);

/**
 * kompletne vycisti a uvolni seznam a itemy
 * @param   dListPointer                ukazatel seznam
 */
void clearList(dListPointer);

/**
 * kompletne vycisti a uvolni itemy
 * @param   dListPointer                ukazatel seznam
 */
void clearItems(dListPointer);
/**
 * kompletne vycisti a uvolni seznam a itemy a tokeny
 * @param   dListPointer                ukazatel seznam
 */
void clearAll(dListPointer);
/**
 * posune aktivitu na nasledujici prvek
 * @param  dListPointer                 ukazatel seznam
 */
void dSucc (dListPointer);
/**
 * zjisti zda se v seznamu nachazi prvek 0-16
 * @param   dListPointer                ukazatel seznam
 * @return  enumTerm                    vraci enumTerm nalezeneho prvku
 */
enumTerm findFirstTerminal(dListPointer L);

/**
 * vrati velikost listu
 * @param   dListPointer                ukazatel seznam
 * @return  int                         velikost seznamu
 */
int size(dListPointer);

/**
 * vraci enumID nejvrchnejsiho prvku
 * @param   dListPointer                ukazatel seznam
 * @return  enumTerm                    vraci enumTerm nejvrchnejsiho prvku
 */
enumTerm topTerm(dListPointer L);

/**
 * vraci true kdyz je list prazdny
 * @param   dListPointer                ukazatel seznam
 * @return  bool                        true pokud je seznam prazdny
 */
bool emptyList(dListPointer L);

#endif

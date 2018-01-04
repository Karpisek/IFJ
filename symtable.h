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
 *  Soubor: symtable.h
 *  Autori: Miroslav Karpisek
 *          Jakub Trubka
 *
 *	Datum:  27-10-2017
 *  Uprava: 04-12-2017
 */

#ifndef PROJEKT_SYMTABLE_H
#define PROJEKT_SYMTABLE_H

#define HASH_TABLE_SIZE 100

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lex.h"
#include "enums.h"

typedef struct hTData{
    cToken token;               // token pro ktery jsou vytvorena hTData
    symbolType type;            // typ symbolu (u promene, navratovy typ u funkce)

    bool defined;               // presna hodnota

    unsigned int paramCount;    // pocet parametru u funkci
    struct hTData **paramType;  // pole typu parametru o delce paramCount (nutno alokovat)
} *pData;

typedef struct hTItem{
    char *key;                  // hasovany klic
    struct hTData *data;        // vnitrni data hashovaneho elementu
    struct hTItem *pPrev;       // predchazejici element
    struct hTItem *pNext;       // nasledujici element
    int visibilitySignature;    // koeficient viditelnosti, pro simulovani lokalnich prom
    bool forcedDefinition;      // byla volana, je treba aby byla na konci prekladu definovana


} *pItem;

typedef struct hTTable{
    char* blockName;
    int visibilitySignature;    // koeficient viditelnosti, pro simulovani lokalnich prom
    unsigned int arrSize;
    struct hTItem **fPtr;
    struct hTTable *pNextTable;
    struct hTTable *pPrevTable;
} *pHTable;

/**
 * inicializace retezce
 * @param   unsigned int        pocet bucketu hash tabulky
 * @return  pHTable             vraci ukazatel na strukturu nebo NULL pri neuspechu
 */
pHTable hTabInit(unsigned int);

/**
 * vytvoreni dcerine tabulky, ktera obsahuje kopie lokalnich promennych
 * rodicovske tabulky
 * @param   pHTable             rodic nove vytvorene tabulky
 * @return  pHTable             nove vytvorena dcerina tabulka
 */
pHTable nestedTable(pHTable);

/**
 * vytvoreni lokalni promene 'return' ve funkci
 * @param   pHTable             zvolena tabulka symbolu
 */
void createReturn(pHTable, symbolType);

/**
 * inicializace vestavenych funkci do zadane tabulky symbolu
 * @param   pHTable             do ktere tabulky symbolu se inicializace ma provest
 * @return  bool                uspech inicializace
 */
bool initInlineFunctions(pHTable);

/**
 * najde a odstrani prvek podle klice, pokud prvek neni nalezen vrati false
 * @param   pHTable             ukazatel na hash table
 * @param   char *              porovnavane slovo zadane polem znaku
 * @return  bool                true pokud byla polozka nalezena a smazana
 */
bool hTRemove(pHTable, char *);

/**
 * najde v hash table item podle key
 * @param   pHTable             ukazatel na hash table
 * @param   char *              porovnavane slovo zadane polem znaku
 * @return  pItem               vraci ukazatel na nalezeny item nebo NULL
 */
pItem hTabFind(pHTable, char *);

/**
 * prohleda hash table jestli neobsahuje item s key, pokud ne vytvori ho a vrati na nej ukazatel
 * @param   pHTable             ukazatel na hash table
 * @param   pData               ukazatel na strukturu dat
 * @return  pItem               vraci nove vytvoreni item
 */
pItem hTabLookupAdd(pHTable, pData);

/**
 * vycisti itemy z hash tabulky
 * @param   pHTable             ukazatel na hash tabulku
*/
void hTClear(pHTable);

/**
 * freeckne hashtable
 * @param   pHTable             ukazatel na hash table
 */
pHTable hTfree(pHTable);

/**
 * vypocet indexu z klice
 * @param   const char *        klic pro vypocet indexu
 * @return  unsigned int        vraci index
 */
unsigned int hash_function(const char *);

/**
 * vraci pocet prvku v hashtable
 * @param   pHTable             zkoumana tabulka
 * @return  int                 pocet jejich prvku
 */
int sizeHashTable(pHTable);

/**
 * tisk pro debug
 * @param   pHTable             zvolena tabulka pro tisk
 */
void printHashTable(pHTable);

/**
 * vraci vynulovana data pouze s vlozenim tokenem
 * @param   cToken              token jez ma byt trasponovan na data
 * @return  pData               naplnena pouze predanym tokenem
 */
pData initData(cToken);

/**
 * vytvoreni ci zmena velikosti pole ukazatelu na jednotlive parametry
 * @param   int                 velikost aktualni
 * @param   pData *             pointer jez ma byt realokovan
 * @return  pData *             novy pointer na pole ukazatelu na parametry
 */
pData* createParamArray(int, pData *);

/**
 * pomocna funkce na tisknuti jednotlivych parametru
 * @param   pItem               element tabulky jez je zkouman pro jednotlive jeho typy parametru
 * @return  char *              obsahujici naformatovany vystup
 */
char* printParamTypes(pItem);

#endif //PROJEKT_SYMTABLE_H

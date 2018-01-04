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
 *  Soubor: garbage_collector.c
 *  Autori: Miroslav Karpisek
 *
 *	Datum:  21-11-2017
 *  Uprava: 02-12-2017
 */

#include "garbage_collector.h"

struct GCHash GC;

char* gcEnumsArray[] = {
    "__UNDEF__",
    "TOKEN",
    "LIST_ITEM",
    "LIST",
    "STRING",
    "TERM",

};

void initGC(unsigned int size) {
    if((GC.fPtr = malloc(size * (sizeof(struct GCItem)))) == NULL){  //alokace bucketu
        error(ERROR_INTER, "Chyba alokace pameti GCBucketu\n");
    }

    GC.arrSize = size;

    for(int i = 0; i < (int)size; i++){
        GC.fPtr[i] = NULL;
    }
}

unsigned int GChash(void* addrForHash) {
    unsigned int index = (unsigned int)((uintptr_t)addrForHash / 2);
    return index/(sizeof(int *)) % GC.arrSize;
}

void initGCItem(void *addrForHash, char* type){
    struct GCItem *newItem = NULL;

    unsigned int index = GChash(addrForHash); //nalezeni indexu pro novy prvek
    if((newItem = malloc(sizeof(struct GCItem))) == NULL) {  //alokace noveho prvku
        error(ERROR_INTER, "Chyba pri alokaci GCItem");
    }

    newItem->addr = addrForHash;     //nastaveni noveho prvku a vlozeni na prvni misto ramene
    newItem->type = type;
    newItem->pPrev = NULL;
    newItem->pNext = GC.fPtr[index];


    if(GC.fPtr[index] != NULL){ //pokud prvek neni prazdny, navazeme seznam
        GC.fPtr[index]->pPrev = newItem;
    }

    GC.fPtr[index] = newItem;

}

void GCErase(void *toErase) {
    if(toErase == NULL){
        return;
    }
    unsigned int index = GChash(toErase);      //nalezeni indexu
    struct GCItem *tmp = GC.fPtr[index];        //docasny prvek

    if(toErase == tmp->addr) {
        if(tmp->pNext != NULL) {
            tmp->pNext->pPrev = NULL;

        }
        GC.fPtr[index] = tmp->pNext;

        free(tmp);

        return;
    }

    while(tmp != NULL){
        if(toErase == tmp->addr){                //hledani prvku v cyklu

            if(tmp->pNext != NULL) {
                tmp->pNext->pPrev = tmp->pPrev;
            }
            tmp->pPrev->pNext = tmp->pNext;

            free(tmp);
            return;
        }
        tmp = tmp->pNext;
    }

    error(ERROR_INTER, "Nenalezen pointer pro erase!");
}

void GCRemove(void *toRemove) {
    if(toRemove == NULL){
        return;
    }
    unsigned int index = GChash(toRemove);      //nalezeni indexu
    struct GCItem *tmp = GC.fPtr[index];        //docasny prvek

    // rpvni prvek je hledanym
    if(toRemove == tmp->addr) {
        if(tmp->pNext != NULL) {
            tmp->pNext->pPrev = NULL;

        }
        GC.fPtr[index] = tmp->pNext;

        free(tmp->addr);
        free(tmp);
        return;
    }

    while(tmp != NULL){
        if(toRemove == tmp->addr){                //hledani prvku v cyklu

            if(tmp->pNext != NULL) {
                tmp->pNext->pPrev = tmp->pPrev;
            }
            tmp->pPrev->pNext = tmp->pNext;

            free(tmp->addr);
            free(tmp);
            return;
        }
        tmp = tmp->pNext;
    }

    error(ERROR_INTER, "Nenalezen pointer !");
}

void cleanUp() {

    for(int i = 0; i < (int)GC.arrSize; i++) {
        struct GCItem *tmp;
        while(GC.fPtr[i] != NULL) {
            tmp = GC.fPtr[i];
            GC.fPtr[i] = tmp->pNext;

            free(tmp->addr);
            free(tmp);


        }
        free(GC.fPtr[i]);



    }

    //smazani pole ukazatelu tabulky
    free(GC.fPtr);

    return;
}

int sizeofGC() {
    unsigned int size = 0;
    for(int i = 0; i < (int)GC.arrSize; i++) {

        struct GCItem *helper = GC.fPtr[i];

        while(helper != NULL) {
            size ++;
            helper = helper->pNext;
        }
    }

    return size;
}

void printGC() {
    printf ("------------GARBAGE--------------\n");
	for ( int i=0; i < (int)GC.arrSize; i++ ) {
		printf ("%i:",i);
		struct GCItem *item = GC.fPtr[i];
		while ( item != NULL ) {
			printf ("\t%s\t(%p)", item->type, item->addr);
            item = item->pNext;
		}
		printf ("\n");
	}

	printf ("------------------------------------\n");
 	printf ("Items count %i \n",sizeofGC());
	printf ("------------------------------------\n");

}

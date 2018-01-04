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
 *  Soubor: dlist.c
 *  Autori: Miroslav Karpisek
 *
 *	Datum:  21-11-2017
 *  Uprava: 02-12-2017
 */

#include "dlist.h"

//TODO volal bych error a cistici funkci primo odtud a vracel bych void
bool initDList(dListPointer *L){
    if((*L = malloc(sizeof(struct dList))) == NULL) { //alokace
        //TODO error
        return false;
    }
    (*L)->actuaIt = NULL;
    (*L)->firstIt = NULL;
    (*L)->lastIt = NULL;


    initGCItem(*L, "LIST");
    return true;
}

void insertFirstTerm(dListPointer L, struct Term *t ) {
    if(L != NULL && t != NULL){
        dItem *newItem = NULL;
        if((newItem = malloc(sizeof(struct dItem))) != NULL){ //alokace pameti
            newItem->term = t;   //nastaveni hodnot a zarazeni do seznamu
            newItem->rightP = L->firstIt;
            newItem->leftP = NULL;
            if(L->firstIt != NULL){
                L->firstIt->leftP = newItem;
            }
            else{
                L->lastIt = newItem;
            }
            L->firstIt = newItem;

            // pridani zaznamu do GC
            initGCItem(newItem, "LITEM");
        }
    }
}

struct Term *popFirstTerm(dListPointer L) {
    dItem *tmpItem = NULL; //docasny ukazatel na prvek ktery budeme rusit
    struct Term *tmpTerm = NULL;
    if(L->firstIt != NULL)//kontrola prazdnosti seznamu
    {
        tmpItem = L->firstIt; //ulozeni ukazatele na prvni prvek
        tmpTerm = L->firstIt->term;
        if(L->firstIt == L->actuaIt)    //kontrola zda prvni prvek neni aktivni, rusime aktivitu
        {
            L->actuaIt = NULL;
        }
        if(L->firstIt == L->lastIt) //kontrola zda v seznamu nei pouze jeden prvek, smazani jedineho prvku
        {
            L->firstIt = NULL;
            L->lastIt = NULL;
        }
        else    //v seznamu je vice prvku, zmena prvniho
        {
            L->firstIt = L->firstIt->rightP;
            L->firstIt->leftP = NULL; //prvni prvek nema zleva nic
            //printList(L);
        }

        GCRemove(tmpItem);
    }

    return tmpTerm;
}


bool preInsertTerm(dListPointer L, struct Term *t, enumTerm id) {
    if(L != NULL && t != NULL){ //kontrola parametru
        dItem *preInsert = findFirstDItem(L, id);    //hledani prvniho vyskytu
        if(preInsert != NULL){
            dItem *newItem;
            if((newItem = malloc(sizeof(struct dItem))) == NULL){ //alokace noveho prvku
                //TODO error handle
            }
            newItem->term = t;   //nastaveni noveho prvku a vlozeni pred nalezeny prvek
            newItem->rightP = preInsert;
            newItem->leftP = preInsert->leftP;
            if(preInsert == L->firstIt){
                L->firstIt = newItem;
            }
            else{
                newItem->leftP->rightP = newItem;
            }

            //vlozeni do GC
            initGCItem(newItem, "LITEM");
            //printList(L);
            return true;
        }
    }
    return false;
}


dItem *findFirstDItem(dListPointer L, enumTerm id) {
    if( L != NULL){
        L->actuaIt = L->firstIt;
        while(L->actuaIt != NULL){
            if(L->actuaIt->term->termType == id){
                return L->actuaIt;
            }
            dSucc(L);
        }
    }
    return NULL;
}

void dSucc(dListPointer L) {
    if(L->actuaIt != NULL) //kontrola aktualniho prvku
    {
        if(L->actuaIt == L->lastIt)   //zruseni aktualniho prvku
        {
            L->actuaIt = NULL;
        }
        else
        {
            L->actuaIt = L->actuaIt->rightP; //zmena aktualniho prvku
        }
    }
}

void clearList(dListPointer L) {
    if(L != NULL){
        clearItems(L);
        free(L);
    }
}

void clearItems(dListPointer L) {
    if(L != NULL){
        dItem *toClear;
        while(L->firstIt != NULL){
            toClear = L->firstIt;
            L->firstIt = toClear->rightP;
            free(toClear);
        }
    }
    L->actuaIt = NULL;
    L->lastIt = NULL;
}

enumTerm findFirstTerminal(dListPointer L) {
    if(L != NULL){
        L->actuaIt = L->firstIt;
        while(L->actuaIt != NULL){

            if(L->actuaIt->term->termType >= __LEFT_BRACKET && L->actuaIt->term->termType <= __STRING){
                return L->actuaIt->term->termType;
            }
            dSucc(L);
        }
    }
    return -1;
}

int size(dListPointer L) {
    int i = 0;
    dItem *helper = L->firstIt;
    for(;helper != NULL; i++) {
        helper = helper->rightP;
    }
    return i;
}

enumTerm topTerm(dListPointer L) {
    return L->firstIt->term->termType;
}

bool emptyList(dListPointer L) {
    return (L->firstIt == NULL);
}

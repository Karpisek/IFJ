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
 *  Soubor: symtable.c
 *  Autori: Miroslav Karpisek
 *          Jakub Trubka
 *
 *	Datum:  27-10-2017
 *  Uprava: 04-12-2017
 */

#include "symtable.h"

pHTable nestedTable(pHTable parentTable) {

    pHTable newTable = hTabInit(HASH_TABLE_SIZE);
    newTable->visibilitySignature = 0;
    newTable->pNextTable = NULL;
    newTable->pPrevTable = NULL;
    newTable->blockName = "";

    // pripade napojeni na "rodice" -> visibilitySignature +1 oproti jemu
    if(parentTable != NULL) {
        newTable->pNextTable = parentTable;
        parentTable->pPrevTable = newTable;
        newTable->visibilitySignature = parentTable->visibilitySignature + 1;

        // napojeni linearnich vazanych seznamu
        if(parentTable->arrSize != newTable->arrSize) {
            error(ERROR_INTER, "Chyba pri vytvareni tabulky symbolu");
        }

        for(int i = 0; i < (int)parentTable->arrSize; i++) {
            newTable->fPtr[i] = parentTable->fPtr[i];
        }
    }

    // pokud se jedna o root tabulku symbolu, inicializace vestavenych funkci
    if(parentTable == NULL) {

        // inicializace vestavenych funkci
        if(!initInlineFunctions(newTable)) {
            error(ERROR_INTER, "Chyba pri vkladani vestavenych funkci do tabulky symbolu");
        }
    }

    return newTable;
}



pHTable hTabInit(unsigned int size){
    pHTable newTable = NULL;
    if((newTable = malloc(sizeof(struct hTTable))) == NULL){  //alokace struktury
        return NULL;
    }

    //pridani do GC
    initGCItem(newTable, "HTABLE");


    if((newTable->fPtr = malloc(size * (sizeof(pItem)))) == NULL){  //alokace bucketu
        //free(newTable);
        return NULL;
    }
    //pridani do GC
    initGCItem(newTable->fPtr, "HTABLEP");

    newTable->arrSize = size;       //prvotni nastaveni
    newTable->pNextTable = NULL;
    newTable->pPrevTable = NULL;

    for(int i = 0; i < (int)size; i++){
        newTable->fPtr[i] = NULL;
    }
    return newTable;
}

pItem hTabFind(pHTable t, char *key){

    if(t == NULL || key == NULL){
        return NULL;
    }
    unsigned int index = (hash_function(key) % t->arrSize); //nalezeni indexu
    struct hTItem *tmp = t->fPtr[index];        //docasny prvek
    while(tmp != NULL){
        if(strcmp(tmp->key, key) == 0){                //hledani prvku v cyklu
            return tmp;
        }
        tmp = tmp->pNext;
    }

    pHTable helperTable = t;
    helperTable = helperTable->pNextTable;
    return hTabFind(helperTable, key);
}

pItem hTabLookupAdd(pHTable t, pData data){
    if(t != NULL && data != NULL && data->token != NULL && data->token->cPointer != NULL){
        struct hTItem *newItem = hTabFind(t, data->token->cPointer);      //je prvek jiz v hash table?
        if(newItem == NULL || newItem->visibilitySignature != t->visibilitySignature){
            unsigned int index = (hash_function(data->token->cPointer) % t->arrSize);//nalezeni indexu pro novy prvek
            if((newItem = malloc(sizeof(struct hTItem))) == NULL) {  //alokace noveho prvku
                return NULL;
            }
            newItem->key = data->token->cPointer;     //nastaveni noveho prvku a vlozeni na prvni misto ramene
            newItem->data = data;
            newItem->pPrev = NULL;
            newItem->pNext = t->fPtr[index];
            newItem->visibilitySignature = t->visibilitySignature;
            newItem->forcedDefinition = false;


            if(t->fPtr[index] != NULL){ //pokud prvek neni prazdny, navazeme seznam
                t->fPtr[index]->pPrev = newItem;
            }
            t->fPtr[index] = newItem;

            //pridani do GC
            initGCItem(newItem, "HITEM");
        }
        return newItem;
    }
    return NULL;

}

//TODO !!!
void hTClear(pHTable t) {
    struct hTItem *toClear = NULL, *tmp = NULL; //pomocne pointery pro pohyb v hash table
    if (t != NULL) {

        // vymazani dvou totozych ukazatelu (int a str, pr simulaci vestavenych funkci)
        for (int i = 0; i < (int)t->arrSize; i++) {    //pruchod vsemy prvky
            toClear = t->fPtr[i];                      //nastaveni prvniho ukazatele
            while (toClear != NULL) {                  //cyklus pro pruchod ve vetvi
                tmp = toClear->pNext;                  //ulozeni dalsiho clene aby se ukazatel na nej neztratil

                // kontrola jestli se nenachazi v listu nadrazeneho
                if(toClear->visibilitySignature == t->visibilitySignature) {
                    if (toClear->data != NULL) {           //vycisteni datove casti
                        GCRemove(toClear->data->paramType);    //uakzatele ukazatelu
                        GCRemove(toClear->data);
                    }
                    GCRemove(toClear);                  //uvolneni prvku
                }
                toClear = tmp;

            }
            t->fPtr[i] = NULL; //zalepeni
        }

        if(t->pNextTable != NULL) {
            t->pNextTable->pPrevTable = NULL;
        }

    }
}


pHTable hTfree(pHTable t){
    return t->pNextTable;
}


unsigned int hash_function(const char *key){    //hashovaci funkce z ijc (sdbm)
    unsigned int h=0;
    const unsigned char *p;
    for(p=(const unsigned char*)key; *p != '\0'; p++){
        h = 65599*h + *p;
    }
    return h;
}

void printHashTable(pHTable t) {

    if(t != NULL) {
    	printf ("------------HASH TABLE--------------\n");
    	for ( int i=0; i < (int)t->arrSize; i++ ) {
    		printf ("%i:",i);
    		pItem item = t->fPtr[i];
    		while ( item != NULL ) {
    			printf (" (%s,%s,vis: %d",item->key, keywords[item->data->token->id], item->visibilitySignature);
                if(item->data->token->cPointer != NULL) {
                    printf(": %s", varType[item->data->type]);
                }
                printf(")");
                item = item->pNext;
    		}
    		printf ("\n");
    	}

    	printf ("------------------------------------\n");
    	printf ("Items count %i \n",sizeHashTable(t));
    	printf ("------------------------------------\n");
    }


}

int sizeHashTable(pHTable t) {
    int size = 0;
    for(int i = 0; i < (int)t->arrSize; i++) {

        pItem item;
        item = t->fPtr[i];
        while(item != NULL) {
            size ++;

            //printf("(%d)\n", i);
            item = item->pNext;
        }

    }

    return size;
}

pData initData(cToken pToken) {
    pData newData = NULL;
    if((newData = malloc(sizeof(struct hTData))) == NULL) {
        //TODO error malloc
    }
    //pridani do GC
    initGCItem(newData, "HDATA");

    newData->token = pToken;
    newData->type = UNDEFINED;
    newData->defined = false;

    newData->paramCount = -1;
    newData->paramType = NULL;

    return newData;
}

pData* createParamArray(int size, pData *oldPA) {
    GCErase(oldPA);
    if((oldPA = realloc(oldPA, size * (sizeof(pData)))) == NULL){  //pole hTData
        return NULL;
    }
    initGCItem(oldPA, "HDATA");
    return oldPA;
}

bool initInlineFunctions(pHTable newTable) {
    //###############################
    //  length(s as string)
    //###############################
    cToken lengthToken = createToken();
    lengthToken->id = _func;
    lengthToken->cPointer = "length";
    pData lenghtData = initData(lengthToken);
    hTabLookupAdd(newTable, lenghtData);
    lenghtData->defined = true;
    lenghtData->paramCount = 1;
    lenghtData->type = INT;

    //vytvoreni parametru s (string)
    cToken stringToken = createToken();
    stringToken->id = _id;
    stringToken->cPointer = "s";
    pData stringData = initData(stringToken);
    stringData->defined = true;
    stringData->paramCount = 0;
    stringData->type = STRING;

    GCErase(lenghtData->paramType);
    if((lenghtData->paramType = realloc(lenghtData->paramType, lenghtData->paramCount * (sizeof(pData)))) == NULL){  //pole hTData
        error(ERROR_INTER, "Chyba pri vkladani vestavenych funkci do tabulky symbolu");
    }
    initGCItem(lenghtData->paramType, "DATA");

    lenghtData->paramType[0] = stringData;

    //##############################################################
    //  SubStr(s As String, i As Integer, n As Integer) As String
    //##############################################################
    cToken subStrToken = createToken();
    subStrToken->id = _func;
    subStrToken->cPointer = "substr";
    pData subStrData = initData(subStrToken);
    hTabLookupAdd(newTable, subStrData);
    subStrData->defined = true;
    subStrData->paramCount = 3;
    subStrData->type = STRING;

    //vytvoreni parametru i (integer)
    cToken integerToken = createToken();
    integerToken->id = _id;
    integerToken->cPointer = "i";
    pData integerData = initData(integerToken);
    integerData->defined = true;
    integerData->paramCount = 0;
    integerData->type = INT;

    GCErase(subStrData->paramType);
    if((subStrData->paramType = realloc(subStrData->paramType, subStrData->paramCount * (sizeof(pData)))) == NULL){  //pole hTData
        error(ERROR_INTER, "Chyba pri vkladani vestavenych funkci do tabulky symbolu");
    }
    initGCItem(subStrData->paramType, "DATA");

    subStrData->paramType[0] = stringData;
    subStrData->paramType[1] = integerData;
    subStrData->paramType[2] = integerData;

    //##############################################################
    //  Asc(s As String, i As Integer) As Integer
    //##############################################################
    cToken ascToken = createToken();
    ascToken->id = _func;
    ascToken->cPointer = "asc";
    pData ascData = initData(ascToken);
    hTabLookupAdd(newTable, ascData);
    ascData->defined = true;
    ascData->paramCount = 2;
    ascData->type = INT;

    GCErase(ascData->paramType);
    if((ascData->paramType = realloc(ascData->paramType, ascData->paramCount * (sizeof(pData)))) == NULL){  //pole hTData
        error(ERROR_INTER, "Chyba pri vkladani vestavenych funkci do tabulky symbolu");
    }
    initGCItem(ascData->paramType, "DATA");

    ascData->paramType[0] = stringData;
    ascData->paramType[1] = integerData;

    //##############################################################
    //  Chr(i As Integer) As String
    //##############################################################
    cToken chrToken = createToken();
    chrToken->id = _func;
    chrToken->cPointer = "chr";
    pData chrData = initData(chrToken);
    hTabLookupAdd(newTable, chrData);
    chrData->defined = true;
    chrData->paramCount = 1;
    chrData->type = STRING;

    GCErase(chrData->paramType);
    if((chrData->paramType = realloc(chrData->paramType, chrData->paramCount * (sizeof(pData)))) == NULL){  //pole hTData
        error(ERROR_INTER, "Chyba pri vkladani vestavenych funkci do tabulky symbolu");
    }
    initGCItem(chrData->paramType, "DATA");

    chrData->paramType[0] = integerData;

    return true;
}

// lokalni promenna return pro funkce
void createReturn(pHTable targetTable, symbolType type) {
    cToken retToken = createToken();
    retToken->id = _id;
    retToken->cPointer = "$return";
    pData retData = initData(retToken);
    hTabLookupAdd(targetTable, retData);
    retData->type = type;
}

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
 *  Soubor: parser.c
 *  Autori: Miroslav Karpisek
 *          Jakub Trubka
 *
 *	Datum:  20-10-2017
 *  Uprava: 05-12-2017
 */

#include "parser.h"

struct customToken *token;                      // ukazatel na aktualni token
struct customToken *tokenMem;                   // pomocny ukazatel na token (pro tisteni chybovych hlasek)

struct hTTable *symbolTableGlobal = NULL;       // ukazatel na globalni tabulku symbolu
struct hTTable *symbolTableActual = NULL;       // ukazatel na aktualni lokalni tabulku symbolu

int precedencTable[20][20] =
//     (     )     f    ,      $     *      /     \    +     -     =     <>    <    <=     >     >=    i    str   dou   int
{   {LESS, EQUA, LESS, EQUA, NONE, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS}, // (
    {NONE, GREA, NONE, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, NONE, NONE, NONE, NONE}, // )
    {EQUA, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE}, // f
    {LESS, EQUA, LESS, EQUA, NONE, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS}, // ,
    {LESS, NONE, LESS, NONE, NONE, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS, LESS}, // $
    {LESS, GREA, LESS, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, LESS, LESS, LESS, LESS}, // *
    {LESS, GREA, LESS, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, LESS, LESS, LESS, LESS}, // /
    {LESS, GREA, LESS, GREA, GREA, LESS, LESS, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, LESS, LESS, LESS, LESS}, // \ .
    {LESS, GREA, LESS, GREA, GREA, LESS, LESS, LESS, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, LESS, LESS, LESS, LESS}, // +
    {LESS, GREA, LESS, GREA, GREA, LESS, LESS, LESS, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, LESS, LESS, LESS, LESS}, // -
    {LESS, GREA, LESS, GREA, GREA, LESS, LESS, LESS, LESS, LESS, NONE, NONE, NONE, NONE, NONE, NONE, LESS, LESS, LESS, LESS}, // =
    {LESS, GREA, LESS, GREA, GREA, LESS, LESS, LESS, LESS, LESS, NONE, NONE, NONE, NONE, NONE, NONE, LESS, LESS, LESS, LESS}, // <>
    {LESS, GREA, LESS, GREA, GREA, LESS, LESS, LESS, LESS, LESS, NONE, NONE, NONE, NONE, NONE, NONE, LESS, LESS, LESS, LESS}, // <
    {LESS, GREA, LESS, GREA, GREA, LESS, LESS, LESS, LESS, LESS, NONE, NONE, NONE, NONE, NONE, NONE, LESS, LESS, LESS, LESS}, // <=
    {LESS, GREA, LESS, GREA, GREA, LESS, LESS, LESS, LESS, LESS, NONE, NONE, NONE, NONE, NONE, NONE, LESS, LESS, LESS, LESS}, // >
    {LESS, GREA, LESS, GREA, GREA, LESS, LESS, LESS, LESS, LESS, NONE, NONE, NONE, NONE, NONE, NONE, LESS, LESS, LESS, LESS}, // >=
    {NONE, GREA, NONE, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, NONE, NONE, NONE, NONE}, // i
    {NONE, GREA, NONE, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, NONE, NONE, NONE, NONE}, // string
    {NONE, GREA, NONE, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, NONE, NONE, NONE, NONE}, // double
    {NONE, GREA, NONE, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, GREA, NONE, NONE, NONE, NONE}, // int
};

bool syntaxError(enumID expected) {

    (expected == _NULL) ?
    errorMsg(ERROR_SYN, token->column, "Neocekavany token: [%s]", keywords[token->id]):
    errorMsg(ERROR_SYN, token->column, "Neocekavany token: [%s], ocekaval jsem [%s]", keywords[token->id], keywords[expected]);

    return false;
}

void nextToken() {

    tokenMem = token;
    if((token = getToken()) == NULL){
        errorMsg(ERROR_LEX, 1,"Nevalidni lexem");
    }
}

// hlavni funkce volana jednou z main(); je hlavnim mozkem celeho prekladu
bool parse() {

    // inicializace garbage collectoru
    initGC(100);

    // vytvoreni tabulek symbolu
    symbolTableActual = nestedTable(symbolTableActual);

    // ulozeni ukazatele glovalni tabulky symbolu (no. 0);
    symbolTableGlobal = symbolTableActual;

    // zadam lex o otevreni souboru
    //openFile(filename);
    initLex();

    // inicializace instruction listu
    insListInit();

    // nacteni pocatecniho tokenu
    nextToken();

    if (LL_begin()) {
        // kontrola, ze byly vsechny volane funkce definovane
        for(int i = 0; i < (int)symbolTableGlobal->arrSize; i++) {
            pItem inspectedItem = symbolTableGlobal->fPtr[i];
            while(inspectedItem != NULL) {
                if(inspectedItem->forcedDefinition) {
                    if(!inspectedItem->data->defined) {
                        error(ERROR_SEM_UNDEF, "Nepovolena implicitni definice funkce %s()", inspectedItem->key);
                    }

                }
                inspectedItem = inspectedItem->pNext;
            }
        }
        insPrint(stdout);

        cleanUp();
        return true;
    }
    else {
        return false;
    }

}

bool LL_program() {
    // simulace pravidla 1
    if(token->id == _Scope || token->id == _Declare || token->id == _Function || token->id == _Dim) {

        if(!LL_declareList()) {
            syntaxError(_NULL);
        }
        if(!LL_body(NULL)) {
            syntaxError(_NULL);
        }
        return true;
    }

    return syntaxError(_NULL);

}

bool LL_body(pItem funcItem) {
    //simulace pravidla 2
    if(token->id == _Scope) {


        // scope -> vytvoreni nested tabulky symbolu
        symbolTableActual = nestedTable(symbolTableActual);

        // vytvoreni ramce a klonovani viditelnych promnenych
        createFrame();

        nextToken();
        if(token->id != _EOL) {
            return syntaxError(_EOL);
        }
        nextToken();
        if(!LL_statList(funcItem)) {
            return syntaxError(_NULL);
        }
        if(token->id != _End) {
            return syntaxError(_End);
        }
        nextToken();
        if(token->id != _Scope) {
            return syntaxError(_Scope);
        }
        nextToken();

        // smazani ramce a klonovani viditelnych promnenych
        destroyFrame();

        // vyrazeni posledni tabulky symbolu
        symbolTableActual = hTfree(symbolTableActual);

        return true;
    }

    return syntaxError(_NULL);
}

// priprava rozsireni else-if
bool LL_ifstat(pItem funcItem) {
    // <ifstat> ->  ELSEIF THEN EOL <statList> <ifstat>
    if(token->id == _Elseif) {

        // smazani if-zanoreni
        //printHashTable(symbolTableActual);
        destroyFrame();
        symbolTableActual = hTfree(symbolTableActual);

        // TODO GENERATE instrukce
        generateSceletInstruction(_ELSE, NULL);

        nextToken();
        int column = token->column;
        if(LL_expr() != BOOL) {
            return errorMsg(ERROR_SEM_TYPE, column, "Vyrazem neni pravdivostni hodnota" );
        }

        generateSceletInstruction(_ELSEIF, NULL);

        if(token->id != _Then) {
            return syntaxError(_Then);
        }

        // vytvoreni noveho zanoreni
        symbolTableActual = nestedTable(symbolTableActual);
        createFrame();

        nextToken();
        if(token->id != _EOL) {
            return syntaxError(_EOL);
        }
        nextToken();
        if(!LL_statList(funcItem)) {
            return syntaxError(_NULL);
        }

        return LL_ifstat(funcItem);
    }

    // <ifstat> -> ELSE EOL <statList>
    if(token->id == _Else) {

        destroyFrame();
        // smazani if-zanoreni
        symbolTableActual = hTfree(symbolTableActual);

        // TODO GENERATE instrukce
        generateSceletInstruction(_ELSE, NULL);

        // vytvoreni noveho zanoreni
        symbolTableActual = nestedTable(symbolTableActual);
        createFrame();

        nextToken();
        if(token->id != _EOL) {
            return syntaxError(_EOL);
        }
        nextToken();
        if(!LL_statList(funcItem)) {
            return syntaxError(_NULL);
        }
        if(token->id != _End) {
            return syntaxError(_End);
        }
        nextToken();
        if(token->id != _If) {
            syntaxError(_If);
        }

        destroyFrame();
        symbolTableActual = hTfree(symbolTableActual);

        generateSceletInstruction(_ENDIF, NULL);

        nextToken();
        return true;
    }

    // <ifstat> ->  ε
    // FOLLOW = {_End}
    else if(token->id == _End) {
        nextToken();
        if(token->id != _If) {
            syntaxError(_If);
        }

        destroyFrame();
        symbolTableActual = hTfree(symbolTableActual);

        // endif
        generateSceletInstruction(_ELSE, NULL);
        generateSceletInstruction(_ENDIF, NULL);

        nextToken();
        return true;
    }

    return syntaxError(_NULL);
}

bool LL_declareList() {
    //simulace pravidla 4
    if(token->id == _Scope) {



        return true;
    }
    //simulace pravidla 3
    else if(token->id == _Declare || token->id == _Function || token->id == _Dim) {




        return(LL_declare() && LL_space(true) && LL_declareList());
    }

    return syntaxError(_NULL);
}

bool LL_declare() {
    // <declare> -> DECLARE FUNCTION id ( <paramsList> ) AS <type>
    if(token->id == _Declare) {



        nextToken();
        if(token->id != _Function) {
            return syntaxError(_Function);
        }
        nextToken();
        if(token->id != _id) {
            return syntaxError(_id);
        }

        //kontrola zda-li stejny identifikator neni deklarovan
        pItem funcHash;
        char* funcName = token->cPointer;

        if((funcHash = hTabFind(symbolTableActual, funcName)) != NULL) {
            if(funcHash->data->token->id == _func) {
                errorMsg(ERROR_SEM_UNDEF, token->column, "Pokus deklarovat jiz deklarovanou funkci '%s()'", funcName);
            }
            else {
                errorMsg(ERROR_SEM_UNDEF, token->column, "Pokus deklarovat jiz deklarovanou promnenou '%s'", funcName);
            }

        }

        // pridani funkce do globalni tabulky symbolu + ulozeni jmena aktualne deklarovane funkce
        token->id = _func;
        hTabLookupAdd(symbolTableGlobal, initData(token));

        // vytvoreni obrazu pro newParamArray a pocet parametru
        int paramCount = 0;
        pData *newParamArray = NULL;

        nextToken();
        if(token->id != _left_bracket) {
            return syntaxError(_left_bracket);
        }
        nextToken();

        if(!LL_paramList(&newParamArray, &paramCount)) {
            return syntaxError(_NULL);
        }
        if(token->id != _right_bracket) {
            return syntaxError(_right_bracket);
        }
        nextToken();
        if(token->id != _As) {
            return syntaxError(_As);
        }
        nextToken();

        // priradi navratovy typ funkce do tabulky symbolu
        if(!LL_type(&hTabFind(symbolTableGlobal, funcName)->data->type)) {
            return syntaxError(_NULL);
        }

        nextToken();

        // aktualizace tabulky symbolu
        hTabFind(symbolTableGlobal, funcName)->data->paramType = newParamArray;
        hTabFind(symbolTableGlobal, funcName)->data->paramCount = paramCount;
        hTabFind(symbolTableGlobal, funcName)->forcedDefinition = true;

        return true;
    }

    // <declare> -> FUNCTION id ( <paramsList> ) AS <type> EOL <statList> END FUNCTION //return
    else if(token->id == _Function) {


        nextToken();
        if(token->id != _id) {
            return syntaxError(_id);
        }

        // ulozeni jmena aktualne deklarovane funkce
        char* funcName = token->cPointer;
        pItem funcHash = hTabFind(symbolTableGlobal, funcName);
        cToken newFunc = token;

        //kontrola zda-li stejny identifikator neni deklarovan
        if(funcHash != NULL) {
            if(funcHash->data->defined) {
                errorMsg(ERROR_SEM_UNDEF, token->column, "Pokus o opetovnou definici definovane funkce %s()", funcName);
            }
        }

        // vytvoreni obrazu pro newParamArray a pocet parametru
        int paramCount = 0;
        pData *newParamArray = NULL;

        nextToken();
        if(token->id != _left_bracket) {
            return syntaxError(_left_bracket);
        }
        nextToken();

        if(!LL_paramList(&newParamArray, &paramCount)) {
            return syntaxError(_NULL);
        }
        if(token->id != _right_bracket) {
            return syntaxError(_right_bracket);
        }
        nextToken();
        if(token->id != _As) {
            return syntaxError(_As);
        }
        nextToken();

        symbolType returnType;

        if(!LL_type(&returnType)) {
            return syntaxError(_NULL);
        }
        if(token->id != _EOL) {
            return syntaxError(_EOL);
        }

        //#################################
        //  KONTROLA DEKLARACE A DEFINICE
        //#################################

        // prepsani tabulky lokalnich promnenych a naplneni vstupnimi paramtry funkce
        symbolTableActual = nestedTable(symbolTableActual);
        createReturn(symbolTableActual, returnType);

        // kontrola shodnohe poctu parametru (jiz byla deklarovana)
        if(funcHash != NULL) {

            // kontrola shodneho poctu parametru
            if(paramCount != (int)funcHash->data->paramCount) {
                errorMsg(ERROR_SEM_UNDEF, newFunc->column, "Funkce %s() pozaduje %d parametru, zadano %d", funcName,funcHash->data->paramCount, paramCount);
            }

            // kontrola shodnosti parametru s deklarovanoou funkci
            for(int i = 0; i < paramCount; i++) {
                if(funcHash->data->paramType[i]->type != newParamArray[i]->type) {
                    errorMsg(ERROR_SEM_UNDEF, newFunc->column, "Funkce %s() pozaduje jiny parametr", funcName);
                }
                else {
                    pItem localVar = hTabLookupAdd(symbolTableActual, newParamArray[i]);
                    localVar->data->defined = true;
                }

            }

            // kontrola shodnosti navratoveho typu
            if(returnType != funcHash->data->type) {
                errorMsg(ERROR_SEM_UNDEF, newFunc->column, "Funkce %s() byla deklarovana navratovym typem %s,\n\tpri jeji definici vsak je jeji navratovy typ %s", funcName, varType[funcHash->data->type], varType[returnType]);
            }

        }
        // definice bez deklarace
        else {
            newFunc->id = _func;
            funcHash = hTabLookupAdd(symbolTableGlobal, initData(newFunc));

            // aktualizace tabulky symbolu
            funcHash->data->type = returnType;
            funcHash->data->paramType = newParamArray;
            funcHash->data->paramCount = paramCount;

            // vlozeni parametru do lokalni tabulky symbolu
            for(int i = 0; i < paramCount; i++) {
                pItem localVar = hTabLookupAdd(symbolTableActual, newParamArray[i]);
                localVar->data->defined = true;
            }
        }

        // ulozeni informace o definici do hashtable (jiz ted pro pripadnou rekurzi)
        pData funcData = NULL;
        (funcData = funcHash->data)->defined = true;




        // generace labelname
        generateSceletInstruction(_CREATE_FUNC, initTerm(funcHash->data->token));

        // volani generatoru instrukci pro napopovani jsou v opacnem poradi na zasobniku
        for(int i = paramCount - 1; i >= 0; i--) {
            generateSceletInstruction(_DECLARE_VAR, initTerm(newParamArray[i]->token));
            generateSceletInstruction(_MOVE, initTerm(newParamArray[i]->token));
        }



        //################################

        nextToken();
        if(!LL_statList(funcHash)) {
            return syntaxError(_NULL);
        }
        if(token->id != _End) {
            return syntaxError(_End);
        }
        nextToken();
        if(token->id != _Function) {
            return syntaxError(_Function);
        }
        nextToken();

        // pop nested tabulky
        symbolTableActual = hTfree(symbolTableActual);

        // bez parametru hodis si hodnotu ze zasobniku do promene RETURN
        generateSceletInstruction(_END_FUNCTION, NULL);

        return true;
    }

    // <declare> -> DIM SHARED id AS <type> <definition>
    else if(token->id == _Dim) {
        nextToken();
        if(token->id != _Shared) {
            return syntaxError(_Shared);
        }

        nextToken();
        if(token->id != _id) {
            return syntaxError(_id);
        }

        // kontorla existence globani promenene se stejnym jmenem
        pItem oldVar = hTabFind(symbolTableActual, token->cPointer);
        if(oldVar != NULL && oldVar->visibilitySignature == symbolTableActual->visibilitySignature) {
            errorMsg(ERROR_SYN, token->column, "Opetovna definice globalni promnene %s", token->cPointer);
        }

        //vytvoreni noveho zaznamu o promenne
        pData newHashData = initData(token);
        hTabLookupAdd(symbolTableActual, newHashData);

        generateSceletInstruction(_DECLARE_VAR, initTerm(token));

        nextToken();
        if(token->id != _As) {
            return syntaxError(_As);
        }
        nextToken();
        return (LL_type(&newHashData->type) && LL_definition(&newHashData));

    }

    return syntaxError(_NULL);

}

bool LL_paramList(pData **newTypeArray, int *size) {
    //simulace pravidla 7
    if(token->id == _id) {

        return(LL_param(newTypeArray, size) && LL_paramNext(newTypeArray, size));

    }
    // simulace pravidla 8
    else if(token->id == _right_bracket) {


        return true;
    }
    return syntaxError(_NULL);
}

bool LL_paramNext(pData **newTypeArray, int *size) {
    //simulace pravidla 10
    if(token->id == _right_bracket) {

        return true;
    }
    //simulace pravidla 9
    else if(token->id == _colon) {



        nextToken();

        return(LL_param(newTypeArray, size) && LL_paramNext(newTypeArray, size));

    }
    return syntaxError(_NULL);
}

bool LL_param(pData **newTypeArray, int *size) {
    //simulace pravidla 11
    if(token->id == _id) {



        //inkrementace velikosti
        (*size) ++;

        //realokace pole typů
        *newTypeArray = createParamArray(*size, *newTypeArray);

        //kontrola kolize se jmeny funkci
        pItem funcHash;
        if((funcHash = hTabFind(symbolTableActual, token->cPointer)) != NULL) {
            errorMsg(ERROR_SEM_UNDEF, token->column, "Kolize parametru se jmenem deklarovane/definovane funkce %s()", token->cPointer);
        }

        //vytvoreni noveho zaznamu o promenne
        pData newHashData = initData(token);

        //vlozeni do priparavovaneho pole typu
        //ekvenci kontrola, ze jiz stejny parametr neni deklarovan
        for(int i = 0; i < *size - 1; i++) {
            if(!strcmp((*newTypeArray)[i]->token->cPointer, newHashData->token->cPointer)) {
                errorMsg(ERROR_SEM_UNDEF, token->column , "Redefinice parametru '%s' funkce", token->cPointer);
            }

        }
        (*newTypeArray)[*size - 1] = newHashData;

        nextToken();
        if(token->id != _As) {
            return syntaxError(_As);
        }
        nextToken();

        return LL_type(&newHashData->type);
    }
    return syntaxError(_NULL);
}

bool LL_stat(pItem funcItem) {
    //simulace pravidla 21
    if(token->id == _EOL || token->id == _End) {


        return true;
    }

    //<stat> -> <body>
    else if(token->id == _Scope) {
        return LL_body(funcItem);
    }

    //<stat> -> id = <expr>
    else if(token->id == _id ) {


        char* varName = token->cPointer;
        cToken tokenMem = token;
        cToken operator = NULL;
        pItem varHash;
        pItem tokenHelper;

        if((varHash = hTabFind(symbolTableActual, varName)) == NULL) {
            errorMsg(ERROR_SEM_UNDEF, tokenMem->column, "Pokus o prirazeni do nedeklarovane promnene: '%s'", varName);
        }

        nextToken();
        if( token->id == _plus_equal    ||
            token->id == _minus_equal   ||
            token->id == _mul_equal     ||
            token->id == _div_equal     ||
            token->id == _mod_equal     ) {

                tokenHelper = hTabFind(symbolTableActual, tokenMem->cPointer);
                generateArithmeticInstruction(_EXP, initTerm(tokenHelper->data->token), NULL);
                operator = token;
        }
        else if(token->id != _equal) {
            return syntaxError(_equal);
        }

        nextToken();

        // kontrola spravnych typu prirazeni, pripadna konverze
        symbolType expresionType = LL_expr();

        // unarni operatory
        if(operator != NULL) {
            switch (operator->id) {
                case _plus_equal:
                    if(tokenHelper->data->type == STRING || tokenHelper->data->type == DOUBLE || tokenHelper->data->type == INT) {
                        // shodnost
                        if(tokenHelper->data->type == expresionType) {
                            generateArithmeticInstruction(_ADD, initTerm(tokenHelper->data->token), createTerm(expresionType + 19));
                        }
                        else if(tokenHelper->data->type != STRING && expresionType != STRING){
                            expresionType = DOUBLE;
                            generateArithmeticInstruction(_ADD, initTerm(tokenHelper->data->token), createTerm(expresionType + 19));
                        }
                        else {
                            errorMsg(ERROR_SEM_TYPE, operator->column, "Neni mozne pricist operand k operandu typu retezec");
                        }
                    }
                    else {
                        errorMsg(ERROR_SEM_TYPE, operator->column, "Typove neplatny vyraz");
                    }

                    break;

                case _minus_equal:
                    if(tokenHelper->data->type == DOUBLE || tokenHelper->data->type == INT) {
                        // shodnost
                        if(tokenHelper->data->type == expresionType) {
                            generateArithmeticInstruction(_SUB, initTerm(tokenHelper->data->token), createTerm(expresionType + 19));
                        }
                        else {
                            expresionType = DOUBLE;
                            generateArithmeticInstruction(_SUB, initTerm(tokenHelper->data->token), createTerm(expresionType + 19));
                        }
                    }
                    else {
                        errorMsg(ERROR_SEM_TYPE, operator->column, "Typove neplatny vyraz");
                    }

                    break;

                case _mul_equal:
                    if(tokenHelper->data->type == DOUBLE || tokenHelper->data->type == INT) {
                        // shodnost
                        if(tokenHelper->data->type == expresionType) {
                            generateArithmeticInstruction(_MUL, initTerm(tokenHelper->data->token), createTerm(expresionType + 19));
                        }
                        else {
                            expresionType = DOUBLE;
                            generateArithmeticInstruction(_MUL, initTerm(tokenHelper->data->token), createTerm(expresionType + 19));
                        }
                    }
                    else {
                        errorMsg(ERROR_SEM_TYPE, operator->column, "Typove neplatny vyraz");
                    }
                    break;

                case _div_equal:
                    if(tokenHelper->data->type == DOUBLE || tokenHelper->data->type == INT) {
                        // shodnost
                        if(tokenHelper->data->type == expresionType) {
                            generateArithmeticInstruction(_DIV, initTerm(tokenHelper->data->token), createTerm(expresionType + 19));
                        }
                        else {
                            expresionType = DOUBLE;
                            generateArithmeticInstruction(_DIV, initTerm(tokenHelper->data->token), createTerm(expresionType + 19));
                        }
                    }
                    else {
                        errorMsg(ERROR_SEM_TYPE, operator->column, "Typove neplatny vyraz");
                    }
                    break;

                case _mod_equal:
                    if(tokenHelper->data->type == DOUBLE || tokenHelper->data->type == INT) {
                        // shodnost
                        if(tokenHelper->data->type == expresionType) {
                            generateArithmeticInstruction(_MOD, initTerm(tokenHelper->data->token), createTerm(expresionType + 19));
                        }
                        else {
                            expresionType = DOUBLE;
                            generateArithmeticInstruction(_MOD, initTerm(tokenHelper->data->token), createTerm(expresionType + 19));
                        }
                    }
                    else {
                        errorMsg(ERROR_SEM_TYPE, operator->column, "Typove neplatny vyraz");
                    }

                default:
                    break;
            }
        }

        if(expresionType != varHash->data->type) {
            if(varHash->data->type != STRING && expresionType != BOOL && expresionType != STRING) {
                //pretrypovani vrcholu zasobniku
                generateChangeTypeInstruction(0);

                //implicitni konverze (zaokrouhleni)
                if(expresionType == DOUBLE) {
                    warningMsg(tokenMem->column, "Byla provedena implicitni konverze vysledku vyrazu pri prirazeni do promene %s", varName);
                }
            }
            else {
                errorMsg(ERROR_SEM_TYPE, tokenMem->column, "Pokus o prirazeni do promenene '%s' definovanou typem %s hodnotu typu %s.\n\tJedna se o nepovolenou implicitni konverzi", varName, varType[varHash->data->type], varType[expresionType]);
            }
        }

        varHash->data->defined = true;

        // ze zasobniku na 1-arg (var) (definice)
        generateSceletInstruction(_MOVE, initTerm(varHash->data->token));

        return true;
    }

    //<stat> -> INPUT id
    else if(token->id == _Input) {





        nextToken();
        if(token->id != _id) {
            return syntaxError(_id);
        }



        // promena je definovana v hTable()
        char* varName = token->cPointer;
        pItem varHash;

        if((varHash = hTabFind(symbolTableActual, varName)) == NULL) {
            errorMsg(ERROR_SEM_UNDEF, token->column, "Pokus o prirazeni do nedeklarovane promnene: '%s'", varName);
        }
        else {
            varHash->data->defined = true;
        }

        // 1-arg -> vytisknout
        generateSceletInstruction(_READ, initTerm(varHash->data->token));

        nextToken();
        return true;
    }

    //<stat> -> PRINT <expr> ; <exprList>
    else if(token->id == _Print) {



        nextToken();
        if(!LL_expr()) {
            return syntaxError(_NULL);
        }
        if(token->id != _semicolon) {
            return syntaxError(_semicolon);
        }
        nextToken();
        return LL_exprList();
    }

    //<stat> -> IF <expr> THEN EOL <statList> <ifstat> END IF
    else if(token->id == _If) {



        nextToken();
        int column = token->column;
        if(LL_expr() != BOOL) {
            return errorMsg(ERROR_SEM_TYPE, column, "Vyrazem neni pravdivostni hodnota" );
        }

        // test vrcholu zasobniku
        generateSceletInstruction(_IF, NULL);

        if(token->id != _Then) {
            return syntaxError(_Then);
        }

        // vytvoreni noveho zanoreni
        //printHashTable(symbolTableActual);
        symbolTableActual = nestedTable(symbolTableActual);
        createFrame();


        //printHashTable(symbolTableActual);

        nextToken();
        if(token->id != _EOL) {
            return syntaxError(_EOL);
        }
        nextToken();
        if(!LL_statList(funcItem)) {
            return syntaxError(_NULL);
        }

        if(!LL_ifstat(funcItem)) {
            return syntaxError(_NULL);
        }

        return true;
    }

    //simulace pravidla 19
    else if(token->id == _Do) {



        nextToken();
        if(token->id != _While) {
            return syntaxError(_While);
        }

        // vytvoreni labelu pro while
        generateSceletInstruction(_CREATE_WHILE, NULL);

        nextToken();
        if(LL_expr() != BOOL) {
            return errorMsg(ERROR_SEM_TYPE, column, "Vyrazem neni pravdivostni hodnota" );
        }

        // test vrcholu zasobniku pro while
        generateSceletInstruction(_WHILE, NULL);

        // vytvoreni noveho zanoreni
        symbolTableActual = nestedTable(symbolTableActual);
        createFrame();

        if(token->id != _EOL) {
            return syntaxError(_EOF);
        }
        nextToken();
        if(!LL_statList(funcItem)) {
            return syntaxError(_NULL);
        }
        if(token->id != _Loop) {
            return syntaxError(_Loop);
        }

        // odebrani zanoreni
        destroyFrame();
        symbolTableActual = hTfree(symbolTableActual);
        generateSceletInstruction(_LOOP, NULL);

        nextToken();
        return true;
    }

    //simulace pravidla 20 (*)
    else if(token->id == _Return) {

        if(funcItem != NULL) {


            nextToken();

            symbolType expresionType = LL_expr();

            if(expresionType < STRING || expresionType > BOOL) {
                error(ERROR_INTER, "Interni chyba pri vypoctu vyrazu");
            }

            if(expresionType != funcItem->data->type) {
                if(funcItem->data->type != STRING && expresionType != BOOL && expresionType != STRING) {
                    //implicitni konverze (zaokrouhleni)

                    if(expresionType == DOUBLE) {
                        warningMsg(token->column, "Byla provedena implicitni konverze vysledku vyrazu pri prirazeni navratu z funkce %s()", funcItem->data->token->cPointer);
                    }

                    //pretrypovani vrcholu zasobniku
                    generateChangeTypeInstruction(0);

                }
                else {
                    errorMsg(ERROR_SEM_TYPE, token->column,"Pokus o volani return z funkce  %s() hodnotou typu %s, kde navratovy typ funkce ovsem je %s\n\t nelze provezt implicitni konverzi datovych typu", funcItem->data->token->cPointer, varType[expresionType], varType[funcItem->data->type]);
                }
            }

            // popni si hodnotu ze zasobniku do promnene RETURN
            generateSceletInstruction(_RETURN, NULL);

            return true;

        }
        else {
            errorMsg(ERROR_SYN, token->column, "Volani Return uvnitr tela programu neni povoleno");
        }

    }

    //simulace pravidla 14
    else if(token->id == _Dim) {



        return LL_declareVar();
    }
    return syntaxError(_NULL);
}

bool LL_statList(pItem funcItem) {
    //simulace pravidla 12
    if( token->id == _EOL           ||
        token->id == _id            ||
        token->id == _Input         ||
        token->id == _Print         ||
        token->id == _If            ||
        token->id == _Do            ||
        token->id == _Return        ||
        token->id == _Dim           ||
        token->id == _Scope         ) {


            if(!LL_stat(funcItem)) {
                return syntaxError(_NULL);
            }
            if(token->id != _EOL) {
                return syntaxError(_NULL);
            }
            nextToken();

            return LL_statList(funcItem);

    }

    //simulace pravidla 13
    else if(token->id == _End || token->id == _Else || token->id == _Loop || token->id == _Elseif) {


        return true;
    }

    return syntaxError(_NULL);
}

bool LL_exprList() {

    // zasobnik 1 -> vytisknout
    generateSceletInstruction(_PRINT, NULL);

    //simulace pravidla 24
    if(token->id == _EOL) {



        return true;
    }

    //simulace pravidla 23
    else if(token->id == _id            ||
            token->id == _left_bracket  ||
            token->id == _celoc_lit     ||
            token->id == _celoc_lit_exp ||
            token->id == _des_lit       ||
            token->id == _des_lit_exp   ||
            token->id == _ret_lit       ||
            token->id == _minus         || //unarni minus
            token->id == _Length        ||
            token->id == _SubStr        ||
            token->id == _Asc           ||
            token->id == _Chr ) {



                if(!LL_expr()) {
                    return syntaxError(_NULL);
                }
                if(token->id != _semicolon) {
                    return syntaxError(_semicolon);
                }

                nextToken();
                return LL_exprList();
    }

    return syntaxError(_NULL);
}

//TODO Errors
symbolType LL_expr() {

    // simuluje precedencni syntakticky analyzator pro vyhodnoceni vyrazu,
    // pocitame jiz s rozsirenim FUNEXP


    struct Term *actualTerm     = NULL;                                // vytvoreni obalujiciho termu pro precedenci analyzu
    struct Term *dolarTerm      = createTerm(__DOLLAR);                // pomocny ukazatel na term dollar
    struct Term *handleTerm     = createTerm(__HANDLE);                // pomocny ukazatel na term handle
    struct Term *exprDoubleTerm = createTerm(__EXPR_DOUBLE);           // pomocny ukazatel na term expr_double
    struct Term *exprIntTerm    = createTerm(__EXPR_INT);              // pomocny ukazatel na term expr_int
    struct Term *exprStringTerm = createTerm(__EXPR_STRING);           // pomocny ukazatel na term expr_string
    struct Term *exprBoolTerm   = createTerm(__EXPR_BOOL);             // pomocny ukazatel na term expr_bool

    if( exprStringTerm  == NULL ||
        exprIntTerm     == NULL ||
        exprDoubleTerm  == NULL ||
        handleTerm      == NULL ||
        dolarTerm       == NULL) {
            error(ERROR_INTER, "Chyba pri alokaci termu");
        }

    dListPointer exprList = NULL;         // ukazatel na list pro simulaci vypoctu vyrazu podle precedencni tabulky

    // inicializace obou listu listu
    if(!initDList(&exprList)) {
        error(ERROR_INTER, "Chyba pri vytvareni instrListu a exprListu pro simulaci vypoctu vyrazu");
    }

    // vlozeni $ jako prvni na zasobnik
    insertFirstTerm(exprList, dolarTerm);

    //obaleni prvotniho tokenu
    actualTerm = initTerm(token);

    while(true) {

        // predavani informace o aktualnim token do fakeTokenu
        if(actualTerm->termType != __DOLLAR) {
            actualTerm = initTerm(token);
        }

        // klicova slova -> length, asc atd...
        if(actualTerm->termType == __UNDEF) {

            // zmena lenght
            if( actualTerm->token->id == _Length    ||
                actualTerm->token->id == _Asc       ||
                actualTerm->token->id == _SubStr    ||
                actualTerm->token->id == _Chr ) {
                    if(actualTerm->token->id == _Length) {
                        actualTerm->token->cPointer = "length";
                    }
                    if(actualTerm->token->id == _Asc) {
                        actualTerm->token->cPointer = "asc";
                    }
                    if(actualTerm->token->id == _SubStr) {
                        actualTerm->token->cPointer = "substr";
                    }
                    if(actualTerm->token->id == _Chr) {
                        actualTerm->token->cPointer = "chr";
                    }

                    actualTerm->token->id = _id;
                    actualTerm->termType = __ID;
                }
                else {
                    errorMsg(ERROR_SYN, actualTerm->token->column,"Neocekavany token na vstupu");
                }
        }

        // kontrola zda-li je token na vstupu soucasti <expr>
        if(actualTerm->termType >= __LEFT_BRACKET && actualTerm->termType <= __STRING) {

            // ziskani vrchniho terminalu
            enumTerm topTerminal = findFirstTerminal(exprList);

            // ziskani informace zda shiftovat ci nikoliv
            precedenc helper = precedencTable[topTerminal][actualTerm->termType];



            // simulace =
            if(helper == EQUA) {
                insertFirstTerm(exprList, actualTerm);
            }

            // simulace < vytvarim handle 0
            else if(helper == LESS) {
                // ziskani prvniho terminalu
                enumTerm firstTerminal = findFirstTerminal(exprList);

                // vlozeni handle pred nalzeny terminal
                preInsertTerm(exprList, handleTerm, firstTerminal);

                // vlozeni terminalu ze vstupu do listu
                if(actualTerm->termType == __ID) {

                    // prohledani tabulky symbolu zda-li se jedna o identifikator deklarovane funkce
                    pItem varHash;
                    char* varName = actualTerm->token->cPointer;

                    if((varHash = hTabFind(symbolTableActual, varName)) == NULL) {
                        errorMsg(ERROR_SEM_UNDEF, actualTerm->token->column,"Pouziti nedeklarovane promnene '%s'", actualTerm->token->cPointer);
                    }

                    if(varHash->data->token->id == _func) {
                       actualTerm->termType = __FUNC;
                    }

                }


                insertFirstTerm(exprList, actualTerm);

            }

            // simulace > vyhodnocuji handle 3
            else if(helper == GREA) {
                expresionEnum instruction = -1;    // ulozeni pouzivane instrukce
                pTerm arg1 = NULL, arg2 = NULL;
                pTerm operatorTerm;

                // obsahuje handle start, je moznost redukovat
                if(findFirstDItem(exprList, __HANDLE) != NULL) {

                    pTerm popedTerm;


                    // unary minus
                    if(actualTerm->termType == __MINUS && (topTerm(exprList) < __ID && topTerm(exprList) >= __MULTIPLY)) {
                        insertFirstTerm(exprList, handleTerm);
                        insertFirstTerm(exprList, actualTerm);
                    }

                    else {
                        // simulace pravidla 1
                        if((popedTerm = popFirstTerm(exprList))->termType >= __ID && popedTerm->termType <= __STRING) {

                            if(popedTerm->termType == __ID) {
                                pItem varHash;
                                char* varName = popedTerm->token->cPointer;

                                if((varHash = hTabFind(symbolTableActual, varName)) != NULL) {
                                    if(!varHash->data->defined) {
                                        warningMsg(popedTerm->token->column, "Prace s nedefinovanou promnenou '%s', pouzita implicitni definice", varName);
                                    }

                                    switch (varHash->data->type) {
                                        case INT:
                                            handleOnTop(exprList);
                                            insertFirstTerm(exprList, exprIntTerm);
                                            break;
                                        case DOUBLE:
                                            handleOnTop(exprList);
                                            insertFirstTerm(exprList, exprDoubleTerm);
                                            break;
                                        case STRING:
                                            handleOnTop(exprList);
                                            insertFirstTerm(exprList, exprStringTerm);
                                            break;
                                        default:
                                            error(ERROR_INTER, "Chyba prevodu typu promene do precedencni tabulky");
                                            break;
                                    }
                                }
                                else {
                                    error(ERROR_INTER, "Chyba pri simulaci pravidla 1 ve vypoctu vyrazu");
                                }
                            }
                            else {

                                switch (popedTerm->termType) {
                                    case __INT:
                                        handleOnTop(exprList);
                                        insertFirstTerm(exprList, exprIntTerm);
                                        break;
                                    case __DOUBLE:
                                        handleOnTop(exprList);
                                        insertFirstTerm(exprList, exprDoubleTerm);
                                        break;
                                    case __STRING:
                                        handleOnTop(exprList);
                                        insertFirstTerm(exprList, exprStringTerm);
                                        break;
                                    default:
                                        //TODO ERROR
                                        break;
                                }
                            }

                            arg1 = popedTerm;


                            //12 translate to expr
                            generateArithmeticInstruction(_EXP, arg1, NULL);


                            continue;
                        }

                        //simulace pravidel 2 - 12
                        else if(popedTerm->termType >= __INT && popedTerm->termType <= __EXPR_INT) {

                            // ulozeni prvni argumentu pro instrukci
                            arg1 = popedTerm;

                            popedTerm = popFirstTerm(exprList);
                            operatorTerm = popedTerm;

                            switch (popedTerm->termType) {

                                //simulace pravidla 2
                                case __MULTIPLY:


                                    if((popedTerm = popFirstTerm(exprList))->termType < __INT || popedTerm->termType > __EXPR_INT) {

                                        syntaxError(_EXPR);
                                    }

                                    // ukladani arg2
                                    arg2 = popedTerm;

                                    // ulozeni pouzivaneho pravidla
                                    instruction = _MUL;




                                    break;

                                //simulace pravidla 3
                                case __BACK_SLASH:

                                    if((popedTerm = popFirstTerm(exprList))->termType < __INT || popedTerm->termType > __EXPR_INT) {

                                        syntaxError(_EXPR);
                                    }

                                    // ukladani arg2
                                    arg2 = popedTerm;

                                    // ulozeni pouzivaneho pravidla
                                    instruction = _MOD;


                                    break;

                                //simulace pravidla 4
                                case __FORWARD_SLASH:

                                    if((popedTerm = popFirstTerm(exprList))->termType < __INT || popedTerm->termType > __EXPR_INT) {

                                        syntaxError(_EXPR);
                                    }

                                    // ukladani arg2
                                    arg2 = popedTerm;

                                    // ulozeni pouzivaneho pravidla
                                    instruction = _DIV;


                                    break;

                                //simulace pravidla 5
                                case __PLUS:

                                    if((popedTerm = popFirstTerm(exprList))->termType < __INT || popedTerm->termType > __EXPR_INT) {

                                        syntaxError(_EXPR);
                                    }

                                    // ukladani arg2
                                    arg2 = popedTerm;

                                    // ulozeni pouzivaneho pravidla
                                    instruction = _ADD;


                                    break;

                                //simulace pravidla 6
                                case __MINUS:

                                    if((popedTerm = popFirstTerm(exprList))->termType < __INT || popedTerm->termType > __EXPR_INT) {


                                        insertFirstTerm(exprList, popedTerm);

                                        // jedna se o unary minus TODO
                                        instruction = _UNARY_MINUS;
                                        arg2 = NULL;

                                    }
                                    else {
                                        // ukladani arg2
                                        arg2 = popedTerm;

                                        // ulozeni pouzivaneho pravidla
                                        instruction = _SUB;

                                    }
                                    break;

                                //simulace pravidla 7
                                case __LESS:

                                    if((popedTerm = popFirstTerm(exprList))->termType < __INT || popedTerm->termType > __EXPR_INT) {

                                        errorMsg(ERROR_SEM_TYPE, operatorTerm->token->column,"Neocekavany typ operandu");
                                    }

                                    // ukladani arg2
                                    arg2 = popedTerm;

                                    // ulozeni pouzivaneho pravidla
                                    instruction = _LESS;


                                    break;

                                //simulace pravidla 8
                                case __GREATER:

                                    if((popedTerm = popFirstTerm(exprList))->termType < __INT || popedTerm->termType > __EXPR_INT) {

                                        errorMsg(ERROR_SEM_TYPE, operatorTerm->token->column,"Neocekavany typ operandu");
                                    }

                                    // ukladani arg2
                                    arg2 = popedTerm;

                                    // ulozeni pouzivaneho pravidla
                                    instruction = _GREATER;


                                    break;

                                //simulace pravidla 9
                                case __EQUAL:

                                    if((popedTerm = popFirstTerm(exprList))->termType < __INT || popedTerm->termType > __EXPR_INT) {

                                        errorMsg(ERROR_SEM_TYPE, operatorTerm->token->column,"Neocekavany typ operandu");
                                    }

                                    // ukladani arg2
                                    arg2 = popedTerm;

                                    // ulozeni pouzivaneho pravidla
                                    instruction = _EQUAL;


                                    break;

                                //simulace pravidla 10
                                case __LESS_EQUAL:

                                    if((popedTerm = popFirstTerm(exprList))->termType < __INT || popedTerm->termType > __EXPR_INT) {

                                        errorMsg(ERROR_SEM_TYPE, operatorTerm->token->column,"Neocekavany typ operandu");
                                    }

                                    // ukladani arg2
                                    arg2 = popedTerm;

                                    // ulozeni pouzivaneho pravidla
                                    instruction = _LESSEQUAL;


                                    break;

                                //simulace pravidla 11
                                case __GREATER_EQUAL:

                                    if((popedTerm = popFirstTerm(exprList))->termType < __INT || popedTerm->termType > __EXPR_INT) {

                                        errorMsg(ERROR_SEM_TYPE, operatorTerm->token->column,"Neocekavany typ operandu");
                                    }

                                    // ukladani arg2
                                    arg2 = popedTerm;

                                    // ulozeni pouzivaneho pravidla
                                    instruction = _GREATEREQUAL;


                                    break;

                                //simulace pravidla 11
                                case __NOT_EQUAL:

                                    if((popedTerm = popFirstTerm(exprList))->termType < __INT || popedTerm->termType > __EXPR_INT) {

                                        errorMsg(ERROR_SEM_TYPE, operatorTerm->token->column,"Neocekavany typ operandu");
                                    }

                                    // ukladani arg2
                                    arg2 = popedTerm;

                                    // ulozeni pouzivaneho pravidla
                                    instruction = _NOTEQUAL;


                                    break;
                                default:
                                    error(ERROR_INTER, "Chyba pri precedencni analyze");
                            }


                        }

                        //simulace pravidla 13
                        else if(popedTerm->termType == __RIGHT_BRACKET) {
                            int errorHelper = popedTerm->token->column;

                            int itemCount = -1;

                            // posledni typ parametru funkce (na zasobniku jsou obracene)
                            struct typeOfParam *lastParam = NULL;

                            while(((popedTerm = popFirstTerm(exprList))->termType >= __INT && popedTerm->termType <= __EXPR_INT) || popedTerm->termType == __EXPR_BOOL) {

                                symbolType paramType = popedTerm->termType - 19;
                                lastParam = createTypeOfParam(paramType, lastParam);

                                itemCount ++;

                                // vkladani nasledujiciho parametru do instructionListu
                                arg1 = popedTerm;

                                if((popedTerm = popFirstTerm(exprList))->termType == __COLON) {
                                    continue;
                                }
                                else if(popedTerm->termType == __LEFT_BRACKET) {
                                    break;
                                }
                                else {
                                    error(ERROR_SYN, "Nevalidni token typu %d pro volani funkce", helperTable[popedTerm->termType]);
                                }

                            }
                            // nasleduje ID -> kontrola jestli je to identifikator funkce jinak chyba
                            if((popedTerm = popFirstTerm(exprList))->termType == __FUNC) {
                                // vkladani druheho oprandu do instructionListu
                                arg1 = popedTerm;
                                arg2 = NULL;

                                instruction = _FUNC;

                                // kontrola existence definovane funkce
                                pItem varHash;
                                char* varName = popedTerm->token->cPointer;

                                if((varHash = hTabFind(symbolTableGlobal, varName)) == NULL) {
                                    errorMsg(ERROR_SEM_UNDEF, popedTerm->token->column, "Volani nedeklarovane funkce '%s()'", varName);
                                }
                                if(!varHash->data->defined) {
                                    varHash->forcedDefinition = true;
                                }
                                // kontrola poctu argumentu s definovanou funkci
                                if((int)varHash->data->paramCount != itemCount + 1) {
                                    errorMsg(ERROR_SEM_TYPE, popedTerm->token->column, "Volana funkce '%s()'\n\tpozaduje %d argumentu namisto %d", varName, varHash->data->paramCount, itemCount + 1);
                                }

                                // kontrola shodnosti parametru s parametry volane funcke, pripadne pokus o jejich pretypovani
                                for(int i = 0; i <= itemCount; i++) {
                                    symbolType actualParam = lastParam->type;
                                    lastParam = removeFirstParam(lastParam);

                                    // rozdilne parametry (zadane, definovane)
                                    if(actualParam != varHash->data->paramType[i]->type) {
                                        if(actualParam != STRING && varHash->data->paramType[i]->type != STRING) {

                                            generateChangeTypeInstruction(itemCount - i);

                                            // upozorneni na mozne ztraceni presnosti
                                            if(actualParam == DOUBLE) {

                                                // pretypovani parametru na zasobniku instrukci, volani za pomoci promnene
                                                warningMsg(popedTerm->token->column, "Pri volani funkce Funkce '%s()\n\tbyla provedena implicitni typova konverze z typu Double na Integer", varName);
                                            }
                                        }
                                        else {
                                            errorMsg(ERROR_SEM_TYPE, popedTerm->token->column, "Funkce '%s()'\n\tpozaduje na %d. pozici paramater typu: %s, namisto: %s\n\tneni mozne provest implicitni konverzi typu", varName, i + 1,  varType[varHash->data->paramType[i]->type], varType[actualParam]);
                                        }

                                    }

                                }

                                switch (varHash->data->type) {
                                    case INT:
                                        handleOnTop(exprList);
                                        insertFirstTerm(exprList, exprIntTerm);
                                        break;
                                    case DOUBLE:
                                        handleOnTop(exprList);
                                        insertFirstTerm(exprList, exprDoubleTerm);
                                        break;
                                    case STRING:
                                        handleOnTop(exprList);
                                        insertFirstTerm(exprList, exprStringTerm);
                                        break;
                                    default:
                                        //TODO ERROR
                                        break;
                                }

                                // volani fce pro vytvoreni instrukce
                                generateArithmeticInstruction(instruction, arg1, arg2);

                                continue;
                            }

                            // simulace pravidla 14 -> treba vratit posledni token(handle start) pouze pokud uvnitr zavorek bylo <expr> a ne <expr>,<expr>
                            else if(popedTerm->termType == __HANDLE && itemCount == 0) {
                                insertFirstTerm(exprList, arg1);
                                continue;
                                //nevytvari se instrukce, pouha redukce zavorek
                            }

                            else {
                                errorMsg(ERROR_SYN, errorHelper + 1, "Neocekavany operand operace %d", popedTerm->termType);
                            }
                        }

                        else{
                            errorMsg(ERROR_SYN, popedTerm->token->column + 1, "Neocekavany znak ve vypoctu vyrazu");
                        }

                        //kontrola handle na zasobniku
                        handleOnTop(exprList);

                        int typesAnalyzer = 0;
                        if(arg1 != NULL && arg2 != NULL) {
                            // kontrola typu pro aritmeticke operace
                            typesAnalyzer = arg1->termType * arg2->termType;
                        }
                        switch (instruction) {
                            case _UNARY_MINUS:
                                if(arg1->termType == __EXPR_DOUBLE) {
                                    insertFirstTerm(exprList, exprDoubleTerm);
                                }
                                else if(arg1->termType == __EXPR_INT) {
                                    insertFirstTerm(exprList, exprIntTerm);
                                }
                                else {
                                    errorMsg(ERROR_SYN, operatorTerm->token->column , "Unarni operand minus nelze provadet s operandem typu %s", operationType[instruction], varType[arg1->termType - 20]);
                                }
                                break;

                            case _ADD:
                                if(INTS) {
                                    insertFirstTerm(exprList, exprIntTerm);
                                }

                                else if(DOUBLES || NOT_STRING) {
                                    insertFirstTerm(exprList, exprDoubleTerm);
                                }
                                else if(STRINGS) {
                                    insertFirstTerm(exprList, exprStringTerm);
                                }
                                else {
                                    errorMsg(ERROR_SEM_TYPE, operatorTerm->token->column, "Operace %s nelze provadet mezi operandy typu %s a %s", operationType[instruction], varType[arg1->termType - 19], varType[arg2->termType - 19]);
                                }
                               break;

                            case _SUB:
                            case _MUL:
                                if(INTS) {
                                    insertFirstTerm(exprList, exprIntTerm);
                                }
                                else if(DOUBLES || NOT_STRING) {
                                    insertFirstTerm(exprList, exprDoubleTerm);
                                }
                                else {
                                    errorMsg(ERROR_SEM_TYPE, operatorTerm->token->column , "Operace %s nelze provadet mezi operandy typu %s a %s", operationType[instruction], varType[arg1->termType - 19], varType[arg2->termType - 19]);
                                }
                                break;

                            case _LESS:
                            case _EQUAL:
                            case _GREATER:
                            case _NOTEQUAL:
                            case _LESSEQUAL:
                            case _GREATEREQUAL:
                                if(INTS || DOUBLES || NOT_STRING || STRINGS) {
                                    insertFirstTerm(exprList, exprBoolTerm);
                                }
                                else {
                                    errorMsg(ERROR_SYN, operatorTerm->token->column, "Operace %s nelze provadet mezi operandy typu %s a %s", operationType[instruction], varType[arg1->termType - 20], varType[arg2->termType - 20]);
                                }

                                break;

                            case _DIV:
                                if(INTS || DOUBLES || NOT_STRING) {
                                    insertFirstTerm(exprList, exprDoubleTerm);
                                }
                                else {
                                    errorMsg(ERROR_SYN, operatorTerm->token->column, "Operace %s nelze provadet mezi operandy typu %s a %s", operationType[instruction], varType[arg1->termType - 20], varType[arg2->termType - 20]);
                                }
                                break;

                            case _MOD:
                                if(INTS) {
                                    insertFirstTerm(exprList, exprIntTerm);
                                }
                                else if(NOT_STRING || DOUBLES) {
                                    warningMsg(operatorTerm->token->column, "Mezi operandy typu %s a %s pri operaci celociselneho deleni\n\tbyla provedena implicitni typova konverze", varType[arg1->termType - 20], varType[arg2->termType - 20]);
                                    insertFirstTerm(exprList, exprIntTerm);
                                }
                                else {
                                    errorMsg(ERROR_SYN, operatorTerm->token->column,"Operace %s nelze provadet mezi operandy typu %s a %s", operationType[instruction], varType[arg1->termType], varType[arg2->termType]);
                                }
                                break;

                            default:
                                error(ERROR_INTER, "Neocekavany operand aritmeticke operace");
                        }

                        // volani fce pro vytvoreni instrukce
                        generateArithmeticInstruction(instruction, arg1, arg2);

                        continue;
                    }
                }

                // nenalezlo zactek handle -> chyba
                else {
                    return syntaxError(_NULL);
                }
            }

            else if(helper == NONE) {

                // kontrola jestli je na vrcholu zasobniku
                // $ a jestli je na vstupu $, pote se jedna o validni vyraz
                if(findFirstTerminal(exprList) && topTerminal == __DOLLAR && size(exprList) == 2) {
                    switch (popFirstTerm(exprList)->termType) {
                        case __EXPR_INT:
                            return INT;
                        case __EXPR_STRING:
                            return STRING;
                        case __EXPR_DOUBLE:
                            return DOUBLE;
                        case __EXPR_BOOL:
                            return BOOL;
                        default:
                            //printHashTable(symbolTableActual);
                            error(ERROR_INTER, "Chyba pri predavani vysledku vyrazu");
                        }
                }

                if(topTerminal == __FUNC) {
                    errorMsg(ERROR_SEM_UNDEF,0 , "Chybne volani funkce bez zavorek");
                }
                else {
                    errorMsg(ERROR_SYN, 0, "Chybny token [%s] ve vyrazu", keywords[token->id]);
                }
            }
        }
        else {
            syntaxError(_NULL);
        }

        // kontrola dalsiho tokenu
        // nasledujici token je platnym tokenem z mnoziny FOLLOW(<expr>)
        if(actualTerm->termType != __DOLLAR) {
            nextToken();
        }

        if( token->id == _EOL           ||
            token->id == _semicolon     ||
            token->id == _Then) {
                // uspech
                actualTerm = dolarTerm;
        }

        // neplatny token ve FOLLOW <expr>
        else if((token->id < _left_bracket || token->id > _ret_lit) && (token->id != _Asc && token->id != _Length && token->id != _SubStr && token->id != _Chr)){
            return syntaxError(_NULL);
        }
    }

    return syntaxError(_NULL);
}

bool LL_declareVar() {
    //simulace pravidla 34
    if(token->id == _Dim) {


        nextToken();
        if(token->id != _id) {
            return syntaxError(_id);
        }

        // kontorla existence lokalni promenene
        pItem oldVar = hTabFind(symbolTableActual, token->cPointer);

        if(oldVar != NULL) {
            if(oldVar->visibilitySignature == symbolTableActual->visibilitySignature) {
                errorMsg(ERROR_SEM_UNDEF, token->column, "Opetovna definice promnene %s", token->cPointer);
            }
            else if(oldVar->data->token->id == _func) {
                errorMsg(ERROR_SEM_UNDEF, token->column, "Pokus o definovani promene %s cimz by doslo k redefinici funkce %s()", token->cPointer, token->cPointer);
            }
        }

        //vytvoreni noveho zaznamu o promenne
        pData newHashData = initData(token);
        hTabLookupAdd(symbolTableActual, newHashData);

        cToken tokenMem = token;

        nextToken();
        if(token->id != _As) {
            return syntaxError(_As);
        }
        nextToken();

        if(!LL_type(&newHashData->type)) {
            error(ERROR_INTER, "Chyba pri analyze noveho typu funkce");
        }

        // 1-arg varname -> podivat se do tabulky kam uz to mira musel dat jinak je kokot, podle toho movnout implicitni hodnotu
        if(oldVar == NULL) {
            generateSceletInstruction(_DECLARE_VAR, initTerm(tokenMem));
        }
        else {
            // prvne nahrat TF do LF, nevytvaret novou promnenou, prepsat starou
            generateSceletInstruction(_COPY_OUT, initTerm(tokenMem));
            generateSceletInstruction(_INIT_VAR, initTerm(tokenMem));
        }

        if(!LL_definition(&newHashData)) {
            error(ERROR_SYN, "Chyba pri inicializaci promnene");
        }

        return true;
    }
    return syntaxError(_NULL);
}

bool LL_definition(pData *newData) {
    //simulace pravidla 36
    if(token->id == _EOL ) {

        return true;

    }
    //simulace pravidla 35
    else if(token->id == _equal) {

        cToken equalToken = token;



        nextToken();

        symbolType expresionType = LL_expr();

        if(expresionType == (*newData)->type) {
            (*newData)->defined = true;

            // ze zasobniku na 1-arg (var) (definice)
            generateSceletInstruction(_MOVE, initTerm((*newData)->token));

            return true;
        }
        else {
            if((*newData)->type != STRING && expresionType != BOOL && expresionType != STRING) {
                //implicitni konverze (zaokrouhleni)

                if(expresionType == DOUBLE) {
                    warningMsg(equalToken->column, "Byla provedena implicitni konverze vysledku vyrazu pri prirazeni do promene %s", (*newData)->token->cPointer);
                }
                (*newData)->defined = true;

                //pretrypovani vrcholu zasobniku
                generateChangeTypeInstruction(0);

                // ze zasobniku na 1-arg (var) (definice)
                generateSceletInstruction(_MOVE, initTerm((*newData)->token));

                return true;
            }
            else {
                errorMsg(ERROR_SEM_TYPE, equalToken->column,"Pokus o prirazeni do promenene '%s' definovanou typem %s hodnotu typu %s.\n\tJedna se o nepovolenou implicitni konverzi", (*newData)->token->cPointer, varType[(*newData)->type], varType[expresionType]);
            }
        }
    }

    return syntaxError(_NULL);
}

bool LL_type(symbolType *newType) {

    //simulace pravidla 37
    if(token->id == _Integer) {
        //prirazeni prametru do paramtypearray

        *newType = INT;
        nextToken();
        return true;
    }
    //simulace pravidla 38
    else if(token->id == _Double) {
        //prirazeni prametru do paramtypearray

        //
        *newType = DOUBLE;
        nextToken();
        return true;
    }
    //simulace pravidla 39
    else if(token->id == _String) {
        //prirazeni prametru do paramtypearray

        *newType = STRING;
        nextToken();
        return true;
    }

    return syntaxError(_NULL);
}

bool LL_space(bool allowScope) {
    // <space> -> ε

    if(token->id == _Scope || token->id == _Declare || token->id == _Function || token->id == _Dim || token->id == _EOF) {
        if(allowScope) {



        }
        else if(token->id != _EOF){
            errorMsg(ERROR_SYN, token->column, "Ocekavan byl token [EOF]");
        }

        return true;
    }

    // <space> -> EOL <space>
    else if(token->id == _EOL){


        nextToken();
        return LL_space(allowScope);
    }

    return syntaxError(_NULL);
}

bool LL_begin() {
    // simulace pravidla 40


    if(LL_space(true) && LL_program() && LL_space(false)) {
        return (token->id == _EOF);
    }
    return syntaxError(_NULL);
}

pTerm initTerm(cToken token) {
    pTerm newTerm;
    if((newTerm = malloc(sizeof(struct Term))) == NULL) {
        error(ERROR_INTER,"Chyba alokace pameti pTerm\n");
    }

    newTerm->token = token;

    switch (token->id) {
    case _left_bracket:
        newTerm->termType = __LEFT_BRACKET;
        break;
    case _right_bracket:
        newTerm->termType = __RIGHT_BRACKET;
        break;
    case _func:
        newTerm->termType = __FUNC;
        break;
    case _colon:
        newTerm->termType = __COLON;
        break;
    case _dollar:
        newTerm->termType = __DOLLAR;
        break;
    case _multiply:
        newTerm->termType = __MULTIPLY;
        break;
    case _forward_slash:
        newTerm->termType = __FORWARD_SLASH;
        break;
    case _back_slash:
        newTerm->termType = __BACK_SLASH;
        break;
    case _plus:
        newTerm->termType = __PLUS;
        break;
    case _minus:
        newTerm->termType = __MINUS;
        break;
    case _equal:
        newTerm->termType = __EQUAL;
        break;
    case _not_equal:
        newTerm->termType = __NOT_EQUAL;
        break;
    case _less:
        newTerm->termType = __LESS;
        break;
    case _less_equal:
        newTerm->termType = __LESS_EQUAL;
        break;
    case _greater:
        newTerm->termType = __GREATER;
        break;
    case _greater_equal:
        newTerm->termType = __GREATER_EQUAL;
        break;
    case _id:
        newTerm->termType = __ID;
        break;
    case _celoc_lit:
    case _celoc_lit_exp:
        newTerm->termType = __INT;
        break;
    case _des_lit_exp:
    case _des_lit:
        newTerm->termType = __DOUBLE;
        break;
    case _ret_lit:
        newTerm->termType = __STRING;
        break;
    default:
        newTerm->termType = __UNDEF;
        break;

    }

    // predani informace pro GC
    initGCItem(newTerm, "TERM");

    return newTerm;
}

pTerm createTerm(enumTerm term) {
    pTerm newTerm;
    if((newTerm = malloc(sizeof(struct Term))) == NULL) {
        error(ERROR_INTER, "Chyba alokace pameti pTerm\n");
    }
    newTerm->termType = term;
    newTerm->token = NULL;

    // predani informace pro GC
    initGCItem(newTerm, "TERM");

    return newTerm;
}

void handleOnTop(dListPointer exprList){
    if((popFirstTerm(exprList))->termType != __HANDLE) {
        syntaxError(_NULL);
    }
}

void createFrame() {
    // vytvoreni ramce
    generateSceletInstruction(_CREATE_BLOCK, NULL);

    // naklonovani viditelnych promnenych z predesleho ramc
    for(int i = 0; i < (int)symbolTableActual->arrSize; i++) {

        pItem tableItem = symbolTableActual->fPtr[i];

        // horizontalni pruchod
        while(tableItem != NULL) {
            if(tableItem->visibilitySignature != symbolTableActual->visibilitySignature && tableItem->data->token->id == _id) {
                if(hTabFind(symbolTableActual, tableItem->key)->visibilitySignature == tableItem->visibilitySignature) {
                    generateSceletInstruction(_COPY_IN, initTerm(tableItem->data->token));
                }
            }

            //posunuti na dalsi prvek
            tableItem = tableItem->pNext;
        }
    }
}

void destroyFrame() {

    // naklonovani viditelnych promnenych z predesleho ramc
    for(int i = 0; i < (int)symbolTableActual->arrSize; i++) {

        pItem tableItem = symbolTableActual->fPtr[i];

        // horizontalni pruchod
        while(tableItem != NULL) {

            if(tableItem->visibilitySignature != symbolTableActual->visibilitySignature && tableItem->data->token->id == _id) {
                if(hTabFind(symbolTableActual, tableItem->key)->visibilitySignature != symbolTableActual->visibilitySignature) {
                    generateSceletInstruction(_COPY_OUT, initTerm(tableItem->data->token));
                }
            }
            //posunuti na dalsi prvek
            tableItem = tableItem->pNext;
        }
    }
    // ukonceni ramce ramce
    generateSceletInstruction(_END_BLOCK, NULL);
}

struct typeOfParam *createTypeOfParam(symbolType newType, struct typeOfParam *oldParam) {
    struct typeOfParam *newParam;
    if((newParam = malloc(sizeof(struct typeOfParam))) == NULL) {
        error(ERROR_INTER, "Chyba alokace pameti customString\n");
    }

    initGCItem(newParam, "F_PARAM");

    newParam->type = newType;

    if(oldParam != NULL) {
        newParam->next = oldParam;
    }

    return newParam;
}

struct typeOfParam *removeFirstParam(struct typeOfParam *oldParam) {
    if(oldParam == NULL) {
        error(ERROR_INTER, "Nastala chyba pri kontrole parametru funcke");
    }

    struct typeOfParam *newFirst = oldParam->next;

    GCRemove(oldParam);

    return newFirst;
}

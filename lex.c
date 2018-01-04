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
 *  Soubor: lex.c
 *  Autori: Miroslav Karpisek
 *          Roman Zajic
 *          Jakub Samek
 *
 *	Datum:  20-10-2017
 *  Uprava: 06-12-2017
 */
 
#include "lex.h"

static bool useMem = false; //flag pro indikaci zda-li se ma cist posledni char jiz nacteny, nebo ze vstupu
static int actual;          //nacteny char - zustava v pameti v pripade restartovani KDA
bool nextLine = false;      //flag pro indikaci konce radku
cToken prefabCtoken[62];    //pole predvytvorenych ukazatelu na primitivni tokeny
char* readHead;             //aktualni cteci halva ze vstupu

void createTokenSet() {
    for(int i = 0; i < 62; i++) {
        cToken newToken = createToken();
        newToken->id = i;
        prefabCtoken[i] = newToken;
    }
}

void initLex() {
    createTokenSet();
    createString(&lineBuffer);
    loadLine();
}

void loadLine() {
    clearStringStr(&lineBuffer);

    char nextChar;
    do {
        nextChar = fgetc(stdin);
        pushChar(&lineBuffer, nextChar);
    }
    while(nextChar != EOF && nextChar != '\n');

    readHead = lineBuffer->str;

}

cToken createToken() {
    struct customToken *newToken;

    if((newToken = malloc(sizeof(struct customToken))) == NULL) {
        error(ERROR_INTER, "Chyba alokace pameti customToken\n");
    }
    else {
        // vlozeni zaznamu do GC
        initGCItem(newToken, "TOKEN");
    }

    newToken->id = _NULL;
    newToken->cPointer = NULL;



    return newToken;

}


cToken getToken() {
    // inicializace automatu na pocatecni stavu a typu tokenu
    AutomatState state = S_Start;
    enumID tokenType = _NULL;

    int tokLine, tokColumn;

    //inicializace stringu pro zaznam jednotlivich charu na vstupu
    cString tokenValue = NULL;
    createString(&tokenValue);
    // konecni automat pro rozpoznani vstupu
    do {
        tokLine = line;
        tokColumn = column;

        // je posledni nacteny znak ulozen v pameti
        if(!useMem) {

            if(nextLine) {
                loadLine();
            }
            if(*readHead == '\0') {
                error(ERROR_LEX, "Neocekavany konec souboru");
            }

            actual = *readHead;
            readHead++;

            addColumn();

            if(nextLine) {
                addLine();
                nextLine = false;
            }

        }
        else {
            if(nextLine) {
                addLine();
                nextLine = false;
            }
            useMem = false;
        }

        switch (state) {

            case S_Base2:
                if(actual == '0' || actual == '1') {
                    pushChar(&tokenValue, actual);
                }
                else {
                    state = S_Start;
                    useMem = true;

                    tokenType = _celoc_lit_bin;
                }
                break;

            case S_Base8:
                if(actual >= '0' && actual <= '7') {
                    pushChar(&tokenValue, actual);
                }
                else {
                    state = S_Start;
                    useMem = true;

                    tokenType = _celoc_lit_oct;
                }
                break;

            case S_Base16:
                if((actual >= '0' && actual <= '9') || (actual >= 'a' && actual <= 'f') || (actual >= 'A' && actual <= 'F')) {
                    pushChar(&tokenValue, actual);
                }
                else {
                    state = S_Start;
                    useMem = true;

                    tokenType = _celoc_lit_hex;
                }
                break;

            case S_Base:
                if(actual == 'b' || actual == 'B') {
                    state = S_Base2;
                    pushChar(&tokenValue, actual);
                }
                else if(actual == 'o' || actual == 'O') {
                    state = S_Base8;
                    pushChar(&tokenValue, actual);
                }
                else if(actual == 'h' || actual == 'H') {
                    state = S_Base16;
                    pushChar(&tokenValue, actual);
                }
                else {
                    destroyString(&tokenValue);
                    errorMsg(ERROR_LEX, column, "Neocekavany znak na: ASCII[%d]", actual);
                }
                break;

            case S_Minus:
                if(actual == '=') {
                    state = S_Start;

                    tokenType = _minus_equal;
                }
                else {
                    state = S_Start;

                    tokenType = _minus;
                    useMem = true;
                }
                break;

            case S_Plus:
                if(actual == '=') {

                    state = S_Start;
                    tokenType = _plus_equal;
                }
                else {
                    state = S_Start;

                    tokenType = _plus;
                    useMem = true;
                }
                break;

            case S_Star:

                if(actual == '=') {
                    state = S_Start;

                    tokenType = _mul_equal;
                }
                else {
                    state = S_Start;

                    tokenType = _multiply;
                    useMem = true;
                }
                break;

            case S_Mod:
                if(actual == '=') {
                    state = S_Start;

                    tokenType = _mod_equal;
                }
                else {
                    state = S_Start;

                    tokenType = _back_slash;
                    useMem = true;
                }
                break;

            case S_Start:
                // dlouhe
                if(actual == '_' || isalpha(actual)) {
                    state = S_Id;
                    pushChar(&tokenValue, actual);
                }
                else if(isdigit(actual)) {
                    state = S_Int;
                    pushChar(&tokenValue, actual);
                }
                //base
                else if(actual == '&') {
                    state = S_Base;
                    pushChar(&tokenValue, actual);
                }
                else if(actual == '\n') {
                    state = S_Start; //zastaveni cteni znaku

                    tokenType = _EOL;
                    nextLine = true;
                }

                // operatory
                else if(actual == '<') {
                    state = S_Less;
                }
                else if(actual == '>') {
                    state = S_Greater;
                }
                else if(actual == '=') {
                    state = S_Start;

                    tokenType = _equal;

                }
                else if(actual == '*') {
                    state = S_Star;
                }
                else if(actual == '-') {
                    state = S_Minus;
                }
                else if(actual == '+') {
                    state = S_Plus;
                }
                else if(actual == ')') {
                    state = S_Start;

                    tokenType = _right_bracket;
                }
                else if(actual == '(') {
                    state = S_Start;

                    tokenType = _left_bracket;
                }
                else if(actual == ',') {
                    state = S_Start;

                    tokenType = _colon;
                }
                else if(actual == '\\') {
                    state = S_Mod;
                }

                //EOF
                else if(actual == EOF) {
                    state = S_Start;

                    tokenType = _EOF;

                }

                else if(actual == '!') {
                    state = S_Mark;
                }
                else if(actual == '\'') {
                    state = S_LineCom;
                    //muze jit pouze o komentar, nebo error -> netreba ukladat znaky
                }
                else if(actual == '/') {
                    state = S_Slash;
                    pushChar(&tokenValue, actual);
                }

                else if(actual == ';') {
                    state = S_Start;

                    tokenType = _semicolon;
                }
                else if(isspace(actual)) {
                    state = S_Start;
                }
                else {
                    destroyString(&tokenValue);
                    errorMsg(ERROR_LEX, column, "Neocekavany znak na: ASCII[%d]", actual);
                }


                break;

            //cisla
            case S_Int:
                if(isdigit(actual)) {
                    pushChar(&tokenValue, actual);
                }
                else if(actual == '.') {
                    state = S_IntDot;
                    pushChar(&tokenValue, actual);
                }
                else if(actual == 'E' || actual == 'e') {
                    state = S_DoubleE;
                    pushChar(&tokenValue, actual);
                }
                else {
                    state = S_Start;
                    useMem = true;

                    tokenType = _celoc_lit;
                }
                break;

            case S_IntDot:
                if(isdigit(actual)) {
                    state = S_Double;
                     pushChar(&tokenValue, actual);
                }
                else {
                    destroyString(&tokenValue);
                    errorMsg(ERROR_LEX, column, "Neocekavany znak na: ASCII[%d]", actual);
                }


                break;

            case S_Double:
                if(isdigit(actual)) {
                     pushChar(&tokenValue, actual);
                }
                else if(actual == 'E' || actual == 'e' ) {
                    state = S_DoubleE;
                     pushChar(&tokenValue, actual);
                }
                else {
                    state = S_Start;
                    useMem = true;

                    tokenType = _des_lit;
                }
                break;

            case S_DoubleE:
                if(isdigit(actual)) {
                    state = S_DoubleExp;
                     pushChar(&tokenValue, '+');
                     pushChar(&tokenValue, actual);
                }
                else if(actual == '+' || actual == '-') {
                    state = S_DoubleEOp;
                    pushChar(&tokenValue, actual);
                }
                else {
                    destroyString(&tokenValue);
                    errorMsg(ERROR_LEX, column, "Neocekavany znak na: ASCII[%d]", actual);
                }
                break;

            case S_DoubleEOp:
                if(isdigit(actual)) {
                    state = S_DoubleExp;
                    pushChar(&tokenValue, actual);
                }
                else {
                    destroyString(&tokenValue);
                    return NULL;
                }
                break;

            case S_DoubleExp:
                if(isdigit(actual)) {
                     pushChar(&tokenValue, actual);
                }
                else {
                    state = S_Start;
                    useMem = true;

                    tokenType = _des_lit_exp;
                }
                break;

            //komentare
            case S_LineCom:
                if(actual == '\n') {
                    state = S_Start;
                    //zadny token, jde o komentar, ale je ukoncen radkovy komentar
                    useMem = true;
                }
                else if(actual == EOF) {
                    state = S_Start;
                    tokenType = _EOF;
                }

                //pokud se jedna o jiny znak, zustavam ve stavu S_LineCom
                break;

            case S_Slash:
                if(actual == '\'') {
                    state = S_BlockCom;
                    //netreba ukladat znak, jedna se o komentar
                }
                else if(actual == '=') {
                    state = S_Start;

                    tokenType = _div_equal;
                }
                else {
                    state = S_Start;
                    useMem = true;

                    tokenType = _forward_slash;
                }
                break;

            case S_BlockCom:
                if(actual == '\'') {
                    state = S_EndBlockCom;
                }
                else if(actual == EOF) {
                    destroyString(&tokenValue);
                    errorMsg(ERROR_LEX, column, "Neocekavany znak na: ASCII[%d]", actual);
                }

                // jinak jsme stale v komentari netreba nic delat pouze nacist dalsi znak
                break;

            case S_EndBlockCom:
                if(actual == '/') {
                    state = S_Start;
                    clearStringStr(&tokenValue);
                    //netreba ukladat znak, jedna se o komentar
                }
                else if(actual == EOF) {
                    destroyString(&tokenValue);
                    errorMsg(ERROR_LEX, column, "Neocekavany znak na: ASCII[%d]", actual);
                }
                else if(actual != '\''){
                    state = S_BlockCom;
                }
                break;

            //string
            case S_Mark:
                if(actual == '\"') {
                    state = S_String;
                }
                else {
                    destroyString(&tokenValue);
                    errorMsg(ERROR_LEX, column, "Neocekavany znak na: ASCII[%d]", actual);
                }
                break;

            case S_String:
                if(actual == '\\') {
                    state = S_Escape;
                }
                if(actual == '\"') {
                    state = S_Start;
                    tokenType = _ret_lit;
                }
                else if(actual > 31){
                     pushChar(&tokenValue, actual);
                }
                else {
                    destroyString(&tokenValue);
                    errorMsg(ERROR_LEX, column, "Neocekavany znak na: ASCII[%d]", actual);
                }
                break;


            case S_Escape:
                if(actual > 31) {
                    state = S_String;
                    pushChar(&tokenValue, actual);
                }
                else {
                    destroyString(&tokenValue);
                    errorMsg(ERROR_LEX, column, "Neocekavany znak na: ASCII[%d]", actual);
                }
                break;

            // id
            case S_Id:
                if(actual == '_' || isalpha(actual) || isdigit(actual)) {
                     pushChar(&tokenValue, actual);
                }
                else {
                    state = S_Start;
                    useMem = true;

                    tokenType = _id;
                }
                break;

            // operatory
            case S_Less:
                if(actual == '=') {
                    state = S_Start;

                    tokenType = _less_equal;
                }
                else if(actual == '>') {
                    state = S_Start;

                    tokenType = _not_equal;
                }
                else {
                    state = S_Start;
                    useMem = true;

                    tokenType = _less;
                }
                break;

            case S_Greater:
                if(actual == '=') {
                    state = S_Start;

                    tokenType = _greater_equal;
                }
                else {
                    state = S_Start;
                    useMem = true;

                    tokenType = _greater;
                }
                break;

            default:
                errorMsg(ERROR_LEX, column, "Neocekavany znak na: ASCII[%d]", actual);
                break;

        }

    } while (tokenType == _NULL);

    cToken newToken;
    if(tokenType == _id) {

        newToken = createToken();
        newToken->id = tokenType;

        // volani normalizace cStringu
        normalizeStr(tokenValue, &tokenType);

        for(int i = _As; i<= _True ; i++) {
            if(stringKeywordCmp(tokenValue, keywords[i])) {
                newToken->id = i;
                GCRemove(newToken->cPointer);

                newToken->cPointer = NULL;
                break;
            }
        }

        newToken->cPointer = tokenValue->str;

    }
    else if((tokenType < _id || tokenType > _ret_lit) && (tokenType < _plus_equal || tokenType > _mod_equal) && (tokenType < _celoc_lit_bin || tokenType > _celoc_lit_oct) ){
        GCRemove(tokenValue->str);
        newToken = prefabCtoken[tokenType];
    }

    else {
        newToken = createToken();

        //normalizace
        normalizeStr(tokenValue, &tokenType);

        newToken->id = tokenType;
        newToken->cPointer = tokenValue->str;
    }

    GCRemove(tokenValue);
    newToken->column = tokColumn;
    newToken->line = tokLine;
    //DEBUG_PRINT_LEX("\n\tTyp Tokenu: %d [%s]\n\tHodnota: %s\n\t %d", newToken->id, keywords[newToken->id], newToken->cPointer, newToken->column);
    return newToken;
}

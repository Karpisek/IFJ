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
 *  Soubor: instruction_generator.h
 *  Autori: Jakub Trubka
 *          Miroslav Karpisek
 *
 *
 *	Datum:  08-11-2017
 *  Uprava: 05-12-2017
 */

#ifndef PROJEKT_INSTRUCTIONGENERATOR_H
#define PROJEKT_INSTRUCTIONGENERATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "parser.h"
#include "dlist.h"
#include "enums.h"

//typy prikazu
typedef enum {
    _ADD = 0,       //0 scitani DONE
    _MUL,           //1 nasobeni DONE
    _DIV,           //2 deleni DONE
    _SUB,           //3 odcitani DONE
    _MOD,           //4 celociselne deleni
    _LESS,          //5 mensi
    _EQUAL,         //6 rovno
    _GREATER,       //7 vetsi
    _NOTEQUAL,      //8 nerovno
    _LESSEQUAL,     //9 mensi-rovno
    _GREATEREQUAL,  //10 vetsi-rovno
    _FUNC,          //11 volani funkce
    _EXP,           //12 translate to expr
    _UNARY_MINUS,

} expresionEnum;

typedef enum {
    _CREATE_FUNC = 0,   // 1-arg labelname + vytvor si lokalni promnenou return podle navratoveho typu arg1->
    _END_FUNCTION,      // bez parametru hodis si lokalni promnenou RETURN na zasobnik a ukoncis funkci
    _DECLARE_VAR,       // 1-arg varname -> podivat se do tabulky kam uz to mira musel dat jinak je kokot, podle toho movnout implicitni hodnotu
    _MOVE,              // ze zasobniku na 1-arg (var) (definice)
    _READ,              // 1-arg -> kam
    _PRINT,             // vrchol zasobniku vytisknout
    _IF,                // test vrcholu zasobniku + vytvor si nobej label (a jmeno)
    _ELSEIF,            // test vrcholu zasobniku (vytvore si jmeno label podle labelu ifu)
    _ELSE,              // bez parametru (smaz si ze stacu jmen aktivnich if lablu posledni zaznam)
    _ENDIF,             // bezparametru
    _CREATE_WHILE,      // bezparametru, pak expr
    _WHILE,             // test vrcholu zasobniku
    _LOOP,              // bez param, skoc zpet
    _RETURN,            // bez parametru hodis si hodnotu ze zasobniku do promene RETURN
    _CREATE_BLOCK,      // bez parametru PUSHFRAME && CREATEFRAME
    _COPY_IN,           // MOVE TF@x LF@x (promnena x v 1-arg)
    _COPY_OUT,          // MOVE LF@x TF@x (promnena x v 1-arg)
    _END_BLOCK,         // POPFRAME
    _INIT_VAR,          // inicializace promnene

} sceletEnum;

typedef enum {
    MOVE = 0,       //0
    CREATEFRAME,    //1
    PUSHFRAME,      //2
    POPFRAME,       //3
    DEFVAR,         //4
    CALL,           //5
    RETURN,         //6
    PUSHS,          //7
    POPS,           //8
    CLEARS,         //9
    ADD,            //10
    SUB,            //11
    MUL,            //12
    DIV,            //13
    ADDS,           //14
    SUBS,           //15
    MULS,           //16
    DIVS,           //17
    LT,             //18
    GT,             //19
    EQ,             //20
    LTS,            //21
    GTS,            //22
    EQS,            //23
    AND,            //24
    OR,             //25
    NOT,            //26
    ANDS,           //27
    ORS,            //28
    NOTS,           //29
    INT2FLOAT,      //30
    FLOAT2INT,      //31
    FLOAT2REINT,    //32
    FLOAT2R2OINT,   //33
    INT2CHAR,       //34
    STRI2INT,       //35
    INT2FLOATS,     //36
    FLOAT2INTS,     //37
    FLOAT2R2EINTS,   //38
    FLOAT2R2OINTS,  //39
    INT2CHARS,      //40
    STRI2INTS,      //41
    READ,           //42
    WRITE,          //43
    CONCAT,         //44
    STRLEN,         //45
    GETCHAR,        //46
    SETCHAR,        //47
    TYPE,           //48
    LABEL,          //49
    JUMP,           //50
    JUMPIFEQ,       //51
    JUMPIFNEQ,      //52
    JUMPIFEQS,      //53
    JUMPIFNEQS,     //54
} instructionEnum;

//struktura instrukce
typedef struct instructionItem{
    instructionEnum id;
    char *argument[4];
    struct instructionItem *left;
    struct instructionItem *right;
} *pInsItem;

//list instrukci
typedef struct instructionList{
    pInsItem first;
    pInsItem last;
    pInsItem actual;
} *pInsList;

/**
 * funkce generujici strukturu aritmeticke instrukce
 * @param  expresionEnum         id operace ke zpracovani
 * @param  pTerm                 prvni hodnota ke zpracovani
 * @param  pTerm                 druha hodnota ke zpracovani
 * @return                       vraci true pri uspechu
 */
bool generateArithmeticInstruction(expresionEnum, pTerm, pTerm);

/**
 * funkce alokuje a inicializuje listy instrukci
 */
void insListInit();

/**
 * funkce generujici strukturu skelet instrukce
 * @param  expresionEnum         id operace ke zpracovani
 * @param  pTerm                 prvni hodnota ke zpracovani
 * @return                       vraci true pri uspechu
 */
bool generateSceletInstruction(sceletEnum, pTerm);

/**
 * funkce generujici pretypovani hdnoty na zasobniku, pozice i
 * @param  int                   index pozice, od 0
 * @return                       vraci true pri uspechu
 */
bool generateChangeTypeInstruction( int i);

/**
 * funkce prida na zacatek seznamu instrukci dany prikaz
 * @param  pInsList             ukazatel na list instrukci
 * @param  pInsItem             ukazatel na novy item
 * @param  char *               misto pro 4 argumenty
 */
void insAdd(pInsList, instructionEnum, char *, char *, char *, char *);

/**
 * funkce provede vypis instrukcniho listu
 * @param  pInsList             ukazatel na list instrukci
 * @param file                  misto vypisu
 */
void insListPrint(pInsList, FILE *);

/**
 * funkce provede vypis vsech instrukci
 * @param file                  misto vypisu
 */
void insPrint( FILE *);

/**
 * funkce uvolni seznam z pameti
 * @param  pInsList             ukazatel na list instrukci
 */
void insListFree(pInsList);

/**
 * funkce vygeneruje instrukci UNARY MINUS vcetne pripadneho pretypovani
 * @param  pInsList             ukazatel na list instrukci
 */
void generateUnaryMinus(pInsList);

/**
 * funkce vygeneruje instrukci ADDS vcetne pripadneho pretypovani/konkatenace
 * @param  @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pInsList             ukazatel na list instrukci
 */
void generateAdd( pTerm, pTerm, pInsList);

/**
 * funkce vygeneruje instrukci MULS vcetne pripadneho pretypovani
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pInsList             ukazatel na list instrukci
 */
void generateMul( pTerm, pTerm, pInsList);

/**
 * provede typovou kontrolu argumentu pripadne nastavi priznak pro generovani pretypovaci fce
 * na true a provede jeji zavolani
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pInsList             ukazatel na list instrukci
 */
void insTypeControl( pTerm, pTerm, pInsList);

/**
 * funkce vygeneruje instrukci SUBS vcetne pripadneho pretypovani a prohozeni stacku
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pInsList             ukazatel na list instrukci
 */
void generateSub( pTerm, pTerm, pInsList);

/**
 * funkce vygeneruje instrukci DIVS vcetne pripadneho pretypovani a prohozeni stacku
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pInsList             ukazatel na list instrukci
 */
void generateDiv( pTerm, pTerm, pInsList);

/**
 * funkce vygeneruje instrukci PUSHS
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pInsList             ukazatel na list instrukci
 */
void generatePush(pTerm , pInsList);

/**
 * funkce vygeneruje instrukci pro celociselne deleni
 * @param  pInsList             ukazatel na list instrukci
 */
void generateModDiv(pInsList instructionList);

/**
 * funkce vygeneruje instrukci pro porovnani cisel <
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pInsList             ukazatel na list instrukci
 */
void generateLess( pTerm, pTerm, pInsList);

/**
 * funkce vygeneruje instrukci pro porovnani cisel =
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pInsList             ukazatel na list instrukci
 */
void generateEqual( pTerm, pTerm, pInsList);

/**
 * funkce vygeneruje instrukci pro porovnani cisel !=
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pInsList             ukazatel na list instrukci
 */
void generateNotEqual( pTerm, pTerm, pInsList);

/**
 * funkce vygeneruje instrukci pro porovnani cisel <=
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pInsList             ukazatel na list instrukci
 */
void generateLessEqual(pTerm, pTerm, pInsList);

/**
 * funkce vygeneruje instrukci pro porovnani cisel >
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pInsList             ukazatel na list instrukci
 */
void generateGreater(pTerm, pTerm, pInsList);

/**
 * funkce vygeneruje instrukci pro cteni z konzole
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pInsList             ukazatel na list instrukci
 */
void generateRead(pTerm, pInsList);

/**
 * funkce vygeneruje instrukci pro porovnani cisel >=
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pInsList             ukazatel na list instrukci
 */
void generateGreaterEqual(pTerm, pTerm, pInsList);

/**
 * funkce vygeneruje instrukci pro volani funkce
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pInsList             ukazatel na list instrukci
 */
void generateFunctionCall(pTerm, pInsList);

/**
 * funkce vygeneruje instrukci pro tisk
 * @param  pInsList             ukazatel na list instrukci
 */
void generatePrint(pInsList);

/**
 * funkce vygeneruje instrukci pro definici promene
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pInsList             ukazatel na list instrukci
 */
void generateDefvar(pTerm, pInsList);

/**
 * funkce vygeneruje instrukci pro presun hodnoty do promene
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 * @param  pInsList             ukazatel na list instrukci
 */
void generateMove(pTerm, pInsList);

/**
 * funkce vygeneruje navesti funkce s novym ramcem a pomocnou navratovou promennou, ulozi do ni hodnotu dle navratoveho typu, istrukcni list se prepne na functionList
 * @param  pTerm                ukazatel na strukturu se jemenem funkce pouziteho jako navesti
 * @param  pInsList             ukazatel na list instrukci
 */
void generateCreateFunc(pTerm, pInsList);

/**
 * funkce vygeneruje instrukci pro navrat z funkce (pushne vysledek na zasobnik) a prepne instrukcni list na bodyList
 * @param  pInsList             ukazatel na list instrukci
 */
void generateEndFunc(pInsList);

/**
 * funkce popne vyskedek ze zasobniku do pomocne navratove instrukce
 * @param  pInsList             ukazatel na list instrukci
 */
void generateReturn(pInsList);

/**
 * funkce vytvori novy ramec
 * @param  pInsList             ukazatel na list instrukci
 */
void generateCreateBlock(pInsList);

/**
 * funkce ukonci ramec
 * @param  pInsList             ukazatel na list instrukci
 */
void generateEndBlock(pInsList instructionList);

/**
 * funkce zkopiruje promennou z LF do TF
 * @param  pTerm                ukazatel na strukturu se jemenem funkce pouziteho jako navesti
 * @param  pInsList             ukazatel na list instrukci
 */
void generateCopyOut(pTerm, pInsList);

/**
 * funkce zkopiruje promennou z TF do LF
 * @param  pTerm                ukazatel na strukturu se jemenem funkce pouziteho jako navesti
 * @param  pInsList             ukazatel na list instrukci
 */
void generateCopyIn(pTerm, pInsList);

/**
 * funkce prepoji instrokci ze zacatku listu 1 na zacatek listu 2
 * @param  pInsList             ukazatel na list z
 * @param  pInsList             ukazatel na list do
 */
void insConnect(pInsList , pInsList);

/**
 * funkce prohledavajici "zasobnik" tabulek symbolu
 * @param  pTerm                ukazatel na strukturu s promenymi/typem
 */
char *frameControl(pTerm);

/**
 * funkce generujici kontroli podminky, navesti a pusnhnuti potrebnych instrukci do listu IF
 * @param  pInsList             ukazatel na listinstrukci
 */
void generateIf(pInsList);

/**
 * funkce prepoji dve instrukce z listu IF na aktualni list
 * @param  pInsList             ukazatel na listinstrukci
 */
void generateElse(pInsList);
/**
 * funkce prepoji instrukci pro ukonceni ifu z IF seznamu na aktualni seznam
 * @param  pInsList             ukazatel na listinstrukci
 */
void generateEndIf(pInsList);

/**
 * funkce vygeneruje vsechny instrukce potrebne pro smycku while, do aktualniho listu pripoji prvni a ostatni pripoji do WHILE listu
 * @param  pInsList             ukazatel na listinstrukci
 */
void generateCreateWhile(pInsList);

/**
 * prepojeni instrukci pro kontrolu podmiky z listu while do aktualniho listu
 * @param  pInsList             ukazatel na listinstrukci
 */
void generateWhile(pInsList);

/**
 * prepojeni instrukci pro skok na vypocet podminky z WHILE listu do aktualniho listu
 * @param  pInsList             ukazatel na list instrukci
 */
void generateLoop(pInsList);

/**
 * funkce pro uvolneni vsech listu
 */
void insFree();

/**
 * funkce vygeneruje inicializaci promenne
 * @param  pTerm                ukazatel na argument
 * @param  pInsList             ukazatel na list instrukci
 */
void generateInit(pTerm, pInsList);

/**
 * funkce vygeneruje index navesti pro if nebo while
 * @param  bool                 prepinac mezi if a while
 * @param  pInsList             ukazatel na list instrukci
 * @return char *               vraci retezecovou verzi indexu
 */
char *labelGenerator(int);

/**
 * funkce vygeneruje kostru pro elseif
 * @param  pInsList             ukazatel na list instrukci
 */
void generateElseIf(pInsList instructionList);


#endif //PROJEKT_INSTRUCTIONGENERATOR_H

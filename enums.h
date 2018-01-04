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
 *  Soubor: enums.h
 *  Autori: Miroslav Karpisek
 *          Jakub Trubka
 *
 *	Datum:  22-10-2017
 *  Uprava: 05-12-2017
 */

#ifndef H_ENUMS
#define H_ENUMS

#include <stdlib.h>
#include <stdio.h>

// makra pro eliminaci konstant (vynasobni enumu pro jednotlive <expr> typy)
#define STRINGS         typesAnalyzer == 400
#define INTS            typesAnalyzer == 484
#define DOUBLES         typesAnalyzer == 441
#define NOT_STRING      typesAnalyzer == 462
#define NOT_DOUBLE      typesAnalyzer == 440
#define NOT_INT         typesAnalyzer == 420

//#############################################
//              POLE TISK ENUMU
//#############################################

char *keywords[67];

char *helperTable[25];

char *varType[5];

char *operationType[13];

//#############################################
//              ENUMS
//#############################################
// enum pro tokeny
typedef enum {

    // NULL
    _NULL = -1,

    //pro precedencni tabulku potrebne v tomto poradi
    _left_bracket = 0,  //0, leva zavorka
    _right_bracket,     //1, prava zavorka
    _func,              //2, token pro precedencni analyzu (nelze ziskat z lexikalniho analyzatoru)
    _colon,             //3, carka
    _dollar,            //4, token pro precedencni analyzu (nelze ziskat z lexikalniho analyzatoru)
    _multiply,          //5, *
    _forward_slash,     //6, lomitko
    _back_slash,        //7, zpetne lomitko
    _plus,              //8, plus
    _minus,             //9, minus
    _equal,             //10, rovna se
    _not_equal,         //11, <>
    _less,              //12, <
    _less_equal,        //13, <=
    _greater,           //14, >
    _greater_equal,     //15, >=
    _id,                //16, identifikator
    _celoc_lit,         //17, celociselny literal
    _celoc_lit_exp,     //18, celociselny literal s exponentem
    _des_lit,           //19, desetinny literal
    _des_lit_exp,       //20, desetinny literal s exponentem
    _ret_lit,           //21, retezcovy literal

    //klicova slova
    _As,                //22
    _Asc,               //23
    _Declare,           //24
    _Dim,               //25
    _Do,                //26
    _Double,            //27
    _Else,              //28
    _End,               //29
    _Chr,               //30
    _Function,          //31
    _If,                //32
    _Input,             //33
    _Integer,           //34
    _Length,            //35
    _Loop,              //36
    _Print,             //37
    _Return,            //38
    _Scope,             //39
    _String,            //40
    _SubStr,            //41
    _Then,              //42
    _While,             //43

    //rezervovana klicova slova
    _And,               //44
    _Boolean,           //45
    _Continue,          //46
    _Elseif,            //47
    _ExitToken,         //48
    _False,             //49
    _For,               //50
    _Next,              //51
    _Not,               //52
    _Or,                //53
    _Shared,            //54
    _Static,            //55
    _True,              //56

    _semicolon,         //57, semicolon

    //operatory
    _EOL,               //58, new line
    _EOF,               //59, EOF

    //pro precedencni
    //_handle_start,    //60
    _EXPR,              //61

    // rozsireni unarni operatory
    _plus_equal,        //62
    _minus_equal,       //63
    _mul_equal,         //64
    _div_equal,         //65
    _mod_equal,         //66
    _celoc_lit_bin,     //67
    _celoc_lit_hex,     //68
    _celoc_lit_oct,     //69

} enumID;

//enum pro jednotlive stavy automatu
typedef enum {
    S_Start,
    S_Mark,
    S_String,
    S_BlockCom,
    S_LineCom,
    S_Double,
    S_DoubleE,
    S_DoubleEOp,
    S_DoubleExp,
    S_EndBlockCom,
    S_Less,
    S_Greater,
    S_Int,
    S_IntDot,
    S_Slash,
    S_Id,
    S_Escape,

    S_Minus,
    S_Plus,
    S_Star,
    S_Mod,
    S_Div,

    S_Base,
    S_Base2,
    S_Base8,
    S_Base16,
} AutomatState;

//enum pro lepsi citelnost precedencni tabulky
typedef enum {
    LESS = 0,
    EQUA = 1,
    GREA = 2,
    NONE = 3,
    FINE = 4
} precedenc;

typedef enum {
    __LEFT_BRACKET = 0,   //0, leva zavorka
    __RIGHT_BRACKET,      //1, prava zavorka
    __FUNC,               //2, token pro precedencni analyzu (nelze ziskat z lexikalniho analyzatoru)
    __COLON,              //3, carka
    __DOLLAR,             //4, token pro precedencni analyzu (nelze ziskat z lexikalniho analyzatoru)
    __MULTIPLY,           //5, *
    __FORWARD_SLASH,      //6, lomitko
    __BACK_SLASH,         //7, zpetne lomitko
    __PLUS,               //8, plus
    __MINUS,              //9, minus
    __EQUAL,              //10, rovna se
    __NOT_EQUAL,          //11, <>
    __LESS,               //12, <
    __LESS_EQUAL,         //13, <=
    __GREATER,            //14, >
    __GREATER_EQUAL,      //15, >=
    __ID,                 //16, identifikator
    __INT,                //17, celociselny literal s exponentem
    __DOUBLE,             //18, desetinny literal s exponentem
    __STRING,             //19, retezcovy literal

    __EXPR_STRING,        //20, ocekavam <expr> string
    __EXPR_DOUBLE,        //21, ocekavam <expr> double
    __EXPR_INT,           //22, ocekavam <expr> int

    __HANDLE,             //23, _handle_start
    __UNDEF,              //23
    __EXPR_BOOL,          //24

} enumTerm;

typedef enum symbolEnum{
    UNDEFINED = 0,
    STRING, //1
    DOUBLE, //2
    INT,    //3
    BOOL,   //4
    FUNC,   //5
} symbolType;

typedef enum {
    TOKEN,
    TERM
} dataTypeEnum;

//Term
typedef struct Term{
    enumTerm termType;
    struct customToken *token;
} *pTerm;

//customString
typedef struct customString{
    int actSize; //delka retezce ukonceneho \0
    int maxSize;  //kolik pameti je alokovano
    char* str; //retezec
} *cString;

//GC
typedef struct customToken{
    enumID id;
    char *cPointer;
    int line, column; //misto ziskani
} *cToken;

#endif

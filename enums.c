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
 *  Soubor: enums.c
 *  Autori: Miroslav Karpisek
 *          Jakub Trubka
 *
 *	Datum:  22-10-2017
 *  Uprava: 05-12-2017
 */

#include "enums.h"

 // pro tisk enumID cTokenu
 char *keywords[67] = {
     "left_bracket",     //leva zavorka
     "right_bracket",    //prava zavorka
     "func",             //token pro precedencni analyzu (nelze ziskat z lexikalniho analyzatoru)
     ",",                //carka
     "$",                //token pro precedencni analyzu (nelze ziskat z lexikalniho analyzatoru)
     "*",                // *
     "/",                //lomitko
     "\\",               //zpetne lomitko
     "+",                //plus
     "-",                //minus
     "=",                //rovna se
     "<>",               // <>
     "<",                // <
     "<=",               // <=
     ">",                // >
     ">=",               // >=
     "id",               //16, identifikator
     "celoc_lit",        //17, celociselny literal
     "celoc_lit_exp",    //18, celociselny literal s exponentem
     "des_lit",          //19, desetinny literal
     "des_lit_exp",      //20, desetinny literal s exponentem
     "ret_lit",          //21, retezcovy literal
     "as",
     "asc",
     "declare",
     "dim",
     "do",
     "double",
     "else",
     "end",
     "chr",
     "function",
     "if",
     "input",
     "integer",
     "length",
     "loop",
     "print",
     "return",
     "scope",
     "string",
     "substr",
     "then",
     "while",
     "and",
     "boolean",
     "continue",
     "elseif",
     "exit",
     "false",
     "for",
     "next",
     "not",
     "or",
     "shared",
     "static",
     "true",
     ";",                //semicolon
     "EOL",              //new line
     "EOF",              //EOF
     "handle_start",
     "EXPR",
     "plus_equal",
     "minus_equal",
     "mul_equal",
     "div_equal",
     "mod_equal",

 };

 // pro tisk precedenc
 char* helperTable[25] = {
     "(",
     ")",
     "<func>",
     ",",
     "$",
     "*",
     "/",
     "\\",
     "+",
     "-",
     "=",
     "<>",
     "<",
     "<=",
     ">",
     ">=",
     "<id>",
     "<int>",
     "<double>",
     "<string>",
     "<expr_string>",
     "<expr_double",
     "<expr_int>",
     "<HANDLE>",
     "__undef__"
 };

 // tisk pro symbolType
 char* varType[5] = {
     "Undef",
     "String",
     "Double",
     "Integer",
     "Boolean",
 };

//pro tisk operatoru
 char* operationType[13] = {
     "scitani/konkatenace",
     "nasobeni",
     "deleni",
     "odcitani",
     "celociselne deleni",
     "porovnani",
     "porovnani",
     "porovnani",
     "porovnani",
     "porovnani",
     "porovnani",
     "porovnani",
     "porovnani",
 };

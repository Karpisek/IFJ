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
 *  Soubor: parser.h
 *  Autori: Miroslav Karpisek
 *          Jakub Trubka
 *
 *	Datum:  20-10-2017
 *  Uprava: 05-12-2017
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lex.h"
#include "enums.h"
#include "dlist.h"
#include "symtable.h"
#include "instruction_generator.h"
#include "garbage_collector.h"

int precedencTable[20][20]; //precedencni tabulka pomoci 3D pole.

struct typeOfParam {
    symbolType type;
    struct typeOfParam *next;
};

struct typeOfParam *createTypeOfParam(symbolType, struct typeOfParam *);

struct typeOfParam *removeFirstParam(struct typeOfParam *);


pHTable symbolTableActual;

/**
 * inicalizuje term dle zadaneho tokenu
 * @param   cToken                      zadany token jez se ma obalit termem
 * @return  pTerm                       alokovany term
 */
pTerm initTerm(cToken);


/**
 * vytvori novy term dle zadaneho term typu
 * @param   enumTerm                zadany termtype
 * @return  pTerm                   alokovany term
 */
pTerm createTerm(enumTerm);

/**
 * funkce pro vytvoreni rutin pri vytvoreni inforamce
 * vcetne zkopirovani promnenych zdedenych nadramcem
 */
void createFrame();

/**
 * kontrola ze na vrcholu zadanecho listu se nachazi zacatek handle
 * @param   dListPointer            prohledavany dlist
 */
void handleOnTop(dListPointer);

/**
 * funkce volana v pripade nalezeni chyby (neocekavany token)
 * @param  enumID                   ocekavany token _NULL pokud nelze urcit
 * @return                          navratova hodnota bool pro utiseni warningu
 */
bool syntaxError(enumID);

/**
 * funkce volana z lexikalniho analyzatoru pri indikaci \n na vstupu pouze
 * pridani counteru radku a reset counteru sloupcu
 */
void addLine();

/**
 * funkce volana z lexikalniho analyzatoru pri nacteni noveho znaku ze souboru
 */
void addColumn();

/**
 * funkce volana z main, hlavni telo programu
 * prvne preda lexikalnimu analyzatoru inforamce o zokoumanem souboru
 * a zkouma jednotlive tokeny predane pomoci funkce nextToken() na ktera aplikuje
 * jednotliva pravidla, syntakticka analyza je simulovana pomoci rekurzivniho
 * sestupu. Pouze o vyhodnoceni vyrazu a volani funkci se stara
 * precedenci tabulka
 * @return  bool                    true v pripade uspesneho prekladu
 */
bool parse();

/**
 * nacte nasledujici token od lexikalniho analyzatoru a ulozi jej do globalni
 * promene cToken token
 */
void nextToken();

/**
 * <program>           ->  <declareList> <body> EO
 * @return  bool                    uspech aplikovani pravidla
 */
bool LL_program();

/**
 * <body>              ->  SCOPE EOL <statList> END SCOPE
 * @return  bool                    uspech aplikovani pravidla
 */
bool LL_body(pItem);

/**
 * <declareList>       ->  <declare> <declareList>
 * <declareList>       ->  ε
 * @return  bool                    uspech aplikovani pravidla
 */
bool LL_declareList();

/**
 * <declare>           ->  DECLARE FUNCTION id ( <paramsList> ) AS <type> EOL
 * <declare>           ->  FUNCTION id ( <paramsList> ) AS <type> EOL <statList> END FUNCTION EOL
 * @return  bool                    uspech aplikovani pravidla
 */
bool LL_declare();

/**
 * <paramsList>        ->  <param> <paramNext>
 * <paramsList>        ->  ε
 * @return  bool                    uspech aplikovani pravidla
 */
bool LL_paramList();

/**
 * <paramNext>         ->  , <param> <paramNext>
 * <paramNext>         ->  ε
 * @return  bool                    uspech aplikovani pravidla
 */
bool LL_paramNext();

/**
 * <param>             ->  id AS <type>
 * @return  bool                    uspech aplikovani pravidla
 */
bool LL_param();

/**
 * <stat>              ->  id = <expr>
 * <stat>              ->  INPUT id
 * <stat>              ->  PRINT <expr> ; <exprList>
 * <stat>              ->  IF <expr> THEN EOL <statList> ELSE EOL <statList> END IF
 * <stat>              ->  DO WHILE <expr> EOL <statList> LOOP
 * <stat>              ->  RETURN <expr>
 * <stat>              ->  ε
 * <stat>              ->  id[function](...)...
 * @return  bool                    uspech aplikovani pravidla
 */
bool LL_stat(pItem);

/**
 * <ifstat>            ->  ELSE EOL <statList>
 * <ifstat>            ->  ELSEIF THEN EOL <statList> <ifstat>
 * <ifstat>            ->  ε
 * @return  bool                     uspech aplikovani pravidla
 */
bool LL_ifstat();

/**
 * <statList>          ->  <stat> EOL <statList>
 * <statList>          ->  ε
 * @return  bool                     uspech aplikovani pravidla
 */
bool LL_statList(pItem funcItem);

/**
 * <exprList>          ->  <expr> ; <exprList>
 * <exprList>          ->  ε
 * @return  bool                     uspech aplikovani pravidla
 */
bool LL_exprList();

/**
 * zmena pristupu z rekurzivniho sestupu na syn. analyzu podle precedencni
 * tabulky
 * @return  symbolType               uspech aplikovani pravidla
 */
symbolType LL_expr();

/**
 * <declareVar>        ->  DIM id AS <type> <definition>
 * @return  bool                     uspech aplikovani pravidla
 */
bool LL_declareVar();

/**
 * <definition>        ->  = <expr>
 * <definition>        ->  ε
 * @return  bool                     uspech aplikovani pravidla
 */
bool LL_definition();

/**
 * <type>              ->  INTEGER
 * <type>              ->  DOUBLE
 * <type>              ->  STRING
 * @return  bool                     uspech aplikovani pravidla
 */
bool LL_type();

/**
 * <space>             ->  EOL <space>
 * <space>             ->  ε
 * @return  bool                     uspech aplikovani pravidla
 */
bool LL_space(bool);

/**
 *  $                  ->  <space> <program> <space> EOF
 * @return  bool                     uspech aplikovani pravidla
 */
bool LL_begin();

/**
 * volani akci generatoru instrukci ktere se postaraji o ukonceni ramce
 * dale nakopirovani upravenych viditelnych promnenych do lokalniho ramce
 */
void destroyFrame();

#endif

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
 *  Soubor: instruction_generator.c
 *  Autori: Jakub Trubka
 *
 *
 *	Datum:  08-11-2017
 *  Uprava: 05-12-2017
 */

#include "instruction_generator.h"
#define FUNCTION_COUNT 14
//tabulka prikazu pouzita pro vypis pomocnych funkci pokud budou potreba
bool functionArrayBool[FUNCTION_COUNT] = {
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
};
//listy instrukci
pInsList bodyList = NULL, functionList = NULL, ifList = NULL, whileList = NULL;
//prepinac mezi body a function listem
bool instructionSwitch = true;

static char *functionArray[FUNCTION_COUNT] = {
//navesti typecontrol provede typovou kontrolu dvou cisel na zasobniku, pokud se jedna o int a flout prevede oba na float
    "LABEL $TYPECONTROL\n\
CREATEFRAME\n\
DEFVAR TF@var1\n\
DEFVAR TF@var2\n\
POPS TF@var1\n\
POPS TF@var2\n\
DEFVAR TF@type1\n\
DEFVAR TF@type2\n\
TYPE TF@type1 TF@var1\n\
TYPE TF@type2 TF@var2\n\
JUMPIFEQ $TYPESAME TF@type1 TF@type2\n\
JUMPIFEQ $TYPE2 TF@type1 string@float\n\
INT2FLOAT TF@var1 TF@var1\n\
JUMP $TYPESAME\n\
LABEL $TYPE2\n\
INT2FLOAT TF@var2 TF@var2\n\
LABEL $TYPESAME\n\
PUSHS TF@var2\n\
PUSHS TF@var1\n\
RETURN\n",
//navesti stackswitch prohodi dve horni hodnoty na stacku
    "LABEL $STACKSWITCH\n\
CREATEFRAME\n\
DEFVAR TF@var1\n\
DEFVAR TF@var2\n\
POPS TF@var1\n\
POPS TF@var2\n\
PUSHS TF@var1\n\
PUSHS TF@var2\n\
RETURN\n",
//navesti CONCAT provadi konkatenaci dvou retezcu
    "LABEL $CONCAT\n\
CREATEFRAME\n\
DEFVAR TF@var1\n\
DEFVAR TF@var2\n\
POPS TF@var2\n\
POPS TF@var1\n\
CONCAT TF@var1 TF@var1 TF@var2\n\
PUSHS TF@var1\n\
RETURN\n",
//navesti pro prevod 2 hodnot na float
    "LABEL $INT2FLOAT\n\
CREATEFRAME\n\
DEFVAR TF@var1\n\
DEFVAR TF@var2\n\
POPS TF@var1\n\
POPS TF@var2\n\
DEFVAR TF@type\n\
TYPE TF@type TF@var1\n\
JUMPIFEQ $INT2FLOATTYPE1SAME TF@type string@float\n\
INT2FLOAT TF@var1 TF@var1\n\
LABEL $INT2FLOATTYPE1SAME\n\
TYPE TF@type TF@var2\n\
JUMPIFEQ $INT2FLOATTYPE2SAME TF@type string@float\n\
INT2FLOAT TF@var2 TF@var2\n\
LABEL $INT2FLOATTYPE2SAME\n\
PUSHS TF@var2\n\
PUSHS TF@var1\n\
RETURN\n",
//funkce prevede 2 hodnoty na int
    "LABEL $FLOAT2INT\n\
CREATEFRAME\n\
DEFVAR TF@var1\n\
DEFVAR TF@var2\n\
POPS TF@var1\n\
POPS TF@var2\n\
DEFVAR TF@type\n\
TYPE TF@type TF@var1\n\
JUMPIFEQ $FLOAT2INTTYPE1SAME TF@type string@int\n\
FLOAT2R2EINT TF@var1 TF@var1\n\
LABEL $FLOAT2INTTYPE1SAME\n\
TYPE TF@type TF@var2\n\
JUMPIFEQ $FLOAT2INTTYPE2SAME TF@type string@int\n\
FLOAT2R2EINT TF@var2 TF@var2\n\
LABEL $FLOAT2INTTYPE2SAME\n\
PUSHS TF@var2\n\
PUSHS TF@var1\n\
RETURN\n",
//funkce pocitajici modulo
    "LABEL $MODULO\n\
CREATEFRAME\n\
DEFVAR TF@bool\n\
DEFVAR TF@var1\n\
DEFVAR TF@var2\n\
DEFVAR TF@priznak\n\
POPS TF@var2\n\
POPS TF@var1\n\
JUMPIFNEQ $MODULOCONTINUE TF@var2 int@0\n\
PUSHS TF@var1\n\
RETURN\n\
LABEL $MODULOCONTINUE\n\
GT TF@bool TF@var1 int@0\n\
JUMPIFEQ $MODULONENASOBIT TF@bool bool@true\n\
MUL TF@var1 TF@var1 int@-1\n\
LABEL $MODULONENASOBIT\n\
LT TF@bool TF@var2 int@0\n\
JUMPIFEQ $MODULOPRIZNAK1 TF@bool bool@true\n\
MOVE TF@priznak int@1\n\
JUMP $MODULOPRIZNAK2\n\
LABEL $MODULOPRIZNAK1\n\
MOVE TF@priznak int@-1\n\
MUL TF@var2 TF@var2 int@-1\n\
LABEL $MODULOPRIZNAK2\n\
LABEL $MODULOVETEV\n\
LT TF@bool TF@var1 TF@var2\n\
JUMPIFEQ $MODULOKONEC TF@bool bool@true\n\
SUB TF@var1 TF@var1 TF@var2\n\
JUMP $MODULOVETEV\n\
LABEL $MODULOKONEC\n\
MUL TF@var1 TF@var1 TF@priznak\n\
PUSHS TF@var1\n\
RETURN\n",
//kod generujici nerovnost
    "LABEL $NOTEQUALS\n\
EQS\n\
PUSHS bool@true\n\
JUMPIFNEQS $NOTEQUALSTRUE\n\
PUSHS bool@false\n\
RETURN\n\
LABEL $NOTEQUALSTRUE\n\
PUSHS bool@true\n\
RETURN\n",
//kod generujici <=
    "LABEL $LESSEQUAL\n\
CREATEFRAME\n\
DEFVAR TF@var1\n\
DEFVAR TF@var2\n\
DEFVAR TF@bool\n\
POPS TF@var2\n\
POPS TF@var1\n\
LT TF@bool TF@var1 TF@var2\n\
JUMPIFEQ $LESSEQUALTRUE TF@bool bool@true\n\
JUMPIFEQ $LESSEQUALTRUE TF@var1 TF@var2\n\
PUSHS bool@false\n\
RETURN\n\
LABEL $LESSEQUALTRUE\n\
PUSHS bool@true\n\
RETURN\n",
//kod generujici >=
    "LABEL $GREATEREQUAL\n\
CREATEFRAME\n\
DEFVAR TF@var1\n\
DEFVAR TF@var2\n\
DEFVAR TF@bool\n\
POPS TF@var2\n\
POPS TF@var1\n\
LT TF@bool TF@var1 TF@var2\n\
JUMPIFNEQ $GREATEREQUALTRUE TF@bool bool@true\n\
JUMPIFEQ $GREATEREQUALTRUE TF@var1 TF@var2\n\
PUSHS bool@false\n\
RETURN\n\
LABEL $GREATEREQUALTRUE\n\
PUSHS bool@true\n\
RETURN\n",
//kod pro vestavenou funkci na vypocet podretezce
    "LABEL substr\n\
CREATEFRAME\n\
DEFVAR TF@tmp\n\
DEFVAR TF@bool\n\
DEFVAR TF@start\n\
DEFVAR TF@stop\n\
DEFVAR TF@stringdest\n\
DEFVAR TF@string\n\
MOVE TF@stringdest string@\n\
POPS TF@stop\n\
POPS TF@start\n\
POPS TF@string\n\
LT TF@bool TF@start int@1\n\
JUMPIFEQ $substrend TF@bool bool@true\n\
JUMPIFEQ $substrend TF@string string@\n\
STRLEN TF@tmp TF@string\n\
LT TF@bool TF@tmp TF@start\n\
JUMPIFEQ $substrend TF@bool bool@true\n\
SUB TF@tmp TF@tmp TF@start\n\
GT TF@bool TF@stop TF@tmp\n\
JUMPIFEQ $substruprava TF@bool bool@true\n\
GT TF@bool int@0 TF@stop\n\
JUMPIFEQ $substruprava TF@bool bool@true\n\
JUMP $substr2\n\
LABEL $substruprava\n\
MOVE TF@stop TF@tmp\n\
ADD TF@stop TF@stop int@1\n\
LABEL $substr2\n\
SUB TF@start TF@start int@1\n\
LABEL $substrloop\n\
JUMPIFEQ $substrend TF@stop int@0\n\
GETCHAR TF@tmp TF@string TF@start\n\
ADD TF@start TF@start int@1\n\
SUB TF@stop TF@stop int@1\n\
CONCAT TF@stringdest TF@stringdest TF@tmp\n\
JUMP $substrloop\n\
LABEL $substrend\n\
PUSHS TF@stringdest\n\
RETURN\n",
//kod pro vestavenou funkci vraci delku retezce na zasobniku
    "LABEL length\n\
CREATEFRAME\n\
DEFVAR TF@string\n\
DEFVAR TF@length\n\
POPS TF@string\n\
STRLEN TF@length TF@string\n\
PUSHS TF@length\n\
RETURN\n",
//kod pro vestavenou funkci asc
    "LABEL asc\n\
CREATEFRAME\n\
DEFVAR TF@string\n\
DEFVAR TF@position\n\
DEFVAR TF@value\n\
MOVE TF@value int@0\n\
DEFVAR TF@length\n\
DEFVAR TF@bool\n\
POPS TF@position\n\
POPS TF@string\n\
STRLEN TF@length TF@string\n\
LT TF@bool TF@position int@1\n\
JUMPIFEQ $ascend TF@bool bool@true\n\
GT TF@bool TF@position TF@length\n\
JUMPIFEQ $ascend TF@bool bool@true\n\
SUB TF@position TF@position int@1\n\
STRI2INT TF@value TF@string TF@position\n\
LABEL $ascend\n\
PUSHS TF@value\n\
RETURN\n",
//kod pro vestavenou funkci chr
    "LABEL chr\n\
CREATEFRAME\n\
DEFVAR TF@value\n\
MOVE TF@value string@\n\
DEFVAR TF@asci\n\
DEFVAR TF@bool\n\
POPS TF@asci\n\
LT TF@bool TF@asci int@0\n\
JUMPIFEQ $chrend TF@bool bool@true\n\
GT TF@bool TF@asci int@255\n\
JUMPIFEQ $chrend TF@bool bool@true\n\
INT2CHAR TF@value TF@asci\n\
LABEL $chrend\n\
PUSHS TF@value\n\
RETURN\n",
    //kod generujici pretypovani int->float nebo float->int
    "LABEL $TYPECHANGE\n\
CREATEFRAME\n\
DEFVAR TF@var1\n\
POPS TF@var1\n\
DEFVAR TF@type1\n\
TYPE TF@type1 TF@var1\n\
JUMPIFEQ $TYPECHANGE2 TF@type1 string@float\n\
INT2FLOAT TF@var1 TF@var1\n\
PUSHS TF@var1\n\
RETURN\n\
LABEL $TYPECHANGE2\n\
FLOAT2R2EINT TF@var1 TF@var1\n\
PUSHS TF@var1\n\
RETURN",
};
//seznam nazvu navesti "vestavenych" funkci
static char *labelArray[FUNCTION_COUNT] = {
    "$TYPECONTROL",
    "$STACKSWITCH",
    "$CONCAT",
    "$INT2FLOAT",
    "$FLOAT2INT",
    "$MODULO",
    "$NOTEQUALS",
    "$LESSEQUAL",
    "$GREATEREQUAL",
    "substr",
    "length",
    "asc",
    "chr",
    "$TYPECHANGE",
};
//pocitadlo indexu navesti
int labelCounter[3] = {0,0,0};
//pomocne pro vypis
static char *variableArray[24] = {
        "TF@$tmp",
        "GF@",
        "TF@",
        "int@",
        "float@",
        "string@",
        "TF@$return",
        "",
        "0",
        "0.0",
        "LF@",
        "$ELSE",
        "$IFEND",
        "bool@false",
        "bool@true",
        "$WHILE",
        "$WHILEEND",
        "int",
        "float",
        "string",
        "bool",
        "int@-1",
        "string@?\\032",
        "TF@$tmpvar"
};
//seznam istrukci
static char *insArray[55] = {
        "MOVE",           //0
        "CREATEFRAME",    //1
        "PUSHFRAME",      //2
        "POPFRAME",       //3
        "DEFVAR",         //4
        "CALL",           //5
        "RETURN",         //6
        "PUSHS",          //7
        "POPS",           //8
        "CLEARS",         //9
        "ADD",            //10
        "SUB",            //11
        "MUL",            //12
        "DIV",            //13
        "ADDS",           //14
        "SUBS",           //15
        "MULS",           //16
        "DIVS",           //17
        "LT",             //18
        "GT",             //19
        "EQ",             //20
        "LTS",            //21
        "GTS",            //22
        "EQS",            //23
        "AND",            //24
        "OR",             //25
        "NOT",            //26
        "ANDS",           //27
        "ORS",            //28
        "NOTS",           //29
        "INT2FLOAT",      //30
        "FLOAT2INT",      //31
        "FLOAT2REINT",    //32
        "FLOAT2R2OINT",   //33
        "INT2CHAR",       //34
        "STRI2INT",       //35
        "INT2FLOATS",     //36
        "FLOAT2INTS",     //37
        "FLOAT2R2EINTS",   //38
        "FLOAT2R2OINTS",  //39
        "INT2CHARS",      //40
        "STRI2INTS",      //41
        "READ",           //42
        "WRITE",          //43
        "CONCAT",         //44
        "STRLEN",         //45
        "GETCHAR",        //46
        "SETCHAR",        //47
        "TYPE",           //48
        "LABEL",          //49
        "JUMP",           //50
        "JUMPIFEQ",       //51
        "JUMPIFNEQ",      //52
        "JUMPIFEQS",      //53
        "JUMPIFNEQS",     //54
};
//pocitadlo vytvorenych framu pri zanorovani uvnitr funkci
unsigned int frameCounter = 0;

bool generateArithmeticInstruction(expresionEnum id, pTerm arg1, pTerm arg2){
    pInsList instructionList = NULL;
    //prepinac body/function listu
    if(instructionSwitch){
        instructionList = bodyList;
    }
    else{
        instructionList = functionList;
    }
    switch (id){
        case _ADD:
            generateAdd(arg1, arg2, instructionList);
            break;
        case _MUL:
            generateMul(arg1, arg2, instructionList);
            break;
        case _DIV:
            generateDiv(arg1, arg2, instructionList);
            break;
        case _SUB:
            generateSub(arg1, arg2, instructionList);
            break;
        case _MOD:
            generateModDiv(instructionList);
            break;
        case _LESS:
            generateLess(arg1, arg2, instructionList);
            break;
        case _EQUAL:
            generateEqual(arg1, arg2, instructionList);
            break;
        case _GREATER:
            generateGreater(arg1, arg2, instructionList);
            break;
        case _NOTEQUAL:
            generateNotEqual(arg1, arg2, instructionList);
            break;
        case _LESSEQUAL:
            generateLessEqual(arg1, arg2, instructionList);
            break;
        case _GREATEREQUAL:
            generateGreaterEqual(arg1, arg2, instructionList);
            break;
        case _FUNC:
            generateFunctionCall(arg1, instructionList);
            break;
        case _EXP: //arg1 int/double/string
            generatePush(arg1, instructionList);
            break;
        case _UNARY_MINUS:
            generateUnaryMinus(instructionList);
            break;
        default:
            return false;
    }
    return true;
}

bool generateSceletInstruction(sceletEnum id, pTerm arg1){
    pInsList instructionList = NULL;
    //prepinac body/function listu
    if(instructionSwitch){
        instructionList = bodyList;
    }
    else{
        instructionList = functionList;
    }
    switch (id){
        case _CREATE_FUNC:
            instructionSwitch = false;
            generateCreateFunc(arg1, functionList);
            break;
        case _END_FUNCTION:
            generateEndFunc(instructionList);
            instructionSwitch = true;
            break;
        case _DECLARE_VAR:
            generateDefvar(arg1,instructionList);
            break;
        case _MOVE:
            generateMove(arg1, instructionList);
            break;
        case _READ:
            generateRead(arg1, instructionList);
            break;
        case _PRINT:
            generatePrint(instructionList);
            break;
        case _IF:
            generateIf(instructionList);
            break;
        case _ELSEIF:
            generateElseIf(instructionList);
            break;
        case _ELSE:
            generateElse(instructionList);
            break;
        case _ENDIF:
            generateEndIf(instructionList);
            break;
        case _CREATE_WHILE:
            generateCreateWhile(instructionList);
            break;
        case _WHILE:
            generateWhile(instructionList);
            break;
        case _LOOP:
            generateLoop(instructionList);
            break;
        case _RETURN:
            generateReturn(instructionList);
            break;
        case _CREATE_BLOCK:
            generateCreateBlock(instructionList);
            break;
        case _COPY_IN:
            generateCopyIn(arg1, instructionList);
            break;
        case _COPY_OUT:
            generateCopyOut(arg1, instructionList);
            break;
        case _END_BLOCK:
            generateEndBlock(instructionList);
            break;
        case _INIT_VAR:
            generateInit(arg1, instructionList);
            break;
        default:
            return false;
            break;
    }
    return true;
}

char *labelGenerator(int ifSwitch){  //0 = if, 1 = while, 2 = endif
    char *index = NULL;
    //rozvetveni zda generujeme if ci while
    if(ifSwitch == 0){
        //alokace ukazatele na index navesti
        if((index = malloc(sizeof(int)*8+1)) == NULL){
            errorMsg(ERROR_INTER, 0, "nepovedla se alokace indexu");
        }

        //pridani do GC
        initGCItem(index, "LABEL");
        //prevod intove hodnoty do retezce charu
        sprintf(index, "%d", labelCounter[0]);
        //inkrementace pocitadla indexu
        labelCounter[0]++;
    }
    else if(ifSwitch == 1){
        //verze pro while
        if((index = malloc(sizeof(int)*8+1)) == NULL){
            errorMsg(ERROR_INTER, 0, "nepovedla se alokace indexu");
        }

        //pridani do GC
        initGCItem(index, "LABEL");

        sprintf(index, "%d", labelCounter[1]);
        labelCounter[1]++;
    }else{
        //verze pro endiff
        if((index = malloc(sizeof(int)*8+1)) == NULL){
            errorMsg(ERROR_INTER, 0, "nepovedla se alokace indexu");
        }

        //pridani do GC
        initGCItem(index, "LABEL");

        sprintf(index, "%d", labelCounter[2]);
    }
    return index;
}

bool generateChangeTypeInstruction( int i) {
    pInsList instructionList = NULL;
    //prepinac body/function listu
    if(instructionSwitch){
        instructionList = bodyList;
    }
    else{
        instructionList = functionList;
    }
    int index = 0;
    if(i!=0){
        insAdd(instructionList, PUSHFRAME, NULL, NULL, NULL, NULL);
        insAdd(instructionList, CREATEFRAME, NULL, NULL, NULL, NULL);
    }

    while(index < i){
        char* varName;
        if((varName = malloc(sizeof(int)*8+1)) == NULL){
            errorMsg(ERROR_INTER, 0, "nepovedla se alokace indexu");
        }

        //pridani do GC
        initGCItem(varName, "DEFVAR");
        //prevod intove hodnoty do retezce charu
        sprintf(varName, "%d", index);
        insAdd(instructionList, DEFVAR, variableArray[23], varName, NULL, NULL);
        insAdd(instructionList, POPS, variableArray[23], varName, NULL, NULL);
        index++;
    }
    functionArrayBool[13] = true;
    insAdd(instructionList, PUSHFRAME, NULL, NULL, NULL, NULL);
    insAdd(instructionList, CALL, labelArray[13], NULL, NULL, NULL);
    insAdd(instructionList, POPFRAME, NULL, NULL, NULL, NULL);
    while(index > 0 && i != 0){
        char* varName;
        if((varName = malloc(sizeof(int)*8+1)) == NULL){
            errorMsg(ERROR_INTER, 0, "nepovedla se alokace indexu");
        }

        //pridani do GC
        initGCItem(varName, "DEFVAR");
        //prevod intove hodnoty do retezce charu
        sprintf(varName, "%d", (index-1));
        insAdd(instructionList, PUSHS, variableArray[23], varName, NULL, NULL);
        index--;
    }
    if(i != 0){
        insAdd(instructionList, POPFRAME, NULL, NULL, NULL, NULL);
    }
    return true;
}

void generateUnaryMinus(pInsList instructionList){
    insAdd(instructionList, PUSHS, variableArray[21], NULL, NULL, NULL);
    functionArrayBool[0] = true;
    insAdd(instructionList,PUSHFRAME, NULL, NULL, NULL, NULL);
    insAdd(instructionList,CALL, labelArray[0], NULL, NULL, NULL);
    insAdd(instructionList,POPFRAME, NULL, NULL, NULL, NULL);
    insAdd(instructionList, MULS, NULL, NULL, NULL, NULL);
}

//scitani scitani nemuze probihat v libovolnem poradi kvuli konkatenaci retezcu
void generateAdd(pTerm arg1, pTerm arg2, pInsList instructionList){
    if(arg1->termType == __EXPR_STRING || arg2->termType == __EXPR_STRING){
        insAdd(instructionList,PUSHFRAME, NULL, NULL, NULL, NULL);
        insAdd(instructionList,CALL, labelArray[2], NULL, NULL, NULL);
        insAdd(instructionList,POPFRAME, NULL, NULL, NULL, NULL);
        functionArrayBool[2] = true;
        return;
    }
    insTypeControl(arg1, arg2, instructionList);
    insAdd(instructionList, ADDS, NULL, NULL, NULL, NULL);
}

//nasobeni muze probihat v libovolnem poradi
void generateMul(pTerm arg1, pTerm arg2, pInsList instructionList){
    //nutnost typove kontroly
    insTypeControl(arg1, arg2, instructionList);
    insAdd(instructionList, MULS, NULL, NULL, NULL, NULL);

}

//odecitani nemuze probihat v kteremkoliv poradi
void generateSub(pTerm arg1, pTerm arg2, pInsList instructionList){
    insTypeControl(arg1, arg2, instructionList);
    insAdd(instructionList, SUBS, NULL, NULL, NULL, NULL);
}

//deleni nemuze probihat v kteremkoliv poradi
void generateDiv(pTerm arg1, pTerm arg2, pInsList instructionList){
    //jedno z cisel muze byt int, musi dojit k pretypovani
    if(arg1->termType != __EXPR_DOUBLE || arg2->termType != __EXPR_DOUBLE){
        insAdd(instructionList,PUSHFRAME, NULL, NULL, NULL, NULL);
        insAdd(instructionList,CALL, labelArray[3], NULL, NULL, NULL);
        insAdd(instructionList,POPFRAME, NULL, NULL, NULL, NULL);
        functionArrayBool[3] = true;
    }
    insAdd(instructionList, DIVS, NULL, NULL, NULL, NULL);
}

void generateModDiv(pInsList instructionList){
        insAdd(instructionList,PUSHFRAME, NULL, NULL, NULL, NULL);
        insAdd(instructionList,CALL, labelArray[4], NULL, NULL, NULL);
        insAdd(instructionList,POPFRAME, NULL, NULL, NULL, NULL);
        functionArrayBool[4] = true;
        insAdd(instructionList,PUSHFRAME, NULL, NULL, NULL, NULL);
        insAdd(instructionList,CALL, labelArray[3], NULL, NULL, NULL);
        insAdd(instructionList,POPFRAME, NULL, NULL, NULL, NULL);
        functionArrayBool[3] = true;
    insAdd(instructionList, DIVS, NULL, NULL, NULL, NULL);
    insAdd(instructionList, FLOAT2INTS, NULL, NULL, NULL, NULL);
}

void generateLess(pTerm arg1, pTerm arg2, pInsList instructionList){
    insTypeControl(arg1, arg2, instructionList);
    insAdd(instructionList, LTS, NULL, NULL, NULL, NULL);
}

void generateGreater(pTerm arg1, pTerm arg2, pInsList instructionList){
    insTypeControl(arg1, arg2, instructionList);
    insAdd(instructionList, GTS, NULL, NULL, NULL, NULL);
}

void generateEqual(pTerm arg1, pTerm arg2, pInsList instructionList){
    insTypeControl(arg1, arg2, instructionList);
    insAdd(instructionList, EQS, NULL, NULL, NULL, NULL);
}

void generateNotEqual(pTerm arg1, pTerm arg2, pInsList instructionList){
    insTypeControl(arg1, arg2, instructionList);
    functionArrayBool[6] = true;
    insAdd(instructionList,CALL, labelArray[6], NULL, NULL, NULL);
}

void generateLessEqual(pTerm arg1, pTerm arg2, pInsList instructionList){
    insTypeControl(arg1, arg2, instructionList);
    functionArrayBool[7] = true;
    insAdd(instructionList,PUSHFRAME, NULL, NULL, NULL, NULL);
    insAdd(instructionList,CALL, labelArray[7], NULL, NULL, NULL);
    insAdd(instructionList,POPFRAME, NULL, NULL, NULL, NULL);
}

void generateGreaterEqual(pTerm arg1, pTerm arg2, pInsList instructionList){
    insTypeControl(arg1, arg2, instructionList);
    functionArrayBool[8] = true;
    insAdd(instructionList,PUSHFRAME, NULL, NULL, NULL, NULL);
    insAdd(instructionList,CALL, labelArray[8], NULL, NULL, NULL);
    insAdd(instructionList,POPFRAME, NULL, NULL, NULL, NULL);
}

void generateFunctionCall(pTerm arg1, pInsList instructionList){
    //volani predem definovanych funkcich zpusobi jejich vypsani
    if(strcmp(arg1->token->cPointer, "length") == 0){
        functionArrayBool[10] = true;
    }else if(strcmp(arg1->token->cPointer, "substr") == 0){
        functionArrayBool[9] = true;
    }else if(strcmp(arg1->token->cPointer, "asc") == 0){
        functionArrayBool[11] = true;
    }else if(strcmp(arg1->token->cPointer, "chr") == 0){
        functionArrayBool[12] = true;
    }
    insAdd(instructionList,PUSHFRAME, NULL, NULL, NULL, NULL);
    insAdd(instructionList,CALL, arg1->token->cPointer, NULL, NULL, NULL);
    insAdd(instructionList,POPFRAME, NULL, NULL, NULL, NULL);
}

void generatePush(pTerm arg1, pInsList instructionList){
    //kontrola typu promene pro generovani instrukce
    char* printVariable = NULL;
    if(arg1->termType == __ID){
        printVariable = frameControl(arg1);
    }else if(arg1->termType == __INT){
        printVariable = variableArray[3];
    }else if(arg1->termType == __DOUBLE){
        printVariable = variableArray[4];
    }else{
        printVariable = variableArray[5];
    }
    insAdd(instructionList, PUSHS, printVariable, arg1->token->cPointer, NULL, NULL);
}

void generateDefvar(pTerm arg1, pInsList instructionList){
    char* printVariable = frameControl(arg1);
    insAdd(instructionList, DEFVAR, printVariable, arg1->token->cPointer, NULL, NULL);

    pItem tmpItem = hTabFind(symbolTableActual, arg1->token->cPointer);
    if(tmpItem->data->type == STRING){
        insAdd(instructionList,MOVE, printVariable, arg1->token->cPointer, variableArray[5], variableArray[7]);
    }else if(tmpItem->data->type == DOUBLE){
        insAdd(instructionList,MOVE, printVariable, arg1->token->cPointer, variableArray[4], variableArray[9]);
    }else{
        insAdd(instructionList,MOVE, printVariable, arg1->token->cPointer, variableArray[3], variableArray[8]);
    }
}

void generatePrint(pInsList instructionList){
    insAdd(instructionList,POPS, variableArray[0], NULL, NULL, NULL);
    insAdd(instructionList,WRITE, variableArray[0], NULL, NULL, NULL);
}

void generateRead(pTerm arg1, pInsList instructionList){
    char* printVariable = frameControl(arg1);
    pItem tmpItem = hTabFind(symbolTableActual, arg1->token->cPointer);
    insAdd(instructionList, WRITE, variableArray[22], NULL, NULL, NULL);
    if(tmpItem->data->type == STRING){
        insAdd(instructionList,READ, printVariable, arg1->token->cPointer, variableArray[19], NULL);
    }else if(tmpItem->data->type == DOUBLE){
        insAdd(instructionList,READ, printVariable, arg1->token->cPointer, variableArray[18], NULL);
    }else{
        insAdd(instructionList,READ, printVariable, arg1->token->cPointer, variableArray[17], NULL);
    }
}

void generateMove(pTerm arg1, pInsList instructionList){
    char* printVariable = frameControl(arg1);
    insAdd(instructionList,POPS, printVariable, arg1->token->cPointer, NULL, NULL);
}

void generateCreateFunc(pTerm arg1, pInsList instructionList){
    insAdd(instructionList, LABEL, arg1->token->cPointer, NULL, NULL, NULL);
    insAdd(instructionList, CREATEFRAME, NULL, NULL, NULL, NULL);
    insAdd(instructionList, DEFVAR, variableArray[6], NULL, NULL, NULL);
    insAdd(instructionList, DEFVAR, variableArray[0], NULL, NULL, NULL);

    pItem tmpItem = hTabFind(symbolTableActual, arg1->token->cPointer);
    if(tmpItem->data->type == STRING){
        insAdd(instructionList,MOVE, variableArray[6], variableArray[5], variableArray[7], NULL);
    }else if(tmpItem->data->type == DOUBLE){
        insAdd(instructionList,MOVE, variableArray[6], variableArray[4], variableArray[9], NULL);
    }else{
        insAdd(instructionList,MOVE, variableArray[6], variableArray[3], variableArray[8], NULL);
    }

}

void generateEndFunc(pInsList instructionList){
    insAdd(instructionList, PUSHS, variableArray[6], NULL, NULL, NULL);
    insAdd(instructionList, RETURN, NULL, NULL, NULL, NULL);
    frameCounter = 0;
}

void generateReturn(pInsList instructionList){
    for(unsigned int i = 0; i < frameCounter; i++){
        insAdd(instructionList,POPFRAME, NULL, NULL, NULL, NULL);
    }
    insAdd(instructionList, RETURN, NULL, NULL, NULL, NULL);
}

void generateCreateBlock(pInsList instructionList){
    if(instructionList == functionList){
        frameCounter++;
    }
    insAdd(instructionList,PUSHFRAME, NULL, NULL, NULL, NULL);
    insAdd(instructionList,CREATEFRAME, NULL, NULL, NULL, NULL);
    insAdd(instructionList, DEFVAR, variableArray[0], NULL, NULL, NULL);
}

void generateEndBlock(pInsList instructionList){
    insAdd(instructionList,POPFRAME, NULL, NULL, NULL, NULL);
    if(instructionList == functionList){
        frameCounter--;
    }
}

void generateInit(pTerm arg1, pInsList instructionList) {
    char* printVariable = frameControl(arg1);

    pItem tmpItem = hTabFind(symbolTableActual, arg1->token->cPointer);
    if(tmpItem->data->type == STRING){
        insAdd(instructionList,MOVE, printVariable, arg1->token->cPointer, variableArray[5], variableArray[7]);
    }else if(tmpItem->data->type == DOUBLE){
        insAdd(instructionList,MOVE, printVariable, arg1->token->cPointer, variableArray[4], variableArray[9]);
    }else{
        insAdd(instructionList,MOVE, printVariable, arg1->token->cPointer, variableArray[3], variableArray[8]);
    }
}

void generateCopyIn(pTerm arg1, pInsList instructionList){
    insAdd(instructionList, DEFVAR, variableArray[2], arg1->token->cPointer, NULL, NULL);
    insAdd(instructionList, MOVE, variableArray[2], arg1->token->cPointer, variableArray[10], arg1->token->cPointer);
}

void generateCopyOut(pTerm arg1, pInsList instructionList){
    insAdd(instructionList, MOVE, variableArray[10], arg1->token->cPointer, variableArray[2], arg1->token->cPointer);
}

void generateIf(pInsList instructionList){
    labelCounter[2] = labelCounter[0];
    char *index = labelGenerator(0);
    insAdd(instructionList, PUSHS, variableArray[13], NULL, NULL, NULL);
    insAdd(instructionList, JUMPIFEQS, variableArray[11], index, NULL, NULL);
    insAdd(ifList, LABEL, variableArray[12], index, NULL, NULL);
    insAdd(ifList, LABEL, variableArray[11], index, NULL, NULL);
    insAdd(ifList, JUMP, variableArray[12], index, NULL, NULL);
}

void generateElse(pInsList instructionList){
    insConnect(ifList, instructionList);
    insConnect(ifList, instructionList);
}

void generateEndIf(pInsList instructionList){
    insConnect(ifList, instructionList);
}

void generateElseIf(pInsList instructionList){
    char *indexIf = labelGenerator(0);
    char *indexElseIf =labelGenerator(3);
    insAdd(instructionList, PUSHS, variableArray[13], NULL, NULL, NULL);
    insAdd(instructionList, JUMPIFEQS, variableArray[11], indexIf, NULL, NULL);
    insAdd(ifList, LABEL, variableArray[11], indexIf, NULL, NULL);
    insAdd(ifList, JUMP, variableArray[12], indexElseIf, NULL, NULL);
}

void generateCreateWhile(pInsList instructionList){
    char *index = labelGenerator(1);
    insAdd(instructionList, LABEL, variableArray[15], index, NULL, NULL);
    insAdd(whileList, LABEL, variableArray[16], index, NULL, NULL);
    insAdd(whileList, JUMP, variableArray[15], index, NULL, NULL);
    insAdd(whileList, JUMPIFEQS, variableArray[16], index, NULL, NULL);
    insAdd(whileList, PUSHS, variableArray[13], NULL, NULL, NULL);
}

void generateWhile(pInsList instructionList){
    insConnect(whileList, instructionList);
    insConnect(whileList, instructionList);
}

void generateLoop(pInsList instructionList){
    insConnect(whileList, instructionList);
    insConnect(whileList, instructionList);
}

void insTypeControl(pTerm arg1, pTerm arg2, pInsList instructionList){
    //kontrola zda jedno z cisel neni int a druhe float ci opacne, pokud ano, dojde k volani funkce k pretypovani
    if(arg1->termType != arg2->termType){
        functionArrayBool[0] = true;
        insAdd(instructionList,PUSHFRAME, NULL, NULL, NULL, NULL);
        insAdd(instructionList,CALL, labelArray[0], NULL, NULL, NULL);
        insAdd(instructionList,POPFRAME, NULL, NULL, NULL, NULL);
    }
}

char *frameControl(pTerm arg1){
    char *arg2 = NULL;
    //nalezeni promenne
    pItem tmpItem = hTabFind(symbolTableActual, arg1->token->cPointer);
    if(tmpItem == NULL){
        errorMsg(ERROR_LEX, 0, "Tabulka symbolu vratila NULL");
    }else{
        //rozhodnuti a framu ktery je treba pouzit
        if(tmpItem->visibilitySignature == 0){
            arg2 = variableArray[1];
        }else{
            arg2 = variableArray[2];
        }
    }
    return arg2;
}

void insListInit(){
    if((bodyList = malloc(sizeof(struct instructionList))) == NULL){
        errorMsg(ERROR_INTER, 0, "nepovedla se alokace inslistu");
    }
    else {
        //pridani do GC
        initGCItem(bodyList, "BODY");
    }

    if((functionList = malloc(sizeof(struct instructionList))) == NULL){
        errorMsg(ERROR_INTER, 0, "nepovedla se alokace inslistu");
    }
    else {
        //pridani do GC
        initGCItem(functionList, "FUNCTION");
    }

    if((ifList = malloc(sizeof(struct instructionList))) == NULL){
        errorMsg(ERROR_INTER, 0, "nepovedla se alokace inslistu");
    }
    else {
        //pridani do GC
        initGCItem(ifList, "IF");
    }

    if((whileList = malloc(sizeof(struct instructionList))) == NULL){
        errorMsg(ERROR_INTER, 0, "nepovedla se alokace inslistu");
    }
    else {
        //pridani do GC
        initGCItem(whileList, "WHILE");
    }

    bodyList->first = NULL;
    bodyList->last = NULL;
    bodyList->actual = NULL;

    functionList->first = NULL;
    functionList->last = NULL;
    functionList->actual = NULL;

    ifList->first = NULL;
    ifList->last = NULL;
    ifList->actual = NULL;

    whileList->first = NULL;
    whileList->last = NULL;
    whileList->actual = NULL;
}

void insConnect(pInsList fromList, pInsList toList){
    pInsItem toMove = fromList->first;
    fromList->first = fromList->first->right;

    toMove->left = NULL;
    toMove->right = toList->first;
    if(toList->first != NULL){
        toList->first->left = toMove;
    }
    else{
        toList->last = toMove;
    }
    toList->first = toMove;
}

void insAdd(pInsList L, instructionEnum id, char *argument1, char *argument2, char *argument3, char *argument4){
    if(L != NULL && L != NULL){
        pInsItem newItem= NULL;
        if((newItem = malloc(sizeof(struct instructionItem))) != NULL){
            newItem->id = id;                   //nastvaveni noveho prvku
            newItem->argument[0] = argument1;
            newItem->argument[1] = argument2;
            newItem->argument[2] = argument3;
            newItem->argument[3] = argument4;
            newItem->right = L->first;      //napojeni noveho prvku na zacatek seznamu
            newItem->left = NULL;
            if(L->first != NULL){
                L->first->left = newItem;
            }
            else{
                L->last = newItem;
            }
            L->first = newItem;

            //GC add
            initGCItem(newItem, "INSTR");

        }else{
            errorMsg(ERROR_INTER, 0, "nepovedla se alokace instrukce");
        }
    }
}

void insListPrint(pInsList instructionList, FILE *stream){
    instructionList->actual = instructionList->last;
    while (instructionList->actual != NULL) {
        fprintf(stream, "%s ", insArray[instructionList->actual->id]);
        for(int i=0; i<4; i++){
            if(instructionList->actual->argument[i] != NULL){
                if(strcmp(instructionList->actual->argument[i], "GF@") != 0 &&
                   strcmp(instructionList->actual->argument[i], "TF@") != 0 &&
                   strcmp(instructionList->actual->argument[i], "LF@") != 0 &&
                   strcmp(instructionList->actual->argument[i], "int@") != 0 &&
                   strcmp(instructionList->actual->argument[i], "float@") != 0 &&
                   strcmp(instructionList->actual->argument[i], "string@") != 0&&
                   strcmp(instructionList->actual->argument[i], "bool@") != 0 &&
                   strcmp(instructionList->actual->argument[i], "$IFEND") != 0 &&
                   strcmp(instructionList->actual->argument[i], "$ELSE") != 0 &&
                   strcmp(instructionList->actual->argument[i], "$WHILE") != 0 &&
                   strcmp(instructionList->actual->argument[i], "$WHILEEND") != 0 &&
                   strcmp(instructionList->actual->argument[i], "TF@$tmpvar") != 0){
                    fprintf(stream, "%s ", instructionList->actual->argument[i]);
                }else{
                    fprintf(stream, "%s", instructionList->actual->argument[i]);
                }
            }
        }
        fprintf(stream, "\n");
        instructionList->actual = instructionList->actual->left;
    }
}

void insPrint(FILE *stream){
    fprintf(stream, ".IFJcode17\n%s\n", insArray[CREATEFRAME]);
    insListPrint(bodyList, stream);
    fprintf(stream, "JUMP $END\n\n");
    insListPrint(functionList, stream);
    for(int i = 0;i < FUNCTION_COUNT; i++){
        if(functionArrayBool[i])
        {
            fprintf(stream, "%s\n", functionArray[i]);
        }
    }
    fprintf(stream, "LABEL $END\n");
    insFree();
}

void insFree(){
    insListFree(bodyList);
    insListFree(functionList);
    insListFree(ifList);
    insListFree(whileList);
}

void insListFree(pInsList instructionList){
    if(instructionList != NULL){
        pInsItem toClear = instructionList->first;
        while(toClear != NULL){
            if((strcmp(insArray[toClear->id], "LABEL") == 0 && strcmp(toClear->argument[0], "$ELSE") == 0)||(strcmp(insArray[toClear->id], "LABEL") == 0 && strcmp(toClear->argument[0], "$WHILE") == 0)){
                GCRemove(toClear->argument[1]);
            }
            instructionList->first = instructionList->first->right;
            GCRemove(toClear);
            toClear = instructionList->first;
        }
    }
    GCRemove(instructionList);
}

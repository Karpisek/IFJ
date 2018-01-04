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
 *  Soubor: custom_string.c
 *  Autori: Miroslav Karpisek
 *          Roman Zajic
 *          Jakub Trubka
 *
 *	Datum:  21-10-2017
 *  Uprava: 05-12-2017
 */


 #include "custom_string.h"
 #define STRING_LENGTH 9
 

void createString(cString *s){
  //alokace noveho retezce
  if((*s = malloc(sizeof(struct customString))) == NULL) {
      error(ERROR_INTER, "Chyba alokace pameti customString\n");
  }

  initGCItem(*s, "STRING");

  if (((*s)->str = (char*) malloc(STRING_LENGTH)) == NULL) {
    error(ERROR_INTER, "Chyba alokace pameti customString\n");
  }

  initGCItem((*s)->str, "CSTRING");

  (*s)->str[0] = '\0';
  (*s)->actSize = 0;
  (*s)->maxSize = STRING_LENGTH;
}

void destroyString(cString *s){
    GCRemove((*s)->str);
    GCRemove(*s);
}

void pushChar(cString *s, char c){
  //kontrola nutnosti realokace pameti
  if ((*s)->actSize + 1 >= (*s)->maxSize) {

    GCErase((*s)->str);
    if (((*s)->str = (char*) realloc((*s)->str, (*s)->actSize + STRING_LENGTH)) == NULL) {
      error(ERROR_INTER, "Chyba realokace pameti\n");
    }

    initGCItem((*s)->str, "STRING_RL");
    (*s)->maxSize = (*s)->actSize + STRING_LENGTH;
 }
 //(*s)->maxSize = (*s)->maxSize + STRING_LENGTH;
 (*s)->str[(*s)->actSize] = c;
 (*s)->actSize++;
 (*s)->str[(*s)->actSize] = '\0';
}

void popChar(cString s){
  if(s->actSize != 0) {
    s->actSize--;
    s->str[s->actSize] = '\0';
  }

}
bool stringKeywordCmp(cString s, char* keyword){
  return (strcmp(s->str, keyword) == 0 ? true : false);
}

bool stringStringCmp(cString s1, cString s2){
    return (strcmp(s1->str, s2->str) == 0 ? true : false);
}
void clearStringStr(cString *s){
    (*s)->str[0]='\0';
    (*s)->actSize = 0;
}

void normalizeStr(cString str, int* id) {
  int num = 0;

  switch (*id){
      case _ret_lit:
        if(strEscape(str) == false) error(ERROR_LEX, "Spatny format retezce: %s", str->str);
        break;

      case _celoc_lit_bin:
        if(baseToDec(str, 2, &num) == false) error(ERROR_LEX, "Neplatny format cisla v binarni soustave: %s", str->str);
        decToStr(str, num);
        *id = _celoc_lit;
        break;

      case _celoc_lit_oct:
        if(baseToDec(str, 8, &num) == false) error(ERROR_LEX, "Neplatny format cisla v osmickove soustave: %s", str->str);
        decToStr(str, num);
        *id = _celoc_lit;
        break;

      case _celoc_lit_hex:
        if(baseToDec(str, 16, &num) == false) error(ERROR_LEX, "Neplatny format cisla v sestnackove soustave: %s", str->str);
        decToStr(str, num);
        *id = _celoc_lit;
        break;

      default:
        for(int i = 0; i < str->actSize; i++) str->str[i] = tolower(str->str[i]);
  }
}

bool strEscape(cString str) {
  cString tmp;
  createString(&tmp);

  for(int i = 0; i < str->actSize; i++) {
    if(str->str[i] == '\\'){
      //Hlida jestli neni '\' na poslednim miste
      if(i < str->actSize - 1) {
        pushChar(&tmp, '\\');
        /* \025 */
        if((i + 3 <= str->actSize - 1) && isdigit(str->str[i+1]) && isdigit(str->str[i+2]) && isdigit(str->str[i+3]) ) {
          int val = ((str->str[i+1] - '0') * 100) + ((str->str[i+2] - '0') * 10) + (str->str[i+3] - '0');

          if(val > 0 && val <= 255) {
            pushChar(&tmp, str->str[i+1]);
            pushChar(&tmp, str->str[i+2]);
            pushChar(&tmp, str->str[i+3]);
            i += 3;
            continue;
          } else {
            destroyString(&tmp);
            return false;
          }
        }
        /* \n */
        else if(tolower(str->str[i+1]) == 'n') {
          pushChar(&tmp, '0');
          pushChar(&tmp, '1');
          pushChar(&tmp, '0');
          i++;
          continue;
        }
        /* \" */
        else if(str->str[i+1] == '"') {
          pushChar(&tmp, '0');
          pushChar(&tmp, '3');
          pushChar(&tmp, '4');
          i++;
          continue;
        }
        /* \t */
        else if(tolower(str->str[i+1]) == 't') {
          pushChar(&tmp, '0');
          pushChar(&tmp, '0');
          pushChar(&tmp, '9');
          i++;
          continue;
        }
        /* \\ */
        else if(str->str[i+1] == '\\') {
          pushChar(&tmp, '0');
          pushChar(&tmp, '9');
          pushChar(&tmp, '2');
          i++;
          continue;
        } else {
          destroyString(&tmp);
          return false;
        }
      } else {
        destroyString(&tmp);
        return false;
      }

    } else if(str->str[i] == ' ') {
      pushChar(&tmp, '\\');
      pushChar(&tmp, '0');
      pushChar(&tmp, '3');
      pushChar(&tmp, '2');

    } else pushChar(&tmp, str->str[i]);

  }

  clearStringStr(&str);
  for(int i = 0; i < tmp->actSize; i++) pushChar(&str, tmp->str[i]);
  destroyString(&tmp);

  return true;
}

bool baseToDec(cString str, int base, int* value) {
  int i = 2;
  int num = 0;
  char c = tolower(str->str[i]);

  while(c != '\0') {
    num *= base;

    if(c >= '0' && c <= '9') num += (c - '0');
    else if(c >= 'a' && c <= 'f' && base == 16) num += (c - 'a') + 10;
    else return false;

    i++;
    c = tolower(str->str[i]);
  }

  *value = num;
  return true;
}

void decToStr(cString str, int num) {
  cString tmp;
  char c;

  createString(&tmp);

  if(num == 0) pushChar(&tmp, '0');

  while(num != 0) {
    c = (num % 10) + '0';
    num /= 10;

    pushChar(&tmp, c);
  }

  clearStringStr(&str);
  for(int i = tmp->actSize - 1; i >= 0; i--) pushChar(&str, tmp->str[i]);
  destroyString(&tmp);

}

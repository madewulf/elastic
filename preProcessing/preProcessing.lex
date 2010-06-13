figure [0-9]
letter [a-z]|[A-Z]
id {letter}+({letter}|{figure}|"_")*
integer {figure}+
delim [ \t\n]

%{
#include <string.h>
#include "preProcessing.tab.h"
#define ppwrap() 1 /* GNU flex */
  YYSTYPE pplval;
  /* #define debug affiche les lexèmes scannés */
%}

%option yylineno
/* enclenche le comptage automatique du nombre de lignes scannées */

%s define
/* mode "define" : contrairement au mode par défaut, ce mode permet de distinguer
   certains symboles de ponctuation */

%%

<define>{delim} { }

<define>"(" {
#ifdef debug
  printf("(\n");
#endif
  return pptext[0];
}

<define>"," {
#ifdef debug
  printf(",\n");
#endif
  return pptext[0];
}

<define>"/" {
#ifdef debug
  printf("/\n");
#endif
  return pptext[0];
}

<define>")" {
#ifdef debug
  printf(")\n");
#endif
  BEGIN(INITIAL);
  return pptext[0];
}

define {
#ifdef debug
  printf("define\n");
#endif
  BEGIN(define);
  return DEFINE_;
}

{id} {
#ifdef debug
  printf("*id* : %s\n", pptext);
#endif
  pplval.text = (char *) malloc(strlen(pptext) + 1);
  strcpy(pplval.text, pptext);
  return id_;
}

{integer} {
#ifdef debug
  printf("*integer* : %i\n", atoi(pptext));
#endif
  pplval.text = (char *) malloc(strlen(pptext) + 1);
  strcpy(pplval.text, pptext);
  return integer_;
}

(.|\n) {
#ifdef debug
  printf("%s\n", pptext);
#endif
  pplval.text = (char *) malloc(strlen(pptext) + 1);
  strcpy(pplval.text, pptext);
  return ponctuation_;
}

%%


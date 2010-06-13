figure [0-9]
letter [a-z]|[A-Z]
id {letter}+({letter}|{figure}|_)*
integer {figure}+
delim [ \t\n]


%{
#include <string.h>
#include "parser.tab.h"
  int nvar=0;
  int yywrap(void)
  {
    BEGIN(INITIAL);
   
    return 1;
  }
  YYSTYPE yylval;
  /* #define debug affiche les lexèmes scannés */
  /* #define debug2 permet l'utilisation seule du scanner */
%}

%option yylineno
/* enclenche le comptage automatique du nombre de lignes scannées */
%option stack
/* permet de gérer les start conditions à l'aide d'un stack */

%x comment
/* mode "commentaire" : tout le texte compris entre "--" et "\n" est ignoré */
%x bigComment
/* mode "commentaire long" : tout le texte compris entre "-*" et "*-" est ignoré */
%x code
/* mode "code" :
 * tout le texte compris entre '#' et '#' est renvoyé dans un seul lexème */

%%

<code>[^#]* {
  /* << lexèmes du mode "code" >>
   * Ce mode est enclenché quand '#' est scanné */
#ifdef debug
  printf("*code Lego* : %s\n", yytext);
#endif
  yylval.text = (char *) malloc(strlen(yytext)+1);
  strcpy(yylval.text, yytext);
  return codeLego_;
}

<code>"#" {
#ifdef debug
  printf("#\n");
#endif
  BEGIN(INITIAL);
  return yytext[0];
}


<comment>[^\n]* {
#ifdef debug
  printf("*comment*\n");
#endif
  /* << lexèmes du mode "commentaire" >>
   * Ce mode est enclenché quand "--" est scanné */

  /* on passe les commentaires */
}

<comment>\n {
  yy_pop_state();
  /* restaure le mode précédemment utilisé */
}

<bigComment>[^*]* {
#ifdef debug
  printf("*big comment*\n");
#endif
  /* << lexèmes du mode "gros commentaire" >>
   * Ce mode est enclenché quand "-*" est scanné */

  /* on passe les commentaires */
}

<bigComment>"*-" {
  yy_pop_state();
  /* restaure le mode précédemment utilisé */
}

<bigComment>"*" {
  /* on passe les commentaires */
}

var {
#ifdef debug
  printf("var\n");
#endif
    return VAR_;
}

clock {
#ifdef debug
  printf("clock\n");
#endif
  return CLOCK_;
}

end {
#ifdef debug
  printf("end\n");
#endif
  BEGIN(INITIAL);
  return END_;
}

initially {
#ifdef debug
  printf("initially\n");
#endif
  return INITIALLY_;
}

loc {
#ifdef debug
  printf("loc\n");
#endif
  return LOC_;
}

when {
#ifdef debug
  printf("when\n");
#endif
  return WHEN_;
}

goto {
#ifdef debug
  printf("goto\n");
#endif
  return GOTO_;
}

True {
#ifdef debug
  printf("True\n");
#endif
  return TRUE_;
}

do {
#ifdef debug
  printf("do\n");
#endif
  return DO_;
}

"elastic automaton" {
  /* lexèmes propres aux automates élastiques */
#ifdef debug
  printf("elastic automaton\n");
#endif
  return ELASTIC_AUTOMATON_;
}

get {
#ifdef debug
  printf("get\n");
#endif
  return GET_;
}

put {
#ifdef debug
  printf("put\n");
#endif
  return PUT_;
}

orderlabs {
#ifdef debug
  printf("orderlabs\n");
#endif
  return ORDERLABS_;
}

eventlabs {
#ifdef debug
  printf("eventlabs\n");
#endif
  return EVENTLABS_;
}

internlabs {
#ifdef debug
  printf("internlabs\n");
#endif
  return INTERNLABS_;
}

automaton {
  /* lexèmes propres aux automates hybrides */
#ifdef debug
  printf("automaton\n");
#endif
  return AUTOMATON_;
}  

integrator {
#ifdef debug
  printf("integrator\n");
#endif
  return INTEGRATOR_;
}

stopwatch {
#ifdef debug
  printf("stopwatch\n");
#endif
  return STOPWATCH_;
}

analog {
#ifdef debug
  printf("analog\n");
#endif
  return ANALOG_;
}

parameter {
#ifdef debug
  printf("parameter\n");
#endif
  return PARAMETER_;
}

discrete {
#ifdef debug
  printf("discrete\n");
#endif
  return DISCRETE_;
}

synclabs {
#ifdef debug
  printf("synclabs\n");
#endif
  return SYNCLABS_;
}

while {
#ifdef debug
  printf("while\n");
#endif
  return WHILE_;
}

wait {
#ifdef debug
  printf("wait\n");
#endif
  return WAIT_;
}

in {
#ifdef debug
  printf("in\n");
#endif
  return IN_;
}

sync {
#ifdef debug
  printf("sync\n");
#endif
  return SYNC_;
}

False {
#ifdef debug
  printf("False\n");
#endif
  return FALSE_;
}

asap {
#ifdef debug
  printf("asap\n");
#endif
  return ASAP_;
}

\< {
#ifdef debug
  /* lexèmes "génériques" et de "ponctuation" */
  printf("<\n");
#endif
  return LES_;  
}

\<= {
#ifdef debug
  printf("<=\n");
#endif
  return LEQ_;  
}

= {
#ifdef debug
  printf("=\n");
#endif
  return EQU_;  
}

>= {
#ifdef debug
  printf(">=\n");
#endif
  return GEQ_;  
}

> {
#ifdef debug
  printf(">\n");
#endif
  return GRT_;  
}

"#" { 
#ifdef debug
  printf("{\n");
#endif
  BEGIN(code);
  return yytext[0];
}

":=" {
#ifdef debug
  printf(":=\n");
#endif
  return ASSIGN_;  
}

"param" {
#ifdef debug
  printf("param\n");
#endif
  return PARAM_;  
}

"init" {
#ifdef debug
  printf("init\n");
#endif
  return INIT_;  
}

"bad" {
#ifdef debug
  printf("bad\n");
#endif
  return BAD_;  
}


"view" {
#ifdef debug
  printf("view\n");
#endif
  return VIEW_;  
}


[\|,:;&(){}\[\]/+\'-] {
#ifdef debug
  printf("%s\n", yytext);
#endif
  return yytext[0];
}

{id} {
#ifdef debug
  printf("*id* : %s\n", yytext);
#endif
  yylval.text = (char *) malloc(strlen(yytext) + 1);
  strcpy(yylval.text, yytext);
  return id_;
}

{integer} {
#ifdef debug
  printf("*integer* : %i\n", atoi(yytext));
#endif
  yylval.intgr = atoi(yytext);
  return positive_integer_;
}

-- { 
  yy_push_state(comment);
  /* passe en mode "comment" */
}

"-*" { 
  yy_push_state(bigComment);
  /* passe en mode "bigComment" */
}

{delim} { }

[\0] {
  return yytext[0];
}

. {
  /* recupère tous les caractères non-utilisables */
#ifdef debug
  printf("%s\n", yytext);
#endif
  fprintf(stderr, "- Error: Symbole '%c' is not allowed.\n", yytext[0]);
 return yytext[0];
}

%%

#ifdef debug2

#include <stdio.h>


int main(int argc, char **argv)
{
  ++argv, --argc; /* passe le nom du programme */
  if (argc > 0) {
    yyin = fopen(argv[0], "r");
  }else{
    yyin = stdin;
  }
  while (yylex() != 0)
    {
    }
  return(1);
}


#endif

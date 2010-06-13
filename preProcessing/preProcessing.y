%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "preProcessing/preProcessing.h"
#include "preProcessing/tableOfConstants.h"
#define YYMAXDEPTH 1000000  
#define YYERROR_VERBOSE 
  int pperror(char *s);
  int pplex ();
  extern int pplineno;
  FILE * ppin;
  int outputFileId;
  TableOfConstants TableOfConst = NULL;
%}

%union {
  char* text;
}

%type <text> constant id_ integer_ ponctuation_

/* REM : tous les lexèmes sont suffixés par '_' pour éviter que leur nom soit identique à celui
   d'éventuelles constantes déclarées dans les fichiers liés par 'include' */
%token DEFINE_ id_ integer_ ponctuation_

%%

input :
lexeme_list
{
  close(outputFileId);
  deleteTableOfConstants(TableOfConst);
}
;

lexeme_list :
lexeme lexeme_list
{}
|
{}
;

lexeme :
define
{}
|
integer_
{
  writeNewInput($1);
}
|
id_
{
  string associetedConst = NULL;
    associetedConst = findConstant(TableOfConst, $1);
  if (associetedConst != NULL){
    writeNewInput(associetedConst);
    free(associetedConst);
  }else{
    writeNewInput($1);
  }
}
|
ponctuation_
{
  writeNewInput($1);
}
;

define :
DEFINE_ '(' id_ ',' constant ')'
{
  TableOfConst = insertConstant(TableOfConst, $3, $5);
}
;

constant :
integer_
{
  $$ = $1;
}
|
integer_ '/' integer_
{
  $$ = (char *)malloc(strlen($1) + strlen($3) + 2);
  strcpy($$, $1);
  strcat($$, "/");
  strcat($$, $3);
}
;

%%

#include <stdio.h>

int pperror (char *s){
  /* fonction invoquée quand une erreur liée au parsing du pré-processing se produit.
     <s> contient le message d'erreur associé à l'erreur de parsing survenue */
  fprintf(stderr, "- Erreur (ligne %i) : %s.\n", pplineno, s);
  close(outputFileId);
  deleteTableOfConstants(TableOfConst);
  return -1;
}

void setInputPreProc(FILE * idFile){
/* spécifie que l'input du pré-processing est situé dans le fichier
   de descripteur <idFile> */

  /* vérification des paramètres */
  if (idFile == NULL){
    fprintf(stderr, "function setInputPreProc error ! The parameter 'idFIle' can't be NULL.\n");
    exit(-1);
  }
  /* fin vérification des paramètres */
  ppin = idFile;
}

void setOutputPreProc(string tempFileName){
  /* précise que l'output du pré-processing sera écrit dans le fichier
     nommé <tempFileName> */

  /* vérification des paramètres */
  if (tempFileName == NULL){
    fprintf(stderr, "function setOutputPreProc error ! The parameter 'tempFileName' can't be NULL.\n");
    exit(-1);
  }
  /* fin vérification des paramètres */
  outputFileId = open("tmpInput", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
  if (outputFileId == -1){
    fprintf(stderr, "Erreur durant l'ouverture du fichier temporaire pour pre-processing.\n");
    exit(-1);
  }
}

void writeNewInput(string text){
  /* écrit le texte <text> dans le fichier d'output du pre-processing */
  int writeResult;
  /* vérification des paramètres */
  if (text == NULL){
    fprintf(stderr, "function writeNewInput error ! The parameter 'text' can't be NULL\n");
    exit(-1);
  }
  /* fin vérification des paramètres */
  writeResult = write(outputFileId, text, strlen(text));
  if (writeResult <= 0){
    fprintf(stderr, "function writeNewInput error ! error while writing output file\n");
    exit(-1);      
  }
}


/*

REMARQUES
*********

1. noms affectés par changement prefixe scanner

yy_create_buffer
yy_delete_buffer
yy_flex_debug
yy_init_buffer
yy_flush_buffer
yy_load_buffer_state
yy_switch_to_buffer
yyin
yyleng
yylex
yylineno
yyout
yyrestart
yytext
yywrap

2. noms affectés par changement prefixe parser

yyparse
yylex
yyerror
yylval
yychar
yydebug

*/

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

/* REM : tous les lex�mes sont suffix�s par '_' pour �viter que leur nom soit identique � celui
   d'�ventuelles constantes d�clar�es dans les fichiers li�s par 'include' */
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
  /* fonction invoqu�e quand une erreur li�e au parsing du pr�-processing se produit.
     <s> contient le message d'erreur associ� � l'erreur de parsing survenue */
  fprintf(stderr, "- Erreur (ligne %i) : %s.\n", pplineno, s);
  close(outputFileId);
  deleteTableOfConstants(TableOfConst);
  return -1;
}

void setInputPreProc(FILE * idFile){
/* sp�cifie que l'input du pr�-processing est situ� dans le fichier
   de descripteur <idFile> */

  /* v�rification des param�tres */
  if (idFile == NULL){
    fprintf(stderr, "function setInputPreProc error ! The parameter 'idFIle' can't be NULL.\n");
    exit(-1);
  }
  /* fin v�rification des param�tres */
  ppin = idFile;
}

void setOutputPreProc(string tempFileName){
  /* pr�cise que l'output du pr�-processing sera �crit dans le fichier
     nomm� <tempFileName> */

  /* v�rification des param�tres */
  if (tempFileName == NULL){
    fprintf(stderr, "function setOutputPreProc error ! The parameter 'tempFileName' can't be NULL.\n");
    exit(-1);
  }
  /* fin v�rification des param�tres */
  outputFileId = open("tmpInput", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
  if (outputFileId == -1){
    fprintf(stderr, "Erreur durant l'ouverture du fichier temporaire pour pre-processing.\n");
    exit(-1);
  }
}

void writeNewInput(string text){
  /* �crit le texte <text> dans le fichier d'output du pre-processing */
  int writeResult;
  /* v�rification des param�tres */
  if (text == NULL){
    fprintf(stderr, "function writeNewInput error ! The parameter 'text' can't be NULL\n");
    exit(-1);
  }
  /* fin v�rification des param�tres */
  writeResult = write(outputFileId, text, strlen(text));
  if (writeResult <= 0){
    fprintf(stderr, "function writeNewInput error ! error while writing output file\n");
    exit(-1);      
  }
}


/*

REMARQUES
*********

1. noms affect�s par changement prefixe scanner

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

2. noms affect�s par changement prefixe parser

yyparse
yylex
yyerror
yylval
yychar
yydebug

*/

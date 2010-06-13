%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include "parser/parser.h"
#include "utils/utils.h"
#include "avl/avl.h"
#include "enums.h"
#include "constraints/constraints.h"
#include "automata/automata.h"  
#include "system/system.h"
#define YYERROR_VERBOSE 
#define YYMAXDEPTH 1000000  
  int yyerror(char *s);
  int yylex ();
 
  extern int yylineno;
  FILE * yyin;

  int flagASAP=0;
  struct exprNode * exprTmp3=NULL;
  struct exprNode * exprTmp4=NULL;
  int idLabTmp=-1;
  int idLabTmp2=-1;
  struct system * syst;

  struct automaton * autom=NULL;

 
  struct stringElem * idLabList=NULL;/* already declared labels*/
  struct stringElem * idVarList= NULL; /*already declared variables*/
  struct stringElem * idAutomList=NULL; /*already declared automaton*/
  struct stringElem * idLocList=NULL; /*already declared locations*/
  
  struct stringElem * se=NULL;
  struct stringElem * destList=NULL;
   
  enum labelType labTy=STANDARD;
%}

%union {
  char * text;
  struct exprNode *   expression;
  int intgr;
  struct location * loc;
  struct lab * label;
  struct rational * signedRational;
  struct stringElem * stringList;
  struct transition * trans;
};


/*
conventions typographiques utilisées
************************************

- Tous les noms de lexèmes se terminent par le symbole '_' (ce pour éviter que leur nom soit
  identique à celui d'éventuelles constantes déclarées dans les fichiers liés par 'include').
- Les noms de lexèmes "mots réservés" sont écrits en majuscules.
- Les noms de lexèmes "génériques" sont écrits en minuscules.
- Les noms de règles sont écrits en minuscules. S'ils sont composés de plusieurs mots,
  ceux sont séparés par le symbole '_' .
*/


/* règles utilisées la déclaration de variables et d'automates */
%type <text> id_ 
%type <intgr> positive_integer_ 

/* règles utilisées la définition d'automates élastic */
%type <text>  code_lego codeLego_ 
%type <intgr> op  var_type elastic_event_synchro elastic_order_synchro

%type <signedRational> pos_rational
%type <stringList> var_list
%type <loc> elastic_location elastic_locations
%type <trans> elastic_transition elastic_transitions
%type <label> elastic_sync_labels elastic_sync_var_nonempty_list elastic_sync_var_list 
%type <expression> elastic_rectangular_constraint elastic_rectangular_predicate 
%type <expression> pos_rational_addition elastic_update_var elastic_update_list 
%type <expression> elastic_update_nonempty_list  updates update_list
%type <expression> init_expr bad_var bad_loc bad_expr

/* règles utilisées la définition d'automates hybrides */
%type <label> sync_var_noempty_list sync_var_list sync_labels
%type <loc> location locations
%type <trans> transition transitions
%type <expression> state_initialization
%type <expression> update_linear_term rate_info_list linear_expression
%type <expression> linear_term convex_predicate linear_constraint
%type <expression> update_linear_expression update update_nonempty_list
%type <intgr> relop rate_relop syn_label
%type <signedRational> rational integer


/* les lexèmes */
%token SYSTEM_ DEFINE_ VAR_ INTEGRATOR_ STOPWATCH_ CLOCK_ ANALOG_ PARAMETER_ DISCRETE_ ELASTIC_AUTOMATON_ END_ INITIALLY_ EVENTLABS_ INTERNLABS_ ORDERLABS_ LOC_ WHEN_ GOTO_ GET_ TRUE_ PUT_ DO_ AUTOMATON_ SYNCLABS_ WHILE_ WAIT_ IN_ SYNC_ FALSE_ ASAP_ LES_ LEQ_ EQU_ GEQ_ GRT_  DELTA_  codeLego_ id_ positive_integer_ ASSIGN_ PARAM_ BAD_ INIT_  VIEW_

%%

parse_tree :
automata_descriptions system tools_commands
{
  fclose(yyin);
}
;

system :
SYSTEM_ automaton_list ';'
{;}
|
{}
;

automaton_list :
id_ ',' automaton_list
{

}
|
id_
{
  
}
;

tools_commands :
init bad couple
|
init bad
|
init couple
|
bad couple
|
init
|
bad
|
couple
|

;

init :
INIT_ ASSIGN_ init_expr ';'
{  syst->paramInit=$3;}
|
INIT_ ASSIGN_ ';'
;

init_expr :
PARAM_ '[' id_ ']' EQU_ rational 
{
  int tmpId;
  
  tmpId=getId($3,idAutomList);
  if (tmpId==-1)
    yyerror("Error: Automaton not declared. \n"); 
  $$=makeExpr(EQU,
	      makeExpr(PARAM,
		       NULL,
		       NULL,
		       makeTerm(tmpId,makeRational(POSITIVE,1,1),NORMAL)),
	      makeExpr(TERM,
		       NULL,
		       NULL,
		       makeTerm(-1,$6,CONSTANT)),
	      NULL)  ; 
	      
}
|
PARAM_ '[' id_ ']' EQU_ rational '&' init_expr
{
  int tmpId;
  
  tmpId=getId($3,idAutomList);
  if (tmpId==-1)
    yyerror("Error: Automaton not declared. \n"); 
  $$=makeExpr(AND,
	      makeExpr(EQU,
		       makeExpr(PARAM,
				NULL,
				NULL,
				makeTerm(tmpId,NULL,NORMAL)),
		       makeExpr(TERM,
				NULL,
				NULL,
				makeTerm(-1,$6,CONSTANT)),
		       NULL),
	      $8,
	      NULL);
}
;

bad : 
BAD_ ASSIGN_ bad_expr ';'
{
  syst->badStates=$3;
}
|
BAD_ ASSIGN_ ';'
;

bad_expr :
bad_loc
{$$=$1;}
|
bad_var
{$$=$1;}
|
bad_loc'&' bad_expr
{
  $$=makeExpr(AND,$1,$3,NULL);
}
|
bad_loc'|' bad_expr
{  
  $$=makeExpr(OR,$1,$3,NULL);
}
|
bad_var '&' bad_expr 
{
  $$=makeExpr(AND,$1,$3,NULL);
}
|
bad_var '|' bad_expr
{
  $$=makeExpr(OR,$1,$3,NULL);
}
|
'(' bad_expr ')' 
{
  $$=$2;
}
;

bad_loc :
LOC_ '[' id_ ']' EQU_ id_ 
{
  int tmpId;
  int tmpId2;
  
  tmpId=getId($3,idAutomList);
  if (tmpId==-1)
    yyerror("Error: Automaton not declared. \n");
  tmpId2=getId($6,idLocList);
  if (tmpId2==-1)
    yyerror("Error: Target location not declared. \n");
  $$=makeExpr(LOCINIT,NULL,NULL,makeTerm(tmpId,NULL,tmpId2));
 
}
;
bad_var :
id_ EQU_ rational
{
  int tmpId;
  
  tmpId=getId($1,idVarList);
  if (tmpId==-1)
    yyerror("Error: Variable not declared. \n");
  $$=makeExpr(EQU,
	      makeExpr(TERM,
		       NULL,
		       NULL,
		       makeTerm(tmpId,makeRational(POSITIVE,1,1),NORMAL)
		       ),
	      makeExpr(TERM,
		       NULL,
		       NULL,
		       makeTerm(-1,$3,NORMAL)
		       ),
	      NULL);
}
;

couple : 
 couple_expr ';'
;

couple_expr : 
VIEW_ '[' id_ ']' EQU_ id_ 
{
  int tmpId;
  int tmpId2;
 
  tmpId=getId($3,idLabList);
  if (tmpId==-1)
    yyerror("Error: Label not declared. \n");
  tmpId2=getId($6,idLabList);
  if (tmpId==-1)
    yyerror("Error: Label not declared. \n");
  syst->viewList=makeLabStatus(tmpId,tmpId2,syst->viewList);
}
|
VIEW_ '[' id_ ']' EQU_ id_ '&' couple_expr 
{
  int tmpId;
  int tmpId2;
  tmpId=getId($3,idLabList);
  if (tmpId==-1)
    yyerror("Error: Label not declared. \n");
  tmpId2=getId($6,idLabList);
  if (tmpId==-1)
    yyerror("Error: Label not declared. \n");
  syst->viewList=makeLabStatus(tmpId,tmpId2,syst->viewList);
} 
;

automata_descriptions : 
declarations automata
{

}
;

declarations :
VAR_ var_lists
{

}
;

var_lists :
var_list ':' var_type ';' var_lists
{
  idVarList=appendStringList(idVarList,$1);
  for(se=$1;se!=NULL;se=se->next)
    {
      /*d'abord, insertion de la clef dans le dictionnaire*/
      AvlTree tmp=makeAvlNode(se->id,se->s,VAR);
      tmp->data.vType=$3;
      syst->idDic=Insert(tmp, syst->idDic);
      /*insertion de la variable dans la liste des variables du système*/
      syst->varList=makeVar(se->id,syst->varList);
    }
}
|
':' var_type ';' var_lists
{
  
}
|
{

} 
;

var_list :
id_ ',' var_list
{
  $$=insertStringElem($1,getFreshId(),$3);
}
|
id_
{
  $$=insertStringElem($1,getFreshId(),NULL);
}
;

var_type :
INTEGRATOR_
{
  $$=INTEGRATOR;
}
|
STOPWATCH_
{
  $$=STOPWATCH;
}
|
CLOCK_
{
  $$=CLOCK;
}
|
ANALOG_
{
  $$=ANALOG;
}
|
PARAMETER_
{
  $$=PARAMETER;
}
|
DISCRETE_ 
{
  $$=DISCRETE;
}
;

automata :
automaton automata
{

}
|
automaton
{

}
;

automaton :
ELASTIC_AUTOMATON_ id_ elastic_sync_labels initialization elastic_locations END_
{
  int tmpId;
  idAutomList=insertStringElem($2,getFreshId(),idAutomList);
  tmpId=getId($2,idAutomList);
  syst->automList=makeAutomaton(tmpId,ELASTIC,exprTmp4,idLabTmp2,$3,$5,syst->automList);
  syst->idDic=Insert(makeAvlNode(tmpId,$2,AUTOMATON),syst->idDic);
  /*printStringList(idList);*/
  while (destList!=NULL)
    {
      if (getId(destList->s,idLocList)==-1)
	yyerror("One of the transition destinations does not exist!\n");
      destList=destList->next;
    }
}
|
AUTOMATON_ id_ sync_labels initialization locations END_
{ int tmpId;
  idAutomList=insertStringElem($2,getFreshId(),idAutomList);
  tmpId=getId($2,idAutomList); 
  syst->automList=makeAutomaton(tmpId,HYTECH,exprTmp4,idLabTmp2,$3,$5,syst->automList);
  syst->idDic=Insert(makeAvlNode(tmpId,$2,AUTOMATON),syst->idDic);
  /*printStringList(idList);*/
  while (destList!=NULL)
    {
      if (getId(destList->s,idLocList)==-1)
	yyerror("One of the transition destinations does not exist!\n");
      destList=destList->next;
    }
}
;

/* elastic_initialization : */
/* INITIALLY_ id_ ';' */
/* { */
/*   int tmpId; */
/*   tmpId=getId($2,idLocList); */
  
/*   if (tmpId==-1)/*la location n'a pas encore été déclarée */
/*     { */
/*       destList=insertStringElem($2,getFreshId(),destList); */
/*       tmpId=getId($2,destList); /*on vérifiera + tard si la location existe vraiment */
/*     }   */
/*   $$=tmpId; */
/* } */
/* ; */

elastic_sync_labels :
EVENTLABS_ ':' {labTy=EVENT;  } elastic_sync_var_list ';'
INTERNLABS_ ':' {labTy=INTERNAL; } elastic_sync_var_list ';'
ORDERLABS_ ':' {labTy=ORDER;  } elastic_sync_var_list ';'
{
  $$=appendLabList($4,appendLabList($9,$14));
}
;

elastic_sync_var_list :
elastic_sync_var_nonempty_list
{
  $$=$1;
}
|
{
  $$=NULL;
}
;

elastic_sync_var_nonempty_list :
id_ ',' elastic_sync_var_nonempty_list
{
  int tmpId;
  struct lab * labTmp;
  AvlTree tmp;
  tmpId=getId($1,idLabList);

  if (tmpId==-1)
    {
      idLabList=insertStringElem($1,getFreshId(),idLabList);
      tmpId= getId($1,idLabList);
      labTmp=makeLabel(tmpId,labTy,$3);
      tmp=makeAvlNode(tmpId,$1,LABEL);
      tmp->data.labPtr=labTmp;
      syst->idDic=Insert(tmp, syst->idDic);
    }
  else
    labTmp=makeLabel(tmpId,labTy,$3);
  $$=labTmp;
}
|
id_
{
int tmpId;
  struct lab * labTmp;
  AvlTree tmp;
  tmpId=getId($1,idLabList);

  if (tmpId==-1)
    {
      idLabList=insertStringElem($1,getFreshId(),idLabList);
      tmpId= getId($1,idLabList);
      labTmp=makeLabel(tmpId,labTy,NULL);
      tmp=makeAvlNode(tmpId,$1,LABEL);
      tmp->data.labPtr=labTmp;
      syst->idDic=Insert(tmp, syst->idDic);
    }
  else
    labTmp=makeLabel(tmpId,labTy,NULL);
  $$=labTmp;
}
;

elastic_locations :
elastic_location elastic_locations
{
  $1->next=$2;
  $$=$1;
}
|
elastic_location
{
  $$=$1;
}
;

elastic_location :
LOC_ id_ ':' elastic_transitions
{
  int tmpId;
  AvlTree tmpavl;
  tmpId=getId($2,destList);
  
  if (tmpId==-1)/*la location n'a pas encore été déclarée comme une destination*/
    {
      idLocList=insertStringElem($2,getFreshId(),idLocList);
      tmpId=getId($2,idLocList); 
    }
  else
    idLocList=insertStringElem($2,tmpId,idLocList); /* déjà dans la liste  des destinations mais nulle part ailleurs*/

  tmpavl= makeAvlNode(tmpId,$2,LOCATION);
  tmpavl->data.locPtr=makeLocation(tmpId,NULL,NULL,$4,NULL);
  syst->idDic=Insert(tmpavl,syst->idDic);
    
  $$=tmpavl->data.locPtr;
}
;

elastic_transitions :
elastic_transition elastic_transitions
{
  $1->next=$2;
  $$=$1;
}
|
{
  $$=NULL;
}
;

elastic_transition :

WHEN_ elastic_rectangular_predicate elastic_update_var GOTO_ id_ ';'
{
  int tmpId;
  tmpId=getId($5,idLocList);
  
  if (tmpId==-1) /*la location n'a pas encore été déclarée*/
    { 
      tmpId=getId($5,destList);
      if (tmpId==-1)
	{
	  destList=insertStringElem($5,getFreshId(),destList);
	  tmpId=getId($5,destList); /*on vérifiera + tard si la location existe vraiment*/
	}
    }

  $$=makeTransition(tmpId,-1,ORDINARY,$2,$3,NULL);
}
|
WHEN_ elastic_event_synchro '&' elastic_rectangular_predicate elastic_update_var GOTO_ id_ ';'
{
 int tmpId;
  tmpId=getId($7,idLocList);
  
  if (tmpId==-1)/*la location n'a pas encore été déclarée*/
    {
      tmpId=getId($7,destList);
      if (tmpId==-1) /*la location n'a pas encore été employée comme destination*/
	{
	  destList=insertStringElem($7,getFreshId(),destList);
	  tmpId=getId($7,destList); /*on vérifiera + tard si la location existe vraiment*/
    
	}
    }
  $$=makeTransition(tmpId,$2,ORDINARY,$4,$5,NULL);
}
|
WHEN_ elastic_event_synchro elastic_update_var GOTO_ id_ ';'
{
  int tmpId;
  tmpId=getId($5,idLocList);
  
  if (tmpId==-1)/*la location n'a pas encore été déclarée*/
    {
      tmpId=getId($5,destList);
      if (tmpId==-1)
	{
	  destList=insertStringElem($5,getFreshId(),destList);
	  tmpId=getId($5,destList); /*on vérifiera + tard si la location existe vraiment*/
	}
    }
  $$=makeTransition(tmpId,$2,ORDINARY,NULL,$3,NULL);
}
|
WHEN_ elastic_rectangular_predicate elastic_order_synchro elastic_update_var GOTO_ id_ ';'
{
 int tmpId;
  tmpId=getId($6,idLocList);
  
  if (tmpId==-1)/*la location n'a pas encore été déclarée*/
    {
      tmpId=getId($6,destList);
      if (tmpId==-1)
	{
	  destList=insertStringElem($6,getFreshId(),destList);
	  tmpId=getId($6,destList); /*on vérifiera + tard si la location existe vraiment*/
	}
    }
  $$=makeTransition(tmpId,$3,ORDINARY,$2,$4,NULL);
}
;

elastic_event_synchro :
GET_ id_ code_lego
{
  int tmpId;
  tmpId=getId($2,idLabList);
  
  if (tmpId==-1)
    yyerror("Use of a non declared label. \n");
  $$=tmpId;
}
;

code_lego :
'#' codeLego_ '#'
{

}
|
{

}
;

elastic_rectangular_predicate :
elastic_rectangular_constraint '&' elastic_rectangular_predicate
{
  $$=makeExpr(AND,$1,$3,NULL);
}
|
elastic_rectangular_constraint
{
  $$=$1;
}
;

elastic_rectangular_constraint :
id_ op pos_rational_addition
{
  int tmpId;
  AvlTree tmp;
  tmpId=getId($1,idVarList);
  if (tmpId==-1)
    yyerror("Use of a non declared variable. \n");
  tmp = Find(tmpId,syst->idDic);
  if (tmp->data.vType!=DISCRETE && $2==EQU)
    yyerror("Equality test is only allowed on discrete variable in Elastic. \n");
  
  $$=makeExpr($2,
	      makeExpr(TERM,
		       NULL,
		       NULL,
		       makeTerm(tmpId,makeRational(POSITIVE,1,1),NORMAL)
		       ),
	      $3,
	      NULL);
}
|
pos_rational_addition op id_
{
  int tmpId;
  int op;
  tmpId=getId($3,idVarList);
  
  if (tmpId==-1)
    yyerror("Use of a non declared variable. \n");
  switch($2)
    {
    case GEQ :
      op=LEQ;
      break;
    case LEQ : 
      op=GEQ;
      break;
    default :
      break;
    }
  /*the elastic expression are always parsed in the same sense : id on the left, linear expression on the right*/
  $$=makeExpr(op,makeExpr(TERM,NULL,NULL,makeTerm(tmpId,makeRational(POSITIVE,1,1),NORMAL)),$1,NULL);
}
|
TRUE_
{
  $$=makeExpr(TRUE,NULL,NULL,NULL);
}
;

op :
EQU_
{
  $$=EQU;
}
|
LEQ_
{
  $$=LEQ;
}
|
GEQ_
{
  $$=GEQ;
}
;

pos_rational_addition :
pos_rational_addition '+' pos_rational
{
  $$=makeExpr(PLUS,$1,makeExpr(TERM,NULL,NULL,makeTerm(-1,$3,CONSTANT)),NULL);
}
|
pos_rational
{
  $$=makeExpr(TERM,NULL,NULL,makeTerm(-1,$1,CONSTANT));
}
;

pos_rational :
positive_integer_
{
 $$=makeRational(POSITIVE, $1,1);
}
|
positive_integer_ '/' positive_integer_
{
  $$=makeRational(POSITIVE, $1,$3);
  /*printf("numérateur : %i , dénominateur : %i \n",$1,$3);*/
}
;

elastic_order_synchro :
PUT_ id_ code_lego
{
  int tmpId;
  tmpId=getId($2,idLabList);
  
  if (tmpId==-1)
    yyerror("Use of a non declared label. \n");
  $$=tmpId;
}
;

elastic_update_var :
DO_ '{' elastic_update_list '}'
{
  $$=$3;
}
|
{
  $$=NULL;
}
;

elastic_update_list :
elastic_update_nonempty_list
{
  $$=$1;
}
|
{
  $$=NULL;
}
;

elastic_update_nonempty_list :
update ',' elastic_update_nonempty_list
{
  $$=makeExpr(AND,$1,$3,NULL);
}
|
update
{
  $$=$1;
}
;

/* elastic_update : */
/* id_ '\'' EQU_ positive_integer_ */
/* { */
/*   struct term * tmpTerm; */
/*   struct term * tmpTerm2; */
/*   int tmpId; */
/*   tmpId=getId($1,idVarList); */
  
/*   if (tmpId==-1) */
/*     yyerror("Use of a non declared variable. \n"); */

/*   tmpTerm=makeTerm(tmpId,makeRational(POSITIVE,1,1),POST); */
/*   tmpTerm2=makeTerm(-1,makeRational(POSITIVE,$4,1),CONSTANT); */
/*   $$=makeExpr(EQU,makeExpr(TERM,NULL,NULL,tmpTerm),makeExpr(TERM,NULL,NULL,tmpTerm2),NULL); */

/* } */
/* ; */

/* règles des automates hybrides */
/* Rem : règles extraites de la grammaire de Hytech */

initialization :
INITIALLY_ id_ state_initialization ';'
{
  int tmpId;
  tmpId=getId($2,idLocList);
  
  if (tmpId==-1)/*la location n'a pas encore été déclarée*/
    {
      destList=insertStringElem($2,getFreshId(),destList);
      tmpId=getId($2,destList); /*on vérifiera + tard si la location existe vraiment*/
    }  
  exprTmp4=$3;
  idLabTmp2=tmpId;
}
;

state_initialization :
'&' convex_predicate
{
  $$=$2;
}
|
{
  $$=NULL;
}
;

sync_labels :
SYNCLABS_ ':' sync_var_list ';'
{
  $$=$3;
}
;

sync_var_list :
sync_var_noempty_list
{
  $$=$1;
}
|
{
  $$=NULL;
}
;

sync_var_noempty_list :
id_ ',' sync_var_noempty_list
{
  int tmpId;
  struct lab * labTmp;
  AvlTree tmp;
  tmpId=getId($1,idLabList);
  
  if (tmpId==-1)
    {
      idLabList=insertStringElem($1,getFreshId(),idLabList);
      tmpId= getId($1,idLabList);
      labTmp=makeLabel(tmpId,STANDARD,$3);  
      tmp=makeAvlNode(tmpId,$1,LABEL);
      labTmp->type=STANDARD;
      tmp->data.labPtr=labTmp;
      syst->idDic=Insert(tmp, syst->idDic);
    }
  else
    labTmp=makeLabel(tmpId,STANDARD,$3);
  $$=labTmp;
}
|
id_
{
  int tmpId;
  struct lab * labTmp;
  AvlTree tmp;
  tmpId=getId($1,idLabList);
  
  if (tmpId==-1)
    {
      idLabList=insertStringElem($1,getFreshId(),idLabList);
      tmpId= getId($1,idLabList);
      labTmp=makeLabel(tmpId,STANDARD,NULL);  
      tmp=makeAvlNode(tmpId,$1,LABEL);
      labTmp->type=STANDARD;
      tmp->data.labPtr=labTmp;
      syst->idDic=Insert(tmp, syst->idDic);
    }
  else
    labTmp=makeLabel(tmpId,STANDARD,NULL);
  $$=labTmp;
}
;

locations :
location locations
{
  $1->next=$2;
  $$=$1;
}
|
location
{
  $$=$1;
}
;

location :
LOC_ id_ ':' WHILE_ convex_predicate WAIT_ '{' rate_info_list '}' transitions
{
  int tmpId;
  AvlTree tmpavl;
  tmpId=getId($2,destList);
  
  if (tmpId==-1)/*la location n'a pas encore été déclarée comme une destination*/
    {
      idLocList=insertStringElem($2,getFreshId(),idLocList);
      tmpId=getId($2,idLocList); 
    }
  else
    idLocList=insertStringElem($2,tmpId,idLocList); /* déjà dans la liste  des destinations mais nulle part ailleurs*/

  tmpavl= makeAvlNode(tmpId,$2,LOCATION);
  tmpavl->data.locPtr=makeLocation(tmpId,$5,$8,$10,NULL);
  syst->idDic=Insert(tmpavl,syst->idDic);
      
  $$=tmpavl->data.locPtr;
}
;

rate_info_list :
rate_info_nonempty_list
{
 yyerror("Can not translate rate conditions to uppaal");
 $$=NULL;
}
|
{
  $$=NULL;
}
;

rate_info_nonempty_list :
rate_info ',' rate_info_nonempty_list
{

}
|
rate_info
{

}
;

rate_info :
rate_linear_expression IN_ '[' rational ',' rational ']'
{
 
}
|
rate_linear_expression rate_relop rate_linear_expression
{
 
}
;

transitions :
transition transitions
{
  $1->next=$2;
  $$=$1;
}
|
{
  $$=NULL;
}
;

transition :
WHEN_ convex_predicate update_synchronisation GOTO_ id_ ';'
{
  int tmpId;
  int status=ORDINARY;
  tmpId=getId($5,idLocList);
  
  if (tmpId==-1) /*la location n'a pas encore été déclarée*/
    { 
      tmpId=getId($5,destList);
      if (tmpId==-1)
	{
	  destList=insertStringElem($5,getFreshId(),destList);
	  tmpId=getId($5,destList); /*on vérifiera + tard si la location existe vraiment*/
	}
    }
  if (flagASAP==1)
    {
      flagASAP=0;
      status=ASAP;
    }
      
  $$=makeTransition(tmpId,idLabTmp,status,$2,exprTmp3,NULL);
}
;

update_synchronisation :
updates syn_label
{
  exprTmp3=$1;
  idLabTmp=$2;
}
|
syn_label updates
{
  exprTmp3=$2;
  idLabTmp=$1;
}
|
updates
{
  exprTmp3=$1;
  idLabTmp=-1;
}
|
syn_label
{
  exprTmp3=NULL;
  idLabTmp=$1;
}
|
{
  exprTmp3=NULL;
  idLabTmp=-1;
}
;

updates :
DO_ '{' update_list '}'
{
  $$=$3;
}
;

update_list :
update_nonempty_list
{
  $$=$1;
}
|
{
  $$=NULL;
}
;

update_nonempty_list :
update ',' update_nonempty_list
{
  $$=makeExpr(AND,$1,$3,NULL);
}
|
update
{
  $$=$1;
}
;

update :
update_linear_expression relop update_linear_expression
{
  $$=makeExpr($2,$1,$3,NULL);
}
;

syn_label :
SYNC_ id_ 
{
 int tmpId;
  tmpId=getId($2,idLabList);
  
  if (tmpId==-1)
    yyerror("Use of a non declared label. \n");
  $$=tmpId;
}
;

/* rationals, linear terms, linear constraints and convex predicates */

convex_predicate :
linear_constraint '&' convex_predicate
{
  if ($1 && $3)
    $$=makeExpr(AND,$1,$3,NULL);
  else
    { 
      if ($1)
	$$=$1;
      else
	$$=($3);
    }
}
|
linear_constraint
{
    $$=$1;
}
;

linear_constraint :
linear_expression relop linear_expression
{
  $$=makeExpr($2,$1,$3,NULL);
}
|
TRUE_
{
  $$=makeExpr(TRUE,NULL,NULL,NULL);
}
|
FALSE_
{
  $$=makeExpr(FALSE,NULL,NULL,NULL);
}
|
ASAP_
{
  flagASAP=1;
  $$=NULL;
}
;

relop :
LES_
{
  $$=LES;
}
|
LEQ_
{
  $$=LEQ;
}
|
EQU_
{
  $$=EQU;
}
|
GEQ_
{
  $$=GEQ;
}
|
GRT_
{
  $$=GRT;
}
;

rate_relop :
LEQ_
{
  $$=LEQ;
}
|
EQU_
{
  $$=EQU;
}
|
GEQ_
{
  $$=GEQ;
}
;

linear_expression :
linear_expression '+' linear_term
{
 $$=makeExpr(PLUS,$1,$3,NULL);
}
|
linear_expression '-' linear_term
{
  $$=makeExpr(MINUS,$1,$3,NULL);
}
|
linear_term
{
  $$=$1;
}
;

linear_term :
rational
{
  $$=makeExpr(TERM,NULL,NULL,makeTerm(-1,$1,CONSTANT));
}
|
rational id_
{
  int tmpId;
  tmpId=getId($2,idVarList);
  if (tmpId==-1)
    {
      yyerror("Use of a non declared variable");
    }
  $$=makeExpr(TERM,NULL,NULL,makeTerm(tmpId,$1,NORMAL));
}
|
id_
{
  int tmpId;
  tmpId=getId($1,idVarList);
  if (tmpId==-1)
    {
      yyerror("Use of a non declared variable");
    }
  $$=makeExpr(TERM,NULL,NULL,makeTerm(tmpId,makeRational(POSITIVE,1,1),NORMAL));
}
;

update_linear_expression :
update_linear_expression '+' update_linear_term
{
  $$=makeExpr(PLUS,$1,$3,NULL);
}
|
update_linear_expression '-' update_linear_term
{
  $$=makeExpr(MINUS,$1,$3,NULL);
}
|
update_linear_term
{
  $$=$1;
}
;

update_linear_term :
rational
{
  $$=makeExpr(TERM,NULL,NULL,makeTerm(-1,$1,CONSTANT));
}
|
rational id_
{  
  int tmpId;
  tmpId=getId($2,idVarList);
  if (tmpId==-1)
    {
      yyerror("Use of a non declared variable");
    }
  $$=makeExpr(TERM,NULL,NULL,makeTerm(tmpId,$1,NORMAL));
}
|
rational id_ '\''
{
  int tmpId;
  tmpId=getId($2,idVarList);
  if (tmpId==-1)
    {
      yyerror("Use of a non declared variable");
    }
  $$=makeExpr(TERM,NULL,NULL,makeTerm(tmpId,$1,POST));
  
}
|
id_
{  
  int tmpId;
  tmpId=getId($1,idVarList);
  if (tmpId==-1)
    {
      yyerror("Use of a non declared variable");
    }
  $$=makeExpr(TERM,NULL,NULL,makeTerm(tmpId,makeRational(POSITIVE,1,1),NORMAL));
  
}
|
id_ '\''
{
  int tmpId;
  tmpId=getId($1,idVarList);
  if (tmpId==-1)
    {
      yyerror("Use of a non declared variable");
    }
  $$=makeExpr(TERM,NULL,NULL,makeTerm(tmpId,makeRational(POSITIVE,1,1),POST));
  
}
;

rate_linear_expression :
rate_linear_expression '+' rate_linear_term
{

}
|
rate_linear_expression '-' rate_linear_term
{
}
|
rate_linear_term
{

}
;

rate_linear_term :
rational
{

}
|
rational id_
{
}
|
id_
{
}
;

rational :
integer
{
  $$=$1;
}
|
integer '/' positive_integer_
{
  ($1->posDenominator) *= $3;
  $$=$1;
}
;

integer :
positive_integer_
{
  $$=makeRational(POSITIVE,$1,1);
}
|
'-' positive_integer_
{
  $$=makeRational(NEGATIVE,$2,1);
}
;

%%

#include <stdio.h>


int yyerror (char *s){
  /* fonction invoquée quand une erreur de parsing se produit. <s> contient le
     message d'erreur associé à l'erreur de parsing survenue */
  fprintf(stderr, "- Error (line %i) : %s", yylineno+1, s);
  fclose(yyin);
  exit(-1);
  return -1;
}

void setInputParser(string tempFileName){
/* précise que la construction de l'arbre de parsing se fera à partir du contenu
   du fichier <tempFileName>  et initialisation de certains champs*/
  yylineno=0;
  syst= makeEmptySystem();
  
  /* vérification des paramètres */
  if (tempFileName == NULL){
    fprintf(stderr, "function setInputParser error ! The parameter 'tempFileName' can't be NULL.\n");
    exit(-1);
  }
  /* fin vérification des paramètres */
  yyin = fopen(tempFileName, "r");
  if (yyin == NULL){
    fprintf(stderr, "Erreur durant l'ouverture du fichier temporaire pour input parser.\n");
    exit(-1);
  }  
}

struct system * getSystem()
{
  return syst;
}

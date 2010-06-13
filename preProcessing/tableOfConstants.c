
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tableOfConstants.h"


struct NodePP{
  string id;
  string constant;
  TableOfConstants Left;
  TableOfConstants Right;
};

TableOfConstants insertConstant(TableOfConstants T, string id, string constant){
/* insère <id> et <constant> dans <T>, puis retourne <T> */
  /* vérification des paramètres */
  if (id == NULL){
    fprintf(stderr, "function insertConstant error ! The parameter 'id' can't be NULL\n");
    exit(-1);
  }
  if (constant == NULL){
    fprintf(stderr, "function insertConstant error ! The parameter 'constant' can't be NULL\n");
    exit(-1);
  }
  /* fin vérification des paramètres */
  if (T == NULL){
    T = (TableOfConstants)malloc(sizeof(struct NodePP));
    T->id = (string)malloc(strlen(id) + 1);
    strcpy(T->id, id);
    T->constant = (string)malloc(strlen(constant) + 1);
    strcpy(T->constant, constant);
    T->Left = NULL;
    T->Right = NULL;
  }
  /* on descend au fils gauche */
  else if (strcmp(T->id, id) < 0){
    T->Left = insertConstant(T->Left, id, constant);
  }
  /* on descend au fils droit */
  else if (strcmp(T->id, id) > 0){
    T->Right = insertConstant(T->Right, id, constant);
  }
  /* si élément déjà présent, on ne fait rien */
  return T;
}

string findConstant(TableOfConstants T, string id){
/* recherche <id> dans <T> et retourne le string nommé "constant" associé à <id>.
Si <id> non trouvé, NULL retourné */
  struct NodePP * p;
  string result = NULL;
  /* vérification des paramètres */
  if (id == NULL){
    fprintf(stderr, "function findConstant error ! The parameter 'id' can't be NULL\n");
    exit(-1);
  }
  /* fin vérification des paramètres */
  p = T;
  while (p){
    /* élément trouvé */
    if (strcmp(p->id, id) == 0){
      break;      
    }
    /* on descend au fils gauche */
    else if (strcmp(p->id, id) < 0){
      p = p->Left;      
    }
    /* on descend au fils droit */
    else{
      p = p->Right;      
    }    
  }
  if (p){
    result = (char *)malloc((strlen(p->constant)) + 1);
    strcpy(result, p->constant);
  }
  return result;
}

void deleteTableOfConstants(TableOfConstants T){
/* supprime la table des constantes <T> */
  if (T != NULL){
    deleteTableOfConstants(T->Left);
    deleteTableOfConstants(T->Right);
    free(T->id);
    T->id = NULL;
    free(T->constant);
    T->constant = NULL;
    free(T);
    T = NULL;
  }
}

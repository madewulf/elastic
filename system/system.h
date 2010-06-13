#ifndef __SYSTEM_H
#define __SYSTEM_H
#include "../automata/automata.h"
#include "../avl/avl.h"
#include "../utils/labStatus.h"
struct system
{
  struct AvlNode * idDic; /*la table des ID*/
  struct var * varList; /*la liste des variables*/
  struct automaton * automList;
  expr paramInit;
  expr badStates;
  bool receptivenessChecking;
  struct labStatus * viewList;
};

struct system * makeEmptySystem();
#endif

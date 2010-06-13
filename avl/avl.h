#ifndef _Avl_H
#define _Avl_H
#include "../automata/automata.h"

struct AvlNode;

typedef struct AvlNode *AvlTree;

struct AvlNode
{
  int id;			/* l'id de la location, transition, automate,... */
  AvlTree Left;
  AvlTree Right;
  int Height;

  string name;
  enum nameType type;
  union
  {
    enum varType vType;/*INTEGRATOR, STOPWATCH, CLOCK, ANALOG, PARAMETER ou DISCRETE*/
    struct lab * labPtr; 
    struct location *locPtr;
    struct automaton *automPtr;
  } data;
};

AvlTree Find (int X, AvlTree T);
AvlTree Insert (AvlTree X, AvlTree T);
AvlTree makeAvlNode(int id, string s, enum nameType ty);
string getName(int X,AvlTree T);
int getFreshId();
void printAvl(AvlTree T);
#endif	/*_AvlTree_H */

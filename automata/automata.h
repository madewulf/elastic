#ifndef __AUTOMATA_H
#define __AUTOMATA_H

#include "../constraints/constraints.h"

struct var
{
  int id;
  struct var * next; /*pour constituer des listes de variable*/
};

struct lab
{
  int id;
  enum labelType type; 
  struct lab * next;
};

struct transition
{
  int idDest; /*l'identifiant de la destination*/
  int idLabel; /*l'identifiant du label*/
  enum transitionStatus ts; /*ORDINARY, ASAP,BROADCASTEMITTER,BROADCASTRECEIVER*/
  expr guard;
  expr update;
  struct transition * next; /*pour constituer des listes de transitions*/
};

struct location
{
  int id;/*l'identifiant de la location*/
  expr inv;
  expr rateCond;
  struct transition * transitions;
  enum locationType type;/*SLW ou URG*/
  struct location * next; /*pour constituer des listes de location*/
};

struct automaton
{
  int id;
  enum automType type;
  expr initialisation;
  int idInitLoc;
  int idParameter;
  struct lab * labList; /*la liste des labels*/
  struct location * locations;
  struct automaton * next; /*pour constituer des listes d'automates*/
};

struct var * makeVar(int id,  
		     struct var * next);
struct lab * makeLabel(int id, enum labelType type, struct lab * next);
struct transition * makeTransition(int idDest,
				   int idLabel,
				   enum transitionStatus ts,
				   expr guard,
				   expr udpate,
				   struct transition*  next);
struct location * makeLocation( int id,
				expr inv,
				expr rateCond,
				struct transition * transitions,
				struct location * next);
struct automaton * makeAutomaton(int id, 
				 enum automType type,
				 expr initialisation,
				 int idInitLoc,
				 struct lab * labList,
				 struct location * locations,
				 struct automaton * next);

struct lab * appendLabList(struct lab * l1, struct lab * l2);
 
#endif

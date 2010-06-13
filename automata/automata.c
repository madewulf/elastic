#include "automata.h"


struct var * makeVar(int id, struct var * next)
{
  struct var * res;
  res= (struct var * ) malloc (sizeof(struct var));
  if (!res)
    {
      fprintf(stderr,"Not enough memory! \n");
      exit(-1);
    }
  res->id=id;
  res->next=next;
  return res;
}

struct lab * makeLabel(int id, enum labelType type, struct lab * next)
{
  struct lab * res;
  res= (struct lab * ) malloc (sizeof(struct lab));
  if (!res)
    {
      fprintf(stderr,"Not enough memory! \n");
      exit(-1);
    }
  res->id=id;
  res->next=next;
  res->type=type;
  return res;
}

struct transition * makeTransition(int idDest,
				   int idLabel,
				   enum transitionStatus ts,
				   expr guard,
				   expr update,
				   struct transition*  next)
{
  struct transition * res;
  res= (struct transition * ) malloc (sizeof(struct transition));
  if (!res)
    {
      fprintf(stderr,"Not enough memory! \n");
      exit(-1);
    }
  res->idDest=idDest;
  res->idLabel=idLabel;
  res->ts=ts;
  res->guard=guard;
  res->update=update;
  res->next=next;
  return res;
}

struct location * makeLocation( int id,
				expr inv,
				expr rateCond,
				struct transition * transitions,
				struct location * next)
{
  struct location * res;
  res= (struct location * ) malloc (sizeof(struct location));
  if (!res)
    {
      fprintf(stderr,"Not enough memory! \n");
      exit(-1);
    }
  res->id=id;
  res->inv=inv;
  res->rateCond=rateCond;
  res->transitions=transitions;
  res->type=SLW;
  res->next=next;
  return res;
}

struct automaton * makeAutomaton( int id,
				  enum automType type,
				  expr initialisation,
				  int idInitLoc,
				  struct lab * labList,
				  struct location * locations,
				  struct automaton * next)
{
  struct automaton * res;
  res= (struct automaton * ) malloc (sizeof(struct automaton));
  if (!res)
    {
      fprintf(stderr,"Not enough memory! \n");
      exit(-1);
    }
  res->id=id;
  res->type=type;
  res->initialisation=initialisation;
  res->idInitLoc=idInitLoc;
  res->labList=labList;
  res->locations=locations;
  res->next=next;
  return res;
}

struct lab * appendLabList(struct lab * l1, struct lab * l2)
{
  struct lab * res;
  if (!l1)
    res=l2;
  else
    {
      res=l1;
      while(l1->next)
	l1=l1->next;
      l1->next=l2;
    }
  return res;
}

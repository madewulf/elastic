#include "avl.h"
#include <stdlib.h>
#include "fatal.h"
#include "../enums.h"

int prof=0;
int count=0;

AvlTree makeAvlNode(int id, string s, enum nameType ty)
{
  AvlTree res;
  res= (AvlTree) malloc(sizeof(struct AvlNode));
  if (!res)
    {
      fprintf(stderr, "Not enough memory!\n");
      exit(-1);
    }
  switch(ty)
    {
    case LOCATION :
      {
	res->data.locPtr=(struct location *) malloc(sizeof(struct location));
	if (!res->data.locPtr)
	  {
	    fprintf(stderr, "Not enough memory!\n");
	    exit(-1);
	  }    
	break ;
      }     
    case  AUTOMATON :
      {
	res->data.automPtr=(struct automaton *) malloc (sizeof(struct automaton));
	if (!res->data.automPtr)
	  {
	    fprintf(stderr, "Not enough memory!\n");
	    exit(-1);
	  }    
	break;
      }      
    default : 
      break;
    }
  res->Left=NULL;
  res->Right=NULL;
  res->Height=0;
  res->id=id;
  res->type=ty;
  res->name=(string)  malloc (sizeof(char)* (strlen(s)+1));
  strcpy(res->name,s);
  return res;
}

AvlTree
Find (int X, AvlTree T)
{
  if (T == NULL)
    return NULL;
  if (X < T->id)
    return Find (X, T->Left);
  else if (X > T->id)
    return Find (X, T->Right);
  else
    return T;
}

static int
Height (AvlTree P)
{
  if (P == NULL)
    return -1;
  else
    return P->Height;
}

static int
Max (int Lhs, int Rhs)
{
  return Lhs > Rhs ? Lhs : Rhs;
}


/* This function can be called only if K2 has a left child */
/* Perform a rotate between a node (K2) and its left child */
/* Update heights, then return new root */

static AvlTree
SingleRotateWithLeft (AvlTree K2)
{
  AvlTree K1;

  K1 = K2->Left;
  K2->Left = K1->Right;
  K1->Right = K2;

  K2->Height = Max (Height (K2->Left), Height (K2->Right)) + 1;
  K1->Height = Max (Height (K1->Left), K2->Height) + 1;

  return K1;			/* New root */
}


/* This function can be called only if K1 has a right child */
/* Perform a rotate between a node (K1) and its right child */
/* Update heights, then return new root */

static AvlTree
SingleRotateWithRight (AvlTree K1)
{
  AvlTree K2;

  K2 = K1->Right;
  K1->Right = K2->Left;
  K2->Left = K1;

  K1->Height = Max (Height (K1->Left), Height (K1->Right)) + 1;
  K2->Height = Max (Height (K2->Right), K1->Height) + 1;

  return K2;			/* New root */
}


/* This function can be called only if K3 has a left */
/* child and K3's left child has a right child */
/* Do the left-right double rotation */
/* Update heights, then return new root */

static AvlTree
DoubleRotateWithLeft (AvlTree K3)
{
  /* Rotate between K1 and K2 */
  K3->Left = SingleRotateWithRight (K3->Left);

  /* Rotate between K3 and K2 */
  return SingleRotateWithLeft (K3);
}


/* This function can be called only if K1 has a right */
/* child and K1's right child has a left child */
/* Do the right-left double rotation */
/* Update heights, then return new root */

static AvlTree
DoubleRotateWithRight (AvlTree K1)
{
  /* Rotate between K3 and K2 */
  K1->Right = SingleRotateWithLeft (K1->Right);

  /* Rotate between K1 and K2 */
  return SingleRotateWithRight (K1);
}

AvlTree
Insert (AvlTree X, AvlTree T)
{
  if (X)
    {
    
      if (T == NULL)
	{
	  T = X;
	}
      else
	{
	  if (X->id < T->id)
	    {
	      T->Left = Insert (X, T->Left);
	      if (Height (T->Left) - Height (T->Right) == 2)
		{
		  if (X->id < T->Left->id)
		    T = SingleRotateWithLeft (T);
		  else
		    T = DoubleRotateWithLeft (T);
		}
	    }
	  else if (X->id > T->id)
	    {
	      T->Right = Insert (X, T->Right);
	      if (Height (T->Right) - Height (T->Left) == 2)
		{
		  if (X->id > T->Right->id)
		    T = SingleRotateWithRight (T);
		  else
		    T = DoubleRotateWithRight (T);
		}
	    }
	  else
	    printf ("%s déjà inséré ! \n", X->name);
	    
	}
      T->Height = Max (Height (T->Left), Height (T->Right)) + 1;
    }
  return T;
}

void printAvl(AvlTree T)
{
  int i;
  if (T)
    {
      prof++;
      printAvl(T->Left);
      for(i=0;i<prof;i++)
	printf("   ");
      printf("ID: %i, HEIGHT: %i, NAME: %s, TYPE: %i\n",T->id,T->Height,T->name,T->type);
      printAvl(T->Right);
      prof--;
    }
}

int getFreshId()
{
  count++;
  return count;
}

string getName(int X, AvlTree T)
{
  AvlTree tmp;
  tmp= Find(X,T);
  if (tmp)
    return tmp->name;
  else
    return NULL;
}

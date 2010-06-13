#ifndef __CONSTRAINTS_H
#define __CONSTRAINTS_H
#include "../rationals/rationals.h"


struct exprNode;

typedef struct exprNode * expr;

struct term
{
  int id;                /*convention : idVar=-1 signifie que le terme est une constante*/
  struct rational * coeff;
  enum varStatus status;
};

struct exprNode
{
  enum exprType type;  /*convention : selon le type, on utilise soit ls et rs, soit te, jamais les deux*/
  struct exprNode * ls;
  struct exprNode *  rs;  

  struct term * te;
};

expr makeExpr(enum exprType type, expr ls, expr rs, struct term * te);
struct term * makeTerm(int id, struct rational * coeff, enum varStatus status);
expr extractInferiorBounds(expr *);
expr copyExpression(expr expression);
expr DNF(expr expression);
bool greaterE(expr A, expr B);
bool imply(expr A, expr B);
void simplifyConjunction(expr * pe, expr * f);
void simplifyDisjunction(expr * pe, expr * f);
#endif

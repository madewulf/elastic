#include "constraints.h"

struct term * makeTerm(int id, struct rational * coeff, enum varStatus status)
{
  struct term * res;
  res= (struct term *) malloc (sizeof(struct term));
  if (!res)
    {
      fprintf(stderr,"Not enough memory!\n");
      exit(-1);
    }

  res->id=id;
  res->coeff=coeff;
  res->status=status;
  return res;
}
struct term * copyTerm(struct term * data)
{
  if (data)
    return makeTerm(data->id, copyRational(data->coeff), data->status);
  else
    return NULL;
}


expr makeExpr(enum exprType type, expr ls, expr rs, struct term * te)
{
  expr  res;
  res= (expr) malloc(sizeof(struct exprNode));
  if (!res)
    {
      fprintf(stderr,"Not enough memory!\n");
      exit(-1);
    }
  res->type= type;
  res->ls=ls;
  res->rs=rs;
  res->te=te;
  return res;
}

expr copyExpression(expr data)
{
  if (data)
    return makeExpr(data->type,copyExpression(data->ls),
		    copyExpression(data->rs),
		    copyTerm(data->te)
		    );
  else
    return NULL;
}

expr extractInferiorBounds(expr * pe)
{
  expr res=NULL;
  expr e ;
 
  e= *pe;

  if (e)
    {
      switch(e->type) 
	{
	case AND : 
	  {
	    expr ls,rs;
	    ls= extractInferiorBounds(&(e->ls));
	    rs= extractInferiorBounds(&(e->rs));
	    if (ls && rs)
	      res=makeExpr(AND,ls,rs,NULL);
	    else
	      {
		if (ls)
		  res=ls;
		else if (rs)
		  res=rs;
		/*last possibility : res must be set to NULL, nothing to do*/
	      }
	    if (e->ls ==NULL && e->rs==NULL)
	      *pe=NULL;
	    else
	      {
		
		if(e->ls==NULL)
		  *pe=e->rs;
		else if (e->rs==NULL)
		  *pe=e->ls;
	      }
	    
	    break;
	  }
	case GEQ : 
	  {
	    res=e;
	    *pe=NULL;
	    break;
	  }
	case GRT : 
	  {
	    res=e;
	    *pe=NULL;
	    break;
	  }
	default : break;
	}
    }
  return res;
}
expr DNF(expr e)
{
 if (e)
    {
      switch(e->type) 
	{
	case AND : 
	  {
	    
	    e->ls=DNF(e->ls);
	    e->rs=DNF(e->rs);
	    if (e->ls->type==OR)
	      {
		expr a,b,c;
		a=e->ls->ls;
		b=e->ls->rs;
		c=e->rs;
		e->type=OR;
		e->ls->type=AND;
		e->ls->rs=c;
		e->rs=makeExpr(AND,b,copyExpression(c),NULL);
	      }
	    else if (e->rs->type==OR)
	      {
		expr a,b,c;
		a=e->ls;
		b=e->rs->ls;
		c=e->rs->rs;
		e->type=OR;
		e->rs->type=AND;
		e->rs->ls=a;
		e->ls=makeExpr(AND,copyExpression(a),b,NULL);
		
		
	      }
	    e->ls=DNF(e->ls);
	    e->rs=DNF(e->rs);
	    break;
	  }
	case OR :
	  {
	    e->ls=DNF(e->ls);
	    e->rs=DNF(e->rs);
	    break;
	  }
	default : 
	  break;
	}
    }
 return e;

}




bool greaterE(expr A, expr B)
{ 
  if (A && B) 
    switch(A->type) 
      { 
      case TERM :  
	switch (B->type)
	  {
	  case TERM : 
	    return (A->te->id==B->te->id && greaterR(A->te->coeff,B->te->coeff));
	  case PLUS : 
	    return(greaterE(A,B->ls));
	    break;
	  case MINUS : 
	    return(greaterE(A,B->ls));
	    break;		    
	  default:
	    return 0;
	  }
      case PLUS :
	switch(B->type)
	  {
	  case TERM : 
	    return(greaterE(A->ls,B));
	    break;
	  case PLUS : 
	    return(greaterE(A->ls,B->ls));
	    break;
	  case MINUS :
	    return(greaterE(A->ls,B->ls));
	    break;
	  default : 
	    return 0;
	    
	  }
      case MINUS :
	switch(B->type)
	  {
	  case TERM : 
	    return(greaterE(A->ls,B));
	    break;
	  case PLUS : 
	    return(greaterE(A->ls,B->ls));
	    break;
	  case MINUS :
	    return(greaterE(A->ls,B->ls));
	    break;	    
	  default : 
	    return 0; 
	  }
      default :
	return 0;
	break;
      } 
  else return 0; 
} 

bool imply(expr A, expr B)  
{
  /* A is a conjunction of inequality, B is an inequality*/ 
  if ((A!=NULL && B!=NULL) && A!=B)
    {
      switch(A->type)  
	{ 
	case LEQ :
	  {
	    if (B->type== LEQ)
	      return(greaterE(B->rs,A->rs));
	    else
	      return 0;
	    break;
	  }
	case GEQ : 
	  {
	    if (B->type== GEQ)
	      return greaterE(A->rs,B->rs);
	    else
	      return 0;
	    break;
	  }
	case AND : 
	  {
	    return(imply(A->ls,B) || imply(A->rs,B));
	    break;
	  }
	default :
	  { 
	    printf("problem \n");
	    return 0;
	    break;
	  }
	} 
    }
  else return 0;
}


bool implyConj(expr A, expr B)
{
  /* A  and B are two conjunctions of inequalities*/
  if (B && B!=A)
    {
      switch(B->type)
	{
	case AND : 
	  { 
	    return(implyConj(A,B->ls)&& implyConj(A,B->rs));
	    break;
	  }
	default : 
	  {
	    return(imply(A,B));
	    break;
	  }
	}
      
    }
  else
    return 0 ;
}

bool implyDisj(expr A, expr B)
{
  /* A is a conjunction of inequalities,
     B is a disjunction of conjunction of inequalities*/
  /*The Function returns true if A implies any of the conjunction of B*/ 
  if (B && B!=A)
    {
      switch(B->type)
	{
	case OR : 
	  { 
	    return(implyDisj(A,B->ls) || implyDisj(A,B->rs));
	    break;
	  }
	 default: 
	  {
	    return(implyConj(A,B));
	    break;
	  }
	}
      
	}
  return 0;
}



void simplifyDisjunction(expr * pe, expr * f)
{
  bool equalFlag;
  expr e= *pe;
  equalFlag=(pe==f);
  
  if (e)
    switch(e->type)
      {
      case OR :
	{
	  simplifyDisjunction(&(e->ls),f);
	  if (!(e->ls))
	    {
	      (*pe)=e->rs;
	      if (equalFlag)
		*f=*pe;
	      simplifyDisjunction(pe,f);
	    }
	  else
	    {
	      simplifyDisjunction(&(e->rs),f);
	      if (!(e->rs))
		{	      
		  (*pe)=e->ls;
		  if (equalFlag)
		    *f=*pe;
		}
	    }
	  break;
	}
      default : 
	{
	  simplifyConjunction(pe,pe);
	  if (implyDisj(e,*f))
	    {
	      *pe=NULL;
	      if (equalFlag)
		*f=*pe;
	    }
	  break;
	}
      }
}

void simplifyConjunction(expr * pe, expr * f)
{
  bool equalFlag;
  expr e= *pe;
  equalFlag=(pe==f);

  if (e)
    switch(e->type)
      {
      case AND :
	{
	  simplifyConjunction(&(e->ls),f);
	  if (!(e->ls))
	    {
	      (*pe)=e->rs;
	      if (equalFlag)
		*f=*pe;
	      simplifyConjunction(pe,f);
	    }
	  else
	    {
	      simplifyConjunction(&(e->rs),f);
	      if (!(e->rs))
		{	      
		  (*pe)=e->ls;
		  if (equalFlag)
		    *f=*pe;
		}
	    }
	  break;
	}
      default : /*LES or GEQ*/
	{
	  if (imply(*f,e))
	    {
	      *pe=NULL;
	      if (equalFlag)
		*f=*pe;
	    }
	  break;
	}
      }
}


/* } */


/* expr simplify(expr expression) */
/* { */
/*   expr res; */
/*   expr temp; */
/*   temp=DNF(expression); */
  







/*   return res; */
/* } */

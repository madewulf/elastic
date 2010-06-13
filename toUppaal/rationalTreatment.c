#include "rationalTreatment.h"
#include <stdio.h>

int ppcmOfExpr(expr expression, int p)
{
  if (expression) 
    {
      switch(expression->type)
	{
	case TERM : 
	  {
	    if (expression->te->coeff)
	      return computePpcm(p,expression->te->coeff->posDenominator);
	    else return p;
	    break;
	  }
	default : 
	  {
	    int res;
	    res =ppcmOfExpr(expression->ls,p);
	    res=ppcmOfExpr(expression->rs,res);
	    return res;
	  }
	}
    }
  else
    return p;
}

void convertExpr(expr expression, int ppcm, struct system * syst)
{
  if (expression) 
    {
      switch(expression->type)
	{
	case TERM : 
	  {
	    if (expression ->te && expression->te->id==-1)
	      if (expression->te->coeff)
		{	      
		  struct rational * tmp;
		  tmp=expression->te->coeff;
		  tmp->posNumerator*=(ppcm/tmp->posDenominator);
		  tmp->posDenominator=1;
		}
	    break;
	  }
	default : 
	  {
	    convertExpr(expression->ls,ppcm,syst);
	    convertExpr(expression->rs,ppcm,syst);
	    break;
	  }
	}
    }
}  


void rationalsToInteger(struct system * syst)
{
  struct automaton * autom;
  struct location * locTmp;
  struct transition * transTmp;

  int ppcm=1;
  for(autom= syst->automList; autom!=NULL;autom=autom->next)
    {
      ppcm=ppcmOfExpr(autom->initialisation, ppcm);
      for(locTmp=autom->locations;locTmp!=NULL;locTmp=locTmp->next)
	{
	  ppcm=ppcmOfExpr(locTmp->inv,ppcm);
	  for(transTmp=locTmp->transitions;transTmp!=NULL;transTmp=transTmp->next)
	    {
	      ppcm=ppcmOfExpr(transTmp->guard,ppcm);
	      ppcm=ppcmOfExpr(transTmp->update,ppcm);
	    }
	}
    }
  ppcm=ppcmOfExpr(syst->paramInit ,ppcm);
  printf("PPCM : %i \n", ppcm);

  for(autom= syst->automList; autom!=NULL;autom=autom->next)
    {
      convertExpr(autom->initialisation, ppcm,syst);
      for(locTmp=autom->locations;locTmp!=NULL;locTmp=locTmp->next)
	{
	  convertExpr(locTmp->inv,ppcm,syst);

	  for(transTmp=locTmp->transitions;transTmp!=NULL;transTmp=transTmp->next)
	    {
	      convertExpr(transTmp->guard,ppcm,syst);
	      convertExpr(transTmp->update,ppcm,syst);
	    }
	}
    }
  convertExpr(syst->paramInit,ppcm,syst);
}

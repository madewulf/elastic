#include "writeUppaal.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../utils/labStatus.h"

int writeConstraintUppaal(expr e, FILE * idFile, AvlTree idDic);
bool printParamConstant(expr e, int id, FILE * idFile,AvlTree idDic);
struct labStatus * labStatusList=NULL;/*structure utilisée localement pour se rappeler si on a utilisé un point d'interrogation ou un point d'exclamation la dernière fois qu'on a imprimé un label ordinary*/
void writeReachabilityQuestion(expr e, FILE * idFile,AvlTree idDic);
void writeUppaal(struct system * syst, string fileName)
{
  
  FILE * idFile;
  FILE * idFile2;
  int virgule=0;
  struct var * varTmp;
  struct lab * labTmp;
  struct automaton * automTmp;
  struct labStatus * lstmp;
  string tmp;

  tmp=(string)malloc((strlen(fileName)+5)*sizeof(char));
  strcpy(tmp,fileName);
  strcat(tmp,".xml");
  idFile = fopen(tmp, "w");
  if (idFile == NULL){
    fprintf(stderr, "- Error : unable to create file  \"%s\".\n", tmp);
    exit(-1);
  }
  

  
  fprintf(idFile,"<?xml version=\"1.0\"?>\n");
  fprintf(idFile,"<!DOCTYPE nta PUBLIC \"-//Uppaal Team//DTD Flat System 1.0//EN\" \"http://www.docs.uu.se/docs/rtmv/uppaal/xml/flat-1_0.dtd\">\n");
  fprintf(idFile,"<nta>\n");
  
  /* écriture des déclarations de variable*/
  fprintf(idFile,"<declaration>");
  
  for(varTmp=syst->varList; varTmp!=NULL; varTmp=varTmp->next)
    {
      AvlTree t;
      t=Find(varTmp->id,syst->idDic);
      switch(t->data.vType)
	{
	case CLOCK :
	  {
	    fprintf(idFile,"\n");
	    fprintf(idFile,"clock ");
	    fprintf(idFile,"%s",t->name);
	    fprintf(idFile,";");
	    break;
	  }
	case DISCRETE:
	  {
	    fprintf(idFile,"\n");
	    fprintf(idFile,"int ");
	    fprintf(idFile,"%s",t->name);
	    fprintf(idFile,";");
	    break;
	  }
	case PARAMETER : 
	  {
	    fprintf(idFile,"\n");
	    fprintf(idFile,"const ");
	    fprintf(idFile,"%s ", t->name);
	    if (!(printParamConstant(syst->paramInit, t->id,idFile,syst->idDic)))
	      {
		printf("No init value found for parameter %s, assuming 0.\n",t->name);
		fprintf(idFile,"0");
	      }
	    fprintf(idFile,";");
	    break;
	  }
	default :
	  {
	    printf("Error: Elastic translation to Uppaal only allows discrete and clock variable \n");
	    exit(-1);
	    break;
	  }
	}
      
    }
  /*Ecriture des déclarations de channel*/
  /*à modifier */
  for(automTmp=syst->automList; automTmp!=NULL;automTmp=automTmp->next)
    for(labTmp=automTmp->labList; labTmp!=NULL; labTmp=labTmp->next)
      {	
	/*printf("id tmp : %i \n",labTmp->id);*/
	if (getStatus(labTmp->id,labStatusList)==-1)
	  {

	    labStatusList=insertIntoLabList(labTmp->id,labStatusList);
	  }
      }
  lstmp=labStatusList;
  while(lstmp!=NULL)
    {	
      AvlTree t;
      t=Find(lstmp->labId,syst->idDic);

      if (t!=NULL)/*à modifier*/
	switch(t->data.labPtr->type)
	  {
	  case STANDARD :
	    {
	      fprintf(idFile,"\n");
	      fprintf(idFile,"chan ");
	      fprintf(idFile,"%s",t->name);
	      fprintf(idFile,";");
	      break;
	    }
	  case BROADCAST:
	    {
	      fprintf(idFile,"\n");
	      fprintf(idFile,"broadcast chan ");
	      fprintf(idFile,"%s",t->name);
	      fprintf(idFile,";");
	      break;
	    }
	  case URGENT:
	    {
	      fprintf(idFile,"\n");
	      fprintf(idFile,"urgent chan ");
	      fprintf(idFile,"%s",t->name);
	      fprintf(idFile,";");
	      break;
	    }
	  default :
	    {	    
	      fprintf(idFile,"\n");
	      fprintf(idFile,"urgent chan ");
	      fprintf(idFile,"%s",t->name);
	      fprintf(idFile,";");
	      fprintf(stderr,"Warning : wrong label type \n");
	      break;
	    }
	    
	  }
      else 
	{
	  printf("Label Nul \n");
	  printf("Id label : %i\n", lstmp->labId);
	}
      lstmp=lstmp->next;
    }

  fprintf(idFile,"\n</declaration>");
  
  /*Ecriture des différents automates */

  for(automTmp=syst->automList;automTmp!=NULL;automTmp=automTmp->next)
    writeUppaalAutomaton(automTmp,idFile,syst->idDic);
  
  /*Ecriture du system*/
  fprintf(idFile,"\n");
  fprintf(idFile,"<system>system ");
  for(automTmp=syst->automList;automTmp!=NULL;automTmp=automTmp->next)
    {
      AvlTree t;
      if (virgule!=0)
	fprintf(idFile,", ");
      else
	virgule=1;
      t=Find(automTmp->id,syst->idDic);
      fprintf(idFile,"%s",t->name);
    }
  fprintf(idFile,";</system>");
  
  fprintf(idFile,"\n</nta>\n");

 
  fclose(idFile);
  printf("- File %s generated.\n",tmp);
  if (syst->badStates)
   {
     tmp=(string)malloc((strlen(fileName)+5)*sizeof(char));
     strcpy(tmp,fileName);
     strcat(tmp,".q");
     idFile2 = fopen(tmp, "w");
     if (idFile2 == NULL){
       fprintf(stderr, "- Error : unable to create file  \"%s\".\n", tmp);
       exit(-1);
     }
     fprintf(idFile2,"A[] not (");
     writeReachabilityQuestion(syst->badStates, idFile2,syst->idDic);
     fprintf(idFile2,")\n");
     fclose(idFile2);
     printf("- File %s generated.\n",tmp);
   }
}

bool printParamConstant(expr e, int id,FILE * idFile,AvlTree idDic)
{
  if (e)
    switch(e->type)
      {
      case AND :
	{
	  if (!(printParamConstant(e->ls,id,idFile,idDic)))
	    return (printParamConstant(e->rs,id,idFile,idDic));
	  else
	    return 1;
	  break;
	}
      case EQU :
	{
	  expr tmp;
	  int coeff;
	  AvlTree t;
	  t=Find(e->ls->te->id,idDic);
	  if (t->data.automPtr->idParameter==id)
	    {
	      tmp = e->rs;
	      coeff=(tmp->te->coeff->posNumerator) /(tmp->te->coeff->posDenominator);
	      if (tmp->te->coeff->sign==NEGATIVE)
		coeff=-coeff;
	      fprintf(idFile,"%i ",coeff);
	      return 1;
	    }
	  else return 0;
	  break;
	}
      default :
	{
	  return 0;
	}
      }
  return 0;
}
    
void  writeUppaalAutomaton(struct automaton * autom, FILE * idFile, AvlTree idDic)
{
  AvlTree t;
  struct location * locTmp;
  struct transition * transTmp;
  struct lab * labTmp;
  
  for(labTmp=autom->labList;labTmp!=NULL;labTmp=labTmp->next)
    inverseStatus(labTmp->id,labStatusList);
  
  t=Find(autom->id,idDic);
  
  fprintf(idFile,"\n");
  fprintf(idFile,"<template>");
  fprintf(idFile,"\n\t");
  fprintf(idFile,"<name>");
  fprintf(idFile,"%s",t->name);
  fprintf(idFile,"</name>");

  /*Ecriture des locations*/
    
  for(locTmp=autom->locations;locTmp!=NULL;locTmp=locTmp->next)
    writeUppaalLocation(locTmp,idFile,idDic);

  /*Ecriture de la location initiale*/
  fprintf(idFile,"\n\t");
  fprintf(idFile,"<init ref=\"");
    t=Find(autom->idInitLoc,idDic);
  fprintf(idFile,"%s",t->name);
  fprintf(idFile,"\"/>");

  /*Ecriture des transitions*/
    
  for(locTmp=autom->locations;locTmp!=NULL;locTmp=locTmp->next)
    for(transTmp=locTmp->transitions;transTmp!=NULL;transTmp=transTmp->next)
      writeUppaalTransition(transTmp,locTmp->id,idFile,idDic);

  fprintf(idFile,"\n");
  fprintf(idFile,"</template>");  
}

void writeUppaalLocation(struct location * loc,FILE * idFile,AvlTree idDic)
{
  AvlTree t;

  t=Find(loc->id,idDic);
  fprintf(idFile,"\n\t\t");
  fprintf(idFile,"<location id=\"");
  fprintf(idFile,"%s",t->name);
  fprintf(idFile,"\">");
  fprintf(idFile,"\n\t\t");
  fprintf(idFile,"<name>");
  fprintf(idFile,"%s",t->name);
  fprintf(idFile,"</name>");
  if (loc->inv)
    {
      fprintf(idFile,"\n\t\t");
      fprintf(idFile,"<label kind=\"invariant\">");
       writeConstraintUppaal(loc->inv, idFile,idDic);
      fprintf(idFile,"</label>");
    }
  if (loc->type==URG)
    fprintf(idFile,"<urgent/>");
  if (loc->type==COMMIT)
    fprintf(idFile,"<committed/>");
  fprintf(idFile,"\n\t\t");
  fprintf(idFile,"</location>");
}

void writeUppaalTransition(struct transition * trans, int idSource, FILE * idFile, AvlTree idDic)
{
  AvlTree t;
  

  fprintf(idFile,"\n\t\t");
  fprintf(idFile,"<transition>");
  /*écriture de la source*/
  fprintf(idFile,"\n\t\t\t");
  t=Find(idSource,idDic);
  fprintf(idFile,"<source ref=\"");
  fprintf(idFile,"%s",t->name);
  fprintf(idFile,"\"/>");
  /*écriture de la destination*/
  fprintf(idFile,"\n\t\t\t");
  t=Find(trans->idDest,idDic);
  fprintf(idFile,"<target ref=\"");
  fprintf(idFile,"%s",t->name);
  fprintf(idFile,"\"/>");
  /*écriture du label*/
  if (trans->idLabel!=-1)
    {
      t=Find(trans->idLabel,idDic);
      fprintf(idFile,"\n\t\t\t");
      fprintf(idFile,"<label kind=\"synchronisation\">");
      fprintf(idFile,"%s",t->name);
      if (trans->ts==BROADCASTEMITTER)
	fprintf(idFile,"!");
      else
	{
	  if (trans->ts==BROADCASTRECEIVER)
	    fprintf(idFile,"?");
	  else 
	    if (t->data.labPtr->type==STANDARD || t->data.labPtr->type==URGENT)
	      {
		if (getStatus(t->id,labStatusList)==0)
		  fprintf(idFile,"?");
		else
		  fprintf(idFile,"!");  
	      }
	}
      
      fprintf(idFile,"</label>");
    }
  if (trans->guard)
    {
      fprintf(idFile,"\n\t\t\t");
      fprintf(idFile,"<label kind=\"guard\">");
      writeConstraintUppaal(trans->guard, idFile,idDic);
      fprintf(idFile,"</label>");
    }
  if (trans->update)
    {
      fprintf(idFile,"\n\t\t\t");
      fprintf(idFile,"<label kind=\"assignment\">");
       writeConstraintUppaal(trans->update, idFile,idDic);
      fprintf(idFile,"</label>");
    }
  
  fprintf(idFile,"\n\t\t");
  fprintf(idFile,"</transition>");
}



int writeConstraintUppaal(expr e, FILE * idFile, AvlTree idDic)
{
  AvlTree t=NULL;
  if (e)
    switch (e->type)
      {
      case TERM : 
	{
	  t=Find(e->te->id,idDic);
	  switch (e->te->status)
	    {
	    case NORMAL : 
	      {
		int coeff=(e->te->coeff->posNumerator) /(e->te->coeff->posDenominator);
		if (e->te->coeff->sign==NEGATIVE)
		  coeff=-coeff;
		 if (coeff!=1)
		  
		  fprintf(idFile,"%i * ",coeff);
		fprintf(idFile,"%s ",t->name);
		return NORMAL;
		break;
	      }
	    case POST:
	      {
		int coeff=(e->te->coeff->posNumerator) /(e->te->coeff->posDenominator);
		if (e->te->coeff->sign==NEGATIVE)
		  coeff=-coeff;
		if (coeff!=1)
		  fprintf(idFile,"%i * ",coeff);
		fprintf(idFile,"%s ",t->name);
		return POST;
		break;
	      }
	    case DERIVATIVE :
	      {
		int coeff=(e->te->coeff->posNumerator) /(e->te->coeff->posDenominator);
		if (e->te->coeff->sign==NEGATIVE)
		  coeff=-coeff;
		if (coeff!=1)
		  fprintf(idFile,"%i * ",coeff);
		exit(-1);
		return DERIVATIVE;
		break;
	      }
	    case CONSTANT :
	      {
		int coeff=(e->te->coeff->posNumerator) /(e->te->coeff->posDenominator);
		if (e->te->coeff->sign==NEGATIVE)
		  coeff=-coeff;
		fprintf(idFile,"%i ",coeff);
		return CONSTANT;
		break;
	      }
	    }
	  break;
	}
      case TRUE : 
	{ 
	  fprintf(idFile,"true ");
	  return -1;
	  break;
	}
      case FALSE : 
	{ 
	  fprintf(idFile,"false ");
	  return -1;
	  break;
	}
      case AND : 
	{
	  writeConstraintUppaal(e->ls, idFile, idDic);
	  fprintf(idFile,", ");
	  writeConstraintUppaal(e->rs, idFile, idDic);
	  return -1;
	}
      case PLUS : 	
  	{
	  writeConstraintUppaal(e->ls, idFile, idDic);
	  fprintf(idFile,"+ ");
	  writeConstraintUppaal(e->rs, idFile, idDic);
	  return -1;
	}
      case MINUS : 	
  	{
	  writeConstraintUppaal(e->ls, idFile, idDic);
	  fprintf(idFile,"- ");
	  writeConstraintUppaal(e->rs, idFile, idDic);
	  return -1;
	}      
      case LES : 	
  	{
	  writeConstraintUppaal(e->ls, idFile, idDic);
	  fprintf(idFile,"&lt; ");
	  writeConstraintUppaal(e->rs, idFile, idDic);
	  return -1;
	}	
      case LEQ : 	
  	{
	  writeConstraintUppaal(e->ls, idFile, idDic);
	  fprintf(idFile,"&lt;= ");
	  writeConstraintUppaal(e->rs, idFile, idDic);
	  return -1;		
	}
      case EQU : 	
  	{
	  if (writeConstraintUppaal(e->ls, idFile, idDic)==POST)
	    fprintf(idFile,":= ");
	  else
	    fprintf(idFile,"== ");
	  writeConstraintUppaal(e->rs, idFile, idDic);
	  return -1;
	}
      case GEQ : 	
  	{
	  writeConstraintUppaal(e->ls, idFile, idDic);
	  fprintf(idFile,"&gt;= ");
	  writeConstraintUppaal(e->rs, idFile, idDic);
	  return -1;
	}
      case GRT : 	
  	{
	  writeConstraintUppaal(e->ls, idFile, idDic);
	  fprintf(idFile,"&gt; ");
	  writeConstraintUppaal(e->rs, idFile, idDic);
	  return -1;
	}
      default : 
	return -1;
	break;
	
      }
  return -1;
}

void writeReachabilityQuestion(expr e, FILE * idFile,AvlTree idDic)
{
  AvlTree t=NULL;
  if (e)
    switch (e->type)
      {
      case TERM : 
	{
	  t=Find(e->te->id,idDic);
	  switch (e->te->status)
	    {
	    case NORMAL : 
	      {
		int coeff=(e->te->coeff->posNumerator) /(e->te->coeff->posDenominator);
		if (e->te->coeff->sign==NEGATIVE)
		  coeff=-coeff;
		if (coeff!=1)
		  fprintf(idFile,"%i * ",coeff);
		fprintf(idFile,"%s ",t->name);
		break;
	      }
	    case CONSTANT :
	      {
		int coeff=(e->te->coeff->posNumerator) /(e->te->coeff->posDenominator);
		if (e->te->coeff->sign==NEGATIVE)
		  coeff=-coeff;
		fprintf(idFile,"%i ",coeff);
		break;
	      }
	    default :
	      {
		printf("Error: unauthorized type of variable in bad states\n");
		exit(-1);
		break;
	      }
	      break;
	    }
	}
      case AND : 
	{
	  fprintf(idFile,"(");
	  writeReachabilityQuestion(e->ls, idFile, idDic);
	  fprintf(idFile," AND ");
	  writeReachabilityQuestion(e->rs, idFile, idDic);
	  fprintf(idFile,")");
	  break;
	}
      case OR : 
	{
	  fprintf(idFile,"(");
	  writeReachabilityQuestion(e->ls, idFile, idDic);
	  fprintf(idFile," | ");
	  writeReachabilityQuestion(e->rs, idFile, idDic);
	  fprintf(idFile,")");
	  break;
	}
      case EQU : 	
	{
	  fprintf(idFile,"=");
	  break;
	}
      case LOCINIT :
	{
	  fprintf(idFile,getName(e->te->id,idDic));
	  fprintf(idFile,".");
	  fprintf(idFile,getName(e->te->status,idDic));
	  fprintf(idFile," ");
	  break;
	}
      default : 
	break;
	
      }

}

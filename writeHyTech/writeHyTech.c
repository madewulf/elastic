#include "writeHyTech.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../utils/labStatus.h"

void writeHyTech(struct system * syst, string fileName)
{
  
  FILE * idFile;
  enum varType vt;
  struct var * varTmp;
  struct automaton * automTmp;

  idFile = fopen(fileName, "w");
  if (idFile == NULL){
    fprintf(stderr, "- Error : in creating and opening file  \"%s\".\n", fileName);
    exit(-1);
  }
    /* écriture des déclarations de variable*/
  fprintf(idFile,"var\n");  

  for(vt=INTEGRATOR;vt<=DISCRETE;vt++)
    {
      bool first=1;
      for(varTmp=syst->varList; varTmp!=NULL; varTmp=varTmp->next)
	{
	  AvlTree t;
	  t=Find(varTmp->id,syst->idDic);
	  if(t->data.vType == vt)
	    {
	      if (first)
		{ 
		  fprintf(idFile,"\t");
		  fprintf(idFile,"%s",t->name);
		  first=0;
		}
	      else
		fprintf(idFile,", %s", t->name);
	    }
	}
      if(!first)
	switch(vt)
	  {
	  case INTEGRATOR:
	    {
	      fprintf(idFile,"\n\t: integrator ; \n");
	      break;
	    }
	  case STOPWATCH:
	    {
	      fprintf(idFile,"\n\t: stopwatch ; \n");
	      break;
	    }	
	  case CLOCK :
	    {
	      fprintf(idFile,"\n\t: clock ; \n");
	      break;
	    }
	  case ANALOG:
	    {
	      fprintf(idFile,"\n\t: analog ; \n");
	      break;
	    }	
	  case PARAMETER:
	    { 
	      fprintf(idFile,"\n\t: parameter ; \n");
	      break;
	    }	
	  case DISCRETE:
	    {
	      fprintf(idFile,"\n\t: discrete ; \n");
	      break;
	    }
	  default : 
	    {
	      fprintf(stderr,"Unsupported variable type\n");
	      exit(-1);
	    }  
	  }
    }
  /*Ecriture des différents automates */
  
  for(automTmp=syst->automList;automTmp!=NULL;automTmp=automTmp->next)
    writeHyTechAutomaton(automTmp,idFile,syst->idDic);
  writeCommands(idFile,syst);
  fclose(idFile);
  printf("- File %s generated.\n",fileName);
}

void writeHyTechAutomaton(struct automaton * autom, FILE * idFile,AvlTree idDic)
{
  AvlTree t;
  struct location * locTmp;
  struct lab * labTmp;
  int first;
  t=Find(autom->id,idDic);
  fprintf(idFile,"automaton %s \n",t->name);
  
 
  
  /*Print synclabs*/
  fprintf(idFile,"synclabs : ");
  first=1;
  for(labTmp=autom->labList; labTmp!=NULL; labTmp=labTmp->next)
    {
       t=Find(labTmp->id,idDic);
       if (first )
	 {
	   fprintf(idFile,"%s",t->name);
	   first=0;
	 }
       else
	 fprintf(idFile,", %s",t->name);
    }
  fprintf(idFile,"; \n");

  /*Print initial condition*/
  t=Find(autom->idInitLoc,idDic);
  fprintf(idFile,"\n");
  fprintf(idFile,"initially %s ",t->name);
  if (autom->initialisation)
    {
      fprintf(idFile,"& ");
      writeConstraintHyTech(autom->initialisation,idFile,idDic,GUARD);
    }
  fprintf(idFile,";\n");
     
  
  /*print locations */ 
  for(locTmp=autom->locations;locTmp!=NULL;locTmp=locTmp->next)
    writeHyTechLocation(locTmp,idFile,idDic);  
  fprintf(idFile,"end\n\n");
}
void writeHyTechLocation(struct location * loc,FILE * idFile,AvlTree idDic)
{
  AvlTree t;
  struct transition * transTmp;
  t=Find(loc->id,idDic);
  fprintf(idFile,"\n");
  fprintf(idFile,"loc %s : while ",t->name);
  if (loc->inv)
    writeConstraintHyTech(loc->inv,idFile,idDic,GUARD);
  else
    fprintf(idFile,"True ");
  fprintf(idFile,"wait{");
  if (loc->rateCond)
    writeConstraintHyTech(loc->rateCond,idFile,idDic,UPDATE);
  fprintf(idFile,"}\n");
  /*print transitions of the current location */
  for(transTmp=loc->transitions;transTmp!=NULL;transTmp=transTmp->next)
    writeHyTechTransition(transTmp,idFile,idDic);
} 

void writeHyTechTransition(struct transition * trans, FILE * idFile, AvlTree idDic)
{
  AvlTree t;
  fprintf(idFile,"\twhen ");
  if (trans->guard)
    writeConstraintHyTech(trans->guard, idFile,idDic,GUARD);
  else
    {
      if (trans->ts== ASAP)
	fprintf(idFile,"asap ");
      else
	fprintf(idFile,"True ");
    }
  if (trans->idLabel!=-1)
    {
      t=Find(trans->idLabel,idDic);
      fprintf(idFile,"sync %s ",t->name);
    }
  if (trans->update)
    {
      fprintf(idFile,"do{");
      writeConstraintHyTech(trans->update, idFile,idDic,UPDATE);
      fprintf(idFile,"} ");
    }
  fprintf(idFile,"goto ");
  t=Find(trans->idDest,idDic);
  fprintf(idFile,"%s;\n",t->name);
}

void writeConstraintHyTech(expr e, FILE * idFile, AvlTree idDic,enum constraintType ct)
{
  AvlTree t;
  if (e){
    switch(e->type)
      {
      case TERM : 
	{
	  t=Find(e->te->id,idDic);
	  switch (e->te->status)
	    {
	    case NORMAL :
	      {
		int coeff=(e->te->coeff->posNumerator);
		if (e->te->coeff->sign==NEGATIVE)
		  coeff=-coeff;
		if (e->te->coeff->posDenominator!=1 )  
		  fprintf(idFile,"%i/%i ",coeff,e->te->coeff->posDenominator );
		else
		  if (coeff!=1)
		      fprintf(idFile,"%i ", coeff);
		fprintf(idFile,"%s ",t->name);	
		break;
	      }
	    case CONSTANT :
	      {
		int coeff=(e->te->coeff->posNumerator);
		if (e->te->coeff->sign==NEGATIVE)
		  coeff=-coeff;
		if (e->te->coeff->posDenominator!=1)  
		  fprintf(idFile,"%i/%i ",coeff,e->te->coeff->posDenominator );
		else
		  fprintf(idFile,"%i ", coeff);
		break;
	      }
	    case POST:
	      {
		int coeff=(e->te->coeff->posNumerator);
		if (e->te->coeff->sign==NEGATIVE)
		  coeff=-coeff;
		if (e->te->coeff->posDenominator!=1 )  
		  fprintf(idFile,"%i/%i ",coeff,e->te->coeff->posDenominator );
		else
		  if (coeff!=1)
		    fprintf(idFile,"%i ", coeff);
		fprintf(idFile,"%s' ",t->name);
		break;
	      }
	    case DERIVATIVE :
	      {
		int coeff=(e->te->coeff->posNumerator);
		if (e->te->coeff->sign==NEGATIVE)
		  coeff=-coeff;
		if (e->te->coeff->posDenominator!=1 )  
		  fprintf(idFile,"%i/%i ",coeff,e->te->coeff->posDenominator );
		else
		  if (coeff!=1)
		    fprintf(idFile,"%i ", coeff);
		fprintf(idFile,"d%s ",t->name);
		break;
	      }
	    default : 
	      {
		break;
	      }
	    }
	  break;
	}
      case LOCINIT:
	{
	  fprintf(idFile,"loc[");
	  fprintf(idFile,getName(e->te->id,idDic));
	  fprintf(idFile,"]=");
	  fprintf(idFile,getName(e->te->status,idDic));
	  fprintf(idFile," ");
	  break;
	}
      case PARAM :
	{
	  AvlTree t2;
	  t2=Find(e->te->id,idDic);
	  fprintf(idFile,getName(t2->data.automPtr->idParameter,idDic));
	  break;
	}
      case TRUE :
	{ 
	  fprintf(idFile,"True ");
	  break;
	}
      case FALSE : 
	{ 
	  fprintf(idFile,"False ");
	  break;
	}
       
    case AND : 
	{
	  writeConstraintHyTech(e->ls, idFile, idDic, ct);
	  if (ct==GUARD) 
	    fprintf(idFile," & ");
	  else
	    fprintf(idFile," , ");
	  writeConstraintHyTech(e->rs, idFile, idDic, ct);
	  break;

	}
      case OR : 
	{
	  fprintf(idFile,"(");
	  writeConstraintHyTech(e->ls, idFile, idDic, ct);
	  fprintf(idFile,")");	  
	  fprintf(idFile," | ");
	  fprintf(idFile,"(");
	  writeConstraintHyTech(e->rs, idFile, idDic, ct);
	  fprintf(idFile,")");	  
	  break;
	}
      case PLUS : 	
  	{
	  writeConstraintHyTech(e->ls, idFile, idDic, ct);
	  fprintf(idFile,"+ ");
	  writeConstraintHyTech(e->rs, idFile, idDic, ct);
	  break;
	}
      case MINUS : 	
  	{
	  writeConstraintHyTech(e->ls, idFile, idDic, ct);
	  fprintf(idFile,"- ");
	  writeConstraintHyTech(e->rs, idFile, idDic, ct);
	  break;
	}      
      case LES : 	
  	{
	  writeConstraintHyTech(e->ls, idFile, idDic, ct);
	  fprintf(idFile,"< ");
	  writeConstraintHyTech(e->rs, idFile, idDic, ct);
	  break;
	}	
      case LEQ : 	
  	{
	  writeConstraintHyTech(e->ls, idFile, idDic, ct);
	  fprintf(idFile,"<= ");
	  writeConstraintHyTech(e->rs, idFile, idDic, ct);
	  break;
	}
      case EQU : 	
  	{
	  writeConstraintHyTech(e->ls, idFile, idDic, ct);
	  fprintf(idFile,"= ");
	  writeConstraintHyTech(e->rs, idFile, idDic, ct);
	  break;
	}
      case GEQ : 	
  	{
	  writeConstraintHyTech(e->ls, idFile, idDic, ct);
	  fprintf(idFile,">= ");
	  writeConstraintHyTech(e->rs, idFile, idDic, ct);
	  break;
	}
      case GRT : 	
  	{
	  writeConstraintHyTech(e->ls, idFile, idDic, ct);
	  fprintf(idFile,"> ");
	  writeConstraintHyTech(e->rs, idFile, idDic, ct);
	  break;
	}	
      default :
	break;
      }
  }
}


void writeCommands(FILE * idFile, struct system * syst)
{
  bool first;
  struct automaton * automTmp;
  
  first=1;
  fprintf(idFile,"var init_states, bad_states, reached : region;\n");
  fprintf(idFile,"init_states := \n\t   ");
  for(automTmp=syst->automList;automTmp!=NULL;automTmp=automTmp->next) 
    {
      if (!first)
	fprintf(idFile,"\n\t & ");
      else
	first=0;
      fprintf(idFile,"loc[");
      fprintf(idFile,getName(automTmp->id,syst->idDic));
      fprintf(idFile,"]= ");
      fprintf(idFile,getName(automTmp->idInitLoc,syst->idDic));
      if (automTmp->initialisation)
	{
	  fprintf(idFile," & ");
	  writeConstraintHyTech(automTmp->initialisation,idFile,syst->idDic,GUARD);
	}
    }
  if (syst->paramInit)
    {
      fprintf(idFile,"\n\t & ");
      writeConstraintHyTech(syst->paramInit,idFile,syst->idDic,GUARD);
    }
  fprintf(idFile,";\n");
  fprintf(idFile,"bad_states := \n\t   ");
  writeConstraintHyTech(syst->badStates,idFile,syst->idDic,GUARD);
  fprintf(idFile,";\n");
  fprintf(idFile,"reached := \n\treach forward from init_states endreach;\n");
  if (syst->paramInit) 
    {  
      fprintf(idFile,"if empty(bad_states & reached)\n");
      fprintf(idFile,"\t then prints \"System safe!\";\n");
      fprintf(idFile,"\t else prints \"System UNsafe, printing trace to error state:\" ;\n"); 
      fprintf(idFile,"\t print trace to bad_states using reached;\n");
      fprintf(idFile,"endif ;\n");
    }
  else{
      fprintf(idFile,"print omit all locations\n");
      fprintf(idFile,"\t hide non_parameters\n");
      fprintf(idFile,"\t in reached & bad_states\n");
      fprintf(idFile,"endhide;\n");
    }
  
}

#include "toUppaal.h"
#include "../utils/labStatus.h"
#include "../utils/utils.h"
#include "rationalTreatment.h"

void treatAsap(struct system * syst, int labId);

void treatSingle(struct system * syst, int labId, enum transitionStatus ts);

void treatMultiple(struct system * syst, int labId, int count,enum transitionStatus ts );

void addMultWatcher(struct system * syst, int labId, int count, int readyLab, int notReadyLab, enum transitionStatus ts);

void moveInferiorBound(struct system * syst);

void toUppaal(struct system * syst)
{
  struct automaton * autom;
  struct lab * label;
  struct location * locTmp;
  struct transition * transTmp;
  struct labStatus * tmpLabList=NULL;

  rationalsToInteger(syst);  
  
    /*premièrement, on collecte tous les labels qui sont ASAP*/
  for(autom= syst->automList; autom!=NULL;autom=autom->next)
    for(locTmp=autom->locations;locTmp!=NULL;locTmp=locTmp->next)
      for(transTmp=locTmp->transitions;transTmp!=NULL;transTmp=transTmp->next)
	{
	  if (transTmp->ts==ASAP)
	    {
	      if (transTmp->idLabel!=-1)
		{
		  tmpLabList= insertWithStatus(transTmp->idLabel, ASAP, tmpLabList);
		}
	      else
		locTmp->type=URG;
	      	            
	    }
	}
  
  /*deuxièmement, on compte combien d'automates utilisent chaque label*/
  for(autom= syst->automList; autom!=NULL;autom=autom->next)
    for(label=autom->labList;label!=NULL;label=label->next)
      tmpLabList=insertIncrement(label->id,tmpLabList);
 

  for( ; tmpLabList!=NULL;tmpLabList=tmpLabList->next)
    {
      if (tmpLabList->status==ASAP)
	{
	  if (tmpLabList->count>2)
	    {
	      treatMultiple(syst,tmpLabList->labId,tmpLabList->count,ASAP);
	      printf("Multiple sync and ASAP : %s \n", getName(tmpLabList->labId,syst->idDic));
	    }
	  else if (tmpLabList->count==2)
	    {
	      printf("Double sync and ASAP :  %s \n", getName(tmpLabList->labId,syst->idDic));
	      treatAsap(syst,tmpLabList->labId);
	    }
	  else 
	    {
	      treatSingle(syst,tmpLabList->labId, tmpLabList->status);
	      printf("Lonely sync and ASAP : %s \n",  getName(tmpLabList->labId,syst->idDic));
	    }
	}
      else
	{
	  if (tmpLabList->count >2)
	    {
	      printf("Multiple sync and not ASAP :  %s \n", getName(tmpLabList->labId,syst->idDic));
	       treatMultiple(syst,tmpLabList->labId,tmpLabList->count,ORDINARY);
	    }
	  else if (tmpLabList->count==1)
	    {
	      treatSingle(syst,tmpLabList->labId, tmpLabList->status);
	      printf("Lonely sync and not ASAP : %s \n",  getName(tmpLabList->labId,syst->idDic));
	    }

	  /*rien à faire si synchro à deux*/
	}
      
    }
  moveInferiorBound(syst);
}


void treatAsap(struct system * syst, int labId)
{
  struct automaton * autom;
  struct location * locTmp;
  struct transition * transTmp;
  
  AvlTree t;
  t=Find(labId,syst->idDic);
  t->data.labPtr->type=URGENT;
  
  /*ensuite, on dédouble les locations concernées */
  for(autom= syst->automList; autom!=NULL;autom=autom->next)
    for(locTmp=autom->locations;locTmp!=NULL;locTmp=locTmp->next)
      {
	struct labStatus * ltmp= NULL;
	
	for(transTmp=locTmp->transitions;transTmp!=NULL;transTmp=transTmp->next)
	  {
	    if (transTmp->idLabel==labId)
	      {
		if (transTmp->ts !=ASAP)
		  {
		    ltmp=insertIntoLabList(transTmp->idLabel,ltmp);
		  }
	      }
	  }
	
	for( ; ltmp!=NULL;ltmp=ltmp->next)
	  {
	    int idLoc;
	    string nameLoc;
	    string tmp;
	    string tmp2;
	    AvlTree t;
	    struct location * newLoc;
	    struct transition * transTmp2;
	    idLoc=getFreshId();
	    tmp=getName(locTmp->id,syst->idDic);
	    tmp2=getName(ltmp->labId,syst->idDic);
	    nameLoc=(string) malloc (sizeof(char)* (strlen(tmp)+strlen(tmp2)+4));
	    strcpy(nameLoc,"URG");
	    strcat(nameLoc,tmp);
	    strcat(nameLoc,tmp2);
	    
	    newLoc=makeLocation(idLoc,NULL,NULL,NULL,autom->locations);
	    newLoc->type=COMMIT;
	    autom->locations= newLoc; 
	    
	    t=makeAvlNode(idLoc,nameLoc,LOCATION);
	    t->data.locPtr=autom->locations;
	    syst->idDic=Insert(t,syst->idDic);
	    
	    /*transfert des "bonnes" transitions à la nouvelle location*/
	    transTmp2=locTmp->transitions;
	    for(transTmp=locTmp->transitions;transTmp!=NULL;)
	      if (transTmp->idLabel==ltmp->labId)
		{
		  if (transTmp==transTmp2)/*suppresion en tête de liste*/
		    {
		      locTmp->transitions=locTmp->transitions->next;
		      transTmp->next=newLoc->transitions;
		      newLoc->transitions=transTmp;
		      transTmp->idLabel=-1; /*on enlève le label*/
		      transTmp=locTmp->transitions;
		      transTmp2=locTmp->transitions;
		    }
		  else
		    {
		      transTmp2->next= transTmp->next;
		      transTmp->next=newLoc->transitions;
		      newLoc->transitions=transTmp;
		      transTmp->idLabel=-1;
		      transTmp=transTmp2->next;
		    }
		}
	      else
		{
		  transTmp2=transTmp;
		  transTmp=transTmp->next;
		}
	    
	    locTmp->transitions=
	      makeTransition(idLoc,
			     ltmp->labId,
			     ORDINARY,
			     NULL,
			     NULL,
			     locTmp->transitions);
	  }
      }
  
}

void treatSingle(struct system * syst, int labId, enum transitionStatus ts)
{
  struct automaton * autom;
  struct location * locTmp;
  struct transition * transTmp;  
  AvlTree t;
  t=Find(labId,syst->idDic);
  t->data.labPtr->type=BROADCAST;
  
  for(autom= syst->automList; autom!=NULL;autom=autom->next)
    for(locTmp=autom->locations;locTmp!=NULL;locTmp=locTmp->next)
      {
	int trouve =0;
	for(transTmp=locTmp->transitions;transTmp!=NULL;transTmp=transTmp->next) 
	  if (transTmp->idLabel==labId)
	    {
	      trouve=1;
	      transTmp->ts=BROADCASTEMITTER;
	    }
	if (trouve==1 && ts==ASAP)
	  locTmp->type=URG;
      }
}

void treatMultiple(struct system * syst, int labId, int count, enum transitionStatus ts)
{
  struct automaton * autom;
  struct lab * label;
  struct location * locTmp;
  struct transition * transTmp;
  int readyLab;
  int notReadyLab;
  string stmp;
  AvlTree t;
  AvlTree t2;
  string labName;
  
  labName=getName(labId,syst->idDic);

  /*changement de statut pour le label*/
  t=Find(labId,syst->idDic);
  t->data.labPtr->type=STANDARD;
  
  /*création des nouveaux labels */
  readyLab=getFreshId();
  notReadyLab=getFreshId();

  stmp=(string) malloc(sizeof(char)* (strlen(labName)+7));
  strcpy(stmp,"ready_");
  strcat(stmp,labName);
  label=makeLabel(readyLab,STANDARD,syst->automList->labList);
  syst->automList->labList=label; /*on insère le label dans la liste du premier automate de la liste, ça n'a pas d'importance pour uppaal*/
  t2=makeAvlNode(readyLab,stmp,LABEL);
  t2->data.labPtr=label;
  t2->data.labPtr->type=STANDARD;
  syst->idDic=Insert(t2,syst->idDic);

  stmp=(string) malloc(sizeof(char)* (strlen(labName)+10));
  strcpy(stmp,"notReady_");
  strcat(stmp,labName);
  label=makeLabel(notReadyLab,STANDARD,syst->automList->labList);
  syst->automList->labList=label; /*on insère le label dans la liste du premier automate de la liste, ça n'a pas d'importance pour uppaal*/
  t2=makeAvlNode(notReadyLab,stmp,LABEL);
  t2->data.labPtr=label;
  t2->data.labPtr->type=STANDARD;
  syst->idDic=Insert(t2,syst->idDic);
  
  /*attention, ne pas oublier d'insérer les labels dans les listes des automates*/

  for(autom= syst->automList; autom!=NULL;autom=autom->next)
    for(locTmp=autom->locations;locTmp!=NULL;locTmp=locTmp->next)
      {
	int trouve=0;
	
	for(transTmp=locTmp->transitions;transTmp!=NULL;transTmp=transTmp->next)
	  {
	    if (transTmp->idLabel==labId)
	      trouve =1;
	  }
	if (trouve==1)
	  {
	    struct location * newEntryLoc =NULL;
	    struct location * exitLoc =NULL;
	    int idNewEntryLoc, idExitOnLabLoc, idExitLoc;

	    struct location * locTmp2;
	    string locName;
	    
	    locName=getName(locTmp->id,syst->idDic);
	    
	    idNewEntryLoc=getFreshId();
	    idExitOnLabLoc=getFreshId();


	    /*mise à jour de la location initiale*/
	    if (autom->idInitLoc==locTmp->id)
	      autom->idInitLoc=idNewEntryLoc;

	    /*rajout d'1 location d'entrée */ 

	    stmp=(string) malloc(sizeof(char)* (strlen(labName)+strlen(locName)+15));
	    strcpy(stmp,"Enter_");
	    strcat(stmp,locName);
	    strcat(stmp,"_on_");
	    strcat(stmp,labName);
	    newEntryLoc=makeLocation(idNewEntryLoc,NULL,NULL,NULL,autom->locations);
	    newEntryLoc->type=COMMIT;
	    t2=makeAvlNode(idNewEntryLoc,stmp,LOCATION);
	    t2->data.locPtr=newEntryLoc;
	    syst->idDic=Insert(t2,syst->idDic);	  

 
	    
	    autom->locations=newEntryLoc;
	    /*modification de la destination des anciennes transitions dans tout l'automate*/
	    for(locTmp2=autom->locations;locTmp2!=NULL;locTmp2=locTmp2->next)
	      for(transTmp=locTmp2->transitions;transTmp!=NULL;transTmp=transTmp->next)
		{
		  if (transTmp->idDest==locTmp->id)
		    transTmp->idDest=idNewEntryLoc;
		}

	    for (transTmp=locTmp->transitions;transTmp!=NULL;transTmp=transTmp->next)
	      {
		if (transTmp->idLabel==labId)
		  {
		    /* création d'une nouvelle location */ 
/* 		    idExitLoc=getFreshId(); */

/* 		    exitLoc=makeLocation(idExitLoc,NULL,NULL,NULL,autom->locations); */
/* 		    autom->locations= exitLoc; */
/* 		    exitLoc->type=COMMIT; */
/* 		    exitLoc->transitions=makeTransition(transTmp->idDest, */
/* 							-1, */
/* 							BROADCASTRECEIVER, */
/* 							transTmp->guard, */
/* 							transTmp->update, */
/* 							NULL); */
/* 		    transTmp->guard=NULL; */
/* 		    transTmp->update=NULL; */
/* 		    transTmp->idDest=idExitLoc; */
		    transTmp->ts=BROADCASTRECEIVER;
		    
/* 		    stmp=(string) malloc(sizeof(char)* (strlen(locName)+15)); */
/* 		    strcpy(stmp,"Exit_"); */
/* 		    strcat(stmp,locName); */
/* 		    strcat(stmp,integerToString(idExitLoc)); */
/* 		    t2=makeAvlNode(idExitLoc,stmp,LOCATION); */
/* 		    t2->data.locPtr= exitLoc; */
/* 		    syst->idDic=Insert(t2,syst->idDic); */
		    /* toujours suppression en tête*/
		    

		  }
		else
		  {
		    idExitLoc=getFreshId();

		    exitLoc=makeLocation(idExitLoc,NULL,NULL,NULL,autom->locations);
		    autom->locations= exitLoc;
		    exitLoc->type=COMMIT;
		    exitLoc->transitions=makeTransition(transTmp->idDest,
							notReadyLab,
							BROADCASTEMITTER,
							NULL,
							NULL,
							NULL);
		    
		    transTmp->idDest=idExitLoc;

		    stmp=(string) malloc(sizeof(char)* (strlen(locName)+15));
		    strcpy(stmp,"Exit_");
		    strcat(stmp,locName);
		    strcat(stmp,integerToString(idExitLoc));
		    t2=makeAvlNode(idExitLoc,stmp,LOCATION);
		    t2->data.locPtr= exitLoc;
		    syst->idDic=Insert(t2,syst->idDic);


		  }
	      }
	    
	    /* ajout des nouvelles transitions*/ 
	    newEntryLoc->transitions=makeTransition(locTmp->id,readyLab,BROADCASTEMITTER,NULL,NULL,NULL);

	  }	
      }
  addMultWatcher(syst, labId,count,readyLab,notReadyLab,ts);  
}

void addMultWatcher(struct system * syst, int labId, int count, int readyLab, int notReadyLab, enum transitionStatus ts)
{
  expr expression;
  int idAutom;
  int idLoc1;
  int idLoc2;
  int idLoc3;
  int idVar;

  struct automaton * autom;
  struct location * loc1;
  struct location * loc2;
  struct location * loc3; 

  AvlTree t;
  string labName;
  string tmp;
  
  idAutom=getFreshId();
  idLoc1=getFreshId();
  idLoc2=getFreshId();
  idLoc3=getFreshId();
  idVar=getFreshId();

  autom=makeAutomaton(idAutom,UPPAAL,NULL,idLoc1,NULL,NULL,syst->automList);
  syst->automList=autom;
  
  /*insertion des nouveaux labels*/
  autom->labList=makeLabel(labId,STANDARD,NULL);
  autom->labList=makeLabel(readyLab,STANDARD,makeLabel(notReadyLab,STANDARD,autom->labList)); 
  
  loc3=makeLocation(idLoc3,NULL,NULL,NULL,NULL);
  loc2=makeLocation(idLoc2,NULL,NULL,NULL,loc3);
  loc1=makeLocation(idLoc1,NULL,NULL,NULL,loc2 );
  if (ts==ASAP)
    loc2->type=URG;
  else
    loc2->type=SLW;
  loc3->type=COMMIT;
  

  autom->locations=loc1;

  expression=makeExpr(EQU,
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(idVar,
					makeRational(POSITIVE,1,1),
					POST)
			       ),
		      makeExpr(PLUS,
			       makeExpr(TERM,
					NULL,
					NULL,
					makeTerm(idVar,
						 makeRational(POSITIVE,1,1),
						 NORMAL)
					),
			       makeExpr(TERM,
					NULL,
					NULL,
					makeTerm(-1,
						 makeRational(POSITIVE,1,1),
						 CONSTANT)
					),
			       NULL),
		      NULL
		      );

  loc1->transitions=makeTransition(idLoc1,
				   readyLab,
				   BROADCASTRECEIVER,
				   NULL,
				   expression,
				   NULL);
  expression=makeExpr(LEQ,
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(idVar,
					makeRational(POSITIVE,1,1),
					NORMAL)
			       ),
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(-1,
					makeRational(POSITIVE,count-2,1),
					CONSTANT)
			       ),
		      NULL);
  loc1->transitions->guard=expression;
  /**********************************************************/
  expression=makeExpr(EQU,
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(idVar,
					makeRational(POSITIVE,1,1),
					POST)
			       ),
		      makeExpr(MINUS,
			       makeExpr(TERM,
					NULL,
					NULL,
					makeTerm(idVar,
						 makeRational(POSITIVE,1,1),
						 NORMAL)
					),
			       makeExpr(TERM,
					NULL,
					NULL,
					makeTerm(-1,
						 makeRational(POSITIVE,1,1),
						 CONSTANT)
					),
			       NULL),
		      NULL
		      );
 loc1->transitions=makeTransition(idLoc1,
				   notReadyLab,
				   BROADCASTRECEIVER,
				   NULL,
				   expression,
				   loc1->transitions);
  /**********************************************************/

  expression=makeExpr(EQU,
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(idVar,
					makeRational(POSITIVE,1,1),
					POST)
			       ),
		      makeExpr(PLUS,
			       makeExpr(TERM,
					NULL,
					NULL,
					makeTerm(idVar,
						 makeRational(POSITIVE,1,1),
						 NORMAL)
					),
			       makeExpr(TERM,
					NULL,
					NULL,
					makeTerm(-1,
						 makeRational(POSITIVE,1,1),
						 CONSTANT)
					),
			       NULL),
		      NULL
		      );
			  
  loc1->transitions=makeTransition(idLoc2,
				   readyLab,
				   BROADCASTRECEIVER,
				   NULL,
				   expression,
				   loc1->transitions);
  expression=makeExpr(EQU,
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(idVar,
					makeRational(POSITIVE,1,1),
					NORMAL)
			       ),
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(-1,
					makeRational(POSITIVE,count-1,1),
					CONSTANT)
			       ),
		      NULL);
  loc1->transitions->guard=expression;
  /******************************************************/

  /******************************************************/
  expression=makeExpr(EQU,
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(idVar,
					makeRational(POSITIVE,1,1),
					POST)
			       ),
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(-1,
					makeRational(POSITIVE,count-1,1),
					CONSTANT)
			       ),
		      NULL
		      );

  loc2->transitions=makeTransition(idLoc1,
				   notReadyLab,
				   BROADCASTRECEIVER,
				   NULL,
				   expression,
				   loc2->transitions);

  /*********************************************************************************/
 expression=makeExpr(EQU,
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(idVar,
					makeRational(POSITIVE,1,1),
					POST)
			       ),
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(-1,
					makeRational(POSITIVE,1,1),
					CONSTANT)
			       ),
		      NULL
		      );

  loc2->transitions=makeTransition(idLoc3,
				   labId,
				   BROADCASTEMITTER,
				   NULL,
				   expression,
				   loc2->transitions);
  /**********************************************************************/
  expression=makeExpr(EQU,
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(idVar,
					makeRational(POSITIVE,1,1),
					POST)
			       ),
		      makeExpr(PLUS,
			       makeExpr(TERM,
					NULL,
					NULL,
					makeTerm(idVar,
						 makeRational(POSITIVE,1,1),
						 NORMAL)
					),
			       makeExpr(TERM,
					NULL,
					NULL,
					makeTerm(-1,
						 makeRational(POSITIVE,1,1),
						 CONSTANT)
					),
			       NULL),
		      NULL
		      );

  loc3->transitions=makeTransition(idLoc3,
				   labId,
				   BROADCASTEMITTER,
				   NULL,
				   expression,
				   NULL);
  expression=makeExpr(LEQ,
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(idVar,
					makeRational(POSITIVE,1,1),
					NORMAL)
			       ),
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(-1,
					makeRational(POSITIVE,count-2,1),
					CONSTANT)
			       ),
		      NULL);
  loc3->transitions->guard=expression;
  /*********************************************************************/
  expression=makeExpr(EQU,
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(idVar,
					makeRational(POSITIVE,1,1),
					POST)
			       ),
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(-1,
					makeRational(POSITIVE,0,1),
					CONSTANT)
			       ),
		      NULL
		      );

  loc3->transitions=makeTransition(idLoc1,
				   labId,
				   BROADCASTEMITTER,
				   NULL,
				   expression,
				   loc3->transitions);
  expression=makeExpr(EQU,
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(idVar,
					makeRational(POSITIVE,1,1),
					NORMAL)
			       ),
		      makeExpr(TERM,
			       NULL,
			       NULL,
			       makeTerm(-1,
					makeRational(POSITIVE,count-1,1),
					CONSTANT)
			       ),
		      NULL);
  loc3->transitions->guard=expression; 

  /*maintenant, on enregistre les nouveaux noms dans le dictionnaire*/

  labName=getName(labId, syst->idDic);
  tmp=(string) malloc ((strlen(labName)+30)*sizeof(char));
  strcpy(tmp,"multAsapWatcher");
  strcat(tmp,labName);
  t= makeAvlNode(idAutom,tmp,AUTOMATON);
  t->data.automPtr=autom;
  syst->idDic=Insert(t,syst->idDic);

  tmp=(string) malloc ((strlen(labName)+2)*sizeof(char));
  strcpy(tmp,"i");
  strcat(tmp,labName);
  t=makeAvlNode(idVar,tmp,VAR);
  t->data.vType=DISCRETE;
  syst->idDic=Insert(t,syst->idDic);
  
  tmp=(string) malloc ((strlen(labName)+22)*sizeof(char));
  strcpy(tmp,"multAsapWatcher");
  strcat(tmp,labName);
  strcat(tmp,"1");
  t=makeAvlNode(idLoc1,tmp,LOCATION);
  t->data.locPtr=loc1;
  syst->idDic=Insert(t,syst->idDic);

  tmp=(string) malloc ((strlen(labName)+22)*sizeof(char));
  strcpy(tmp,"multAsapWatcher");
  strcat(tmp,labName);
  strcat(tmp,"2");
  t=makeAvlNode(idLoc2,tmp,LOCATION);
  t->data.locPtr=loc2;
  syst->idDic=Insert(t,syst->idDic);
  
  tmp=(string) malloc ((strlen(labName)+22)*sizeof(char));
  strcpy(tmp,"multAsapWatcher");
  strcat(tmp,labName);
  strcat(tmp,"3");
  t=makeAvlNode(idLoc3,tmp,LOCATION);
  t->data.locPtr=loc3;
  syst->idDic=Insert(t,syst->idDic);

  /*insertion de la variable dans les déclarations de variable*/
  syst->varList=makeVar(idVar,syst->varList);
}


void moveInferiorBound(struct system * syst)
{
  struct automaton * autom;
   struct location * locTmp;
  struct location * locTmp2;
  struct transition * transTmp;
  expr e;
  
  for(autom= syst->automList; autom!=NULL;autom=autom->next)
    for(locTmp=autom->locations;locTmp!=NULL;locTmp=locTmp->next)
      {
	e=extractInferiorBounds(&(locTmp->inv));
	if (e)
	  {
	    for(locTmp2=autom->locations;locTmp2!=NULL;locTmp2=locTmp2->next)
	      for(transTmp=locTmp2->transitions;transTmp!=NULL;transTmp=transTmp->next)
		if (transTmp->idDest==locTmp->id)
		  {
		    if (transTmp->guard)
		      transTmp->guard=makeExpr(AND,e,transTmp->guard,NULL);
		    else
		      transTmp->guard=e;
		  }
	  }
      }
}

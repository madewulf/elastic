#include "toWatchers.h"
#include "../utils/labStatus.h"
#include "../utils/utils.h"

#ifdef _DEBUG_
FILE *idFile;
#include "../writeHyTech/writeHyTech.h"
#endif
struct location *generateLocList (expr e, struct location *l,
				  struct system *syst);
void makeEventWatcher (struct system *syst, int idLab, int idGetLab,
		       int idParam);
void makeGuardWatcher (struct system *syst, int idLab, int idLoc,
		       int idInLocLab, int idOutLocLab, int idParam,
		       int idWatcherClock, expr invar);
struct location *splitting (struct automaton *autom, struct location *l,
			    struct system *syst);
expr keepOnlyClockConstraints (expr e, AvlTree idDic);
expr createWatcherGuardInvariant (expr e, int idParam, AvlTree idDic);
void enlargeGuard (expr e, int idParam, AvlTree idDic);
void  createEntryExitLabels(struct automaton * autom, struct system * syst, struct labStatus ** tmpOutLabList, struct labStatus ** tmpInLabList,struct labStatus * tmpGetLabList);
bool testIfLocHasEvent(struct location *,struct automaton *,struct labStatus * tmpGetLabList  );

void
toWatchers (struct system *syst)
{
  int idParam;
  int idClock;
  int idWatcherClock;
  int tmpIdLab;
  int paramCount = 0;
  struct automaton *autom;
  struct lab *labTmp;
  struct location *locTmp;
  struct transition *transTmp;
  struct labStatus *ptrLabStat;
  struct labStatus *tmpGetLabList;
  struct labStatus *tmpInLabList;
  struct labStatus *tmpOutLabList;
 
  string tmp;
  AvlTree t;

  for (autom = syst->automList; autom != NULL; autom = autom->next)
    {
      tmpGetLabList = NULL;
      tmpInLabList = NULL;
      tmpOutLabList = NULL; 
      
      if (autom->type == ELASTIC)
	{
	  /*adding a new parameter to the system */
	  string scount;
	  expr expr1;
	  expr expr2;
	  AvlTree brol;
	  paramCount++;
	  scount = integerToString (paramCount);
	  idParam = getFreshId ();
	  
	  brol=Find(autom->id,syst->idDic);
	  brol->data.automPtr->idParameter=idParam;
	  /*I DONT UNDERSTAND WHY I HAD TO USE BROL*/
	  
	  tmp = (string) malloc ((6 + strlen (scount)) * sizeof (char));
	  strcpy (tmp, "delta");
	  strcat (tmp, scount);
	  t = makeAvlNode (idParam, tmp, VAR);
	  t->data.vType = PARAMETER;
	  syst->idDic = Insert (t, syst->idDic);
	  syst->varList = makeVar (idParam, syst->varList);
	  
	  /*adding a new clock to the system */
	  idClock = getFreshId ();
	  tmp = (string) malloc ((4 + strlen (scount)) * sizeof (char));
	  strcpy (tmp, "ddd");
	  strcat (tmp, scount);
	  t = makeAvlNode (idClock, tmp, VAR);
	  t->data.vType = CLOCK;
	  syst->idDic = Insert (t, syst->idDic);
	  syst->varList = makeVar (idClock, syst->varList);
	  
	  /*adding a new clock to the system for the guard watchers */
	  /*only if there is at least one input event*/
	  for (labTmp=autom->labList;labTmp!=NULL && labTmp->type!=EVENT;labTmp=labTmp->next)
	    ;
	  if (labTmp!=NULL)
	    {
	      idWatcherClock = getFreshId ();
	      tmp = (string) malloc ((4 + strlen (scount)) * sizeof (char));
	      strcpy (tmp, "uuu");
	      strcat (tmp, scount);
	      t = makeAvlNode (idWatcherClock, tmp, VAR);
	      t->data.vType = CLOCK;
	      syst->idDic = Insert (t, syst->idDic);
	      syst->varList = makeVar (idWatcherClock, syst->varList);
	    }
	  /*creating the initialization expr */
	  expr1 = makeExpr (EQU,
			    makeExpr (TERM,
				      NULL,
				      NULL,
				      makeTerm (idClock,
						makeRational
						(POSITIVE, 1,
						 1), NORMAL)),
			    makeExpr (TERM, NULL, NULL,
				      makeTerm (-1,
						makeRational
						(POSITIVE, 0,
						 1),
						CONSTANT)),
			    NULL);
	  expr2 =
	    makeExpr (EQU,
		      makeExpr (TERM, NULL, NULL,
				makeTerm (idWatcherClock,
					  makeRational (POSITIVE, 1, 1),
					  NORMAL)), makeExpr (TERM, NULL,
							      NULL,
							      makeTerm (-1,
									makeRational
									(POSITIVE,
									 0,
									 1),
									CONSTANT)),
		      NULL);
	  if (autom->initialisation)
	    autom->initialisation =
	      makeExpr (AND, autom->initialisation, makeExpr(AND,expr1,expr2,NULL), NULL);
	  else
	    autom->initialisation = makeExpr(AND,expr1,expr2,NULL);
	  
	  /*creating the new "event perceived" labels */
	  for (labTmp = autom->labList; labTmp != NULL; labTmp = labTmp->next)
	    {
	      int idLab;
	      int idGetLab;
	      idLab = labTmp->id;
	      idGetLab = idLab;
	      if (labTmp->type == EVENT)
		{
		  string labName;
		  AvlTree t2;
		  
		  labName = getName (idLab, syst->idDic);
		  idGetLab=getStatus(idLab,syst->viewList);
		  if (idGetLab==-1)
		    {
		      idGetLab= getFreshId ();
		      
		      tmp =
			(string) malloc (sizeof (char) * (strlen (labName) + 5));
		      strcpy (tmp, "get");
		      strcat (tmp, labName);
		      t2 = makeAvlNode (idGetLab, tmp, LABEL);
		      t2->data.labPtr = labTmp;
		      syst->idDic = Insert (t2, syst->idDic);
		      t2=Find(idLab,syst->idDic);
		      t2->data.labPtr= makeLabel(idLab, STANDARD,NULL);
		    }
		  /*updating the label list */
		  labTmp->id = idGetLab;
		  
		  /*creating event-watcher */
		  makeEventWatcher (syst, idLab, idGetLab, idParam);
		  
		}
	      tmpGetLabList = makeLabStatus (idLab, idGetLab, tmpGetLabList);
	    }
	  tmpGetLabList=makeLabStatus(-1,-1,tmpGetLabList);
	  
	  /*creating the new entry and exit label for each location */
	  createEntryExitLabels(autom,syst, &tmpOutLabList, &tmpInLabList,tmpGetLabList);
	  
	  /*adding guard watchers and transforming the automaton */
	  for (locTmp = autom->locations; locTmp != NULL;
	       locTmp = locTmp->next)
	    {
	      string locName;
	      string stmp;
	      expr expression;
	      expr guardDisj = NULL;
	      bool  orderFlag;
	      bool eventFlag;
	      /*id's for the locations */
	      int idLoc;
	      int idLab;
	      int idPreInLoc;
	      int idInLoc;
	      int cptOutLoc;
	      AvlTree t;
	      
	      eventFlag=testIfLocHasEvent(locTmp ,autom, tmpGetLabList );
	      
	      idLoc = locTmp->id;
	      t = Find (idLoc, syst->idDic);
	      locName = t->name;

	      
	      if (!eventFlag)
		{
		  idInLoc = idLoc;	/*to avoid changing the destination of a lot of transitions */
		  idLoc=getFreshId();
	      /*creation of the new in */
		  
		  expression = makeExpr (LEQ,
					 makeExpr (TERM,
						   NULL,
						   NULL,
						   makeTerm (idClock,
							     makeRational
							     (POSITIVE, 1, 1),
							     NORMAL)),
					 makeExpr (TERM, NULL, NULL,
						   makeTerm (idParam,
							     makeRational
							     (POSITIVE, 1, 1),
							     NORMAL)), NULL);
		  autom->locations =
		    makeLocation (idInLoc, expression, NULL, NULL,
				  autom->locations);
		  t->data.locPtr = autom->locations;
		  
		  autom->locations->transitions = makeTransition (idLoc,
								  -1,
								  ORDINARY,
								  NULL,
								  NULL,
								  autom->
								  locations->
								  transitions);
		  
		  stmp = (string) malloc ((strlen (locName) + 3) * sizeof (char));
		  strcpy (stmp, "In");
		  strcat (stmp, locName);
		  t->name=stmp;
		  t->data.locPtr=autom->locations;
		  
		  t = makeAvlNode (idLoc, locName, LOCATION);
		  syst->idDic = Insert (t, syst->idDic);
		  t->data.locPtr=locTmp;
		  
		  locTmp->id = idLoc;
		}
	      else
		{		
		  idPreInLoc = idLoc;	/*to avoid changing the destination of a lot of transitions */
		  idLoc = getFreshId();
		  idInLoc=getFreshId();
	      	      
		  /*creation of the new pre_in */
		  stmp = (string) malloc ((strlen (locName) + 6) * sizeof (char));
		  strcpy (stmp, "PreIn");
		  strcat (stmp, locName);
		  t->name = stmp;
		  expression = makeExpr (LEQ,
					 makeExpr (TERM,
						   NULL,
						   NULL,
						   makeTerm (idClock,
							     makeRational
							     (POSITIVE, 1, 1),
							     NORMAL)),
					 makeExpr (TERM, NULL, NULL,
						   makeTerm (-1,
							     makeRational
							     (POSITIVE, 0, 1),
							     CONSTANT)), NULL);
		  autom->locations =
		    makeLocation (idPreInLoc, expression, NULL, NULL,
				  autom->locations);
		  
		  expression = makeExpr (EQU,
					 makeExpr (TERM,
						   NULL,
						   NULL,
						   makeTerm (idClock,
							     makeRational
							     (POSITIVE, 1, 1),
							     POST)),
					 makeExpr (TERM, NULL, NULL,
						   makeTerm (-1,
							     makeRational
							     (POSITIVE, 0, 1),
							     CONSTANT)), NULL);
		  autom->locations->transitions =
		    makeTransition (idInLoc, getStatus (idPreInLoc, tmpInLabList),
				    ORDINARY, NULL, expression,
				    autom->locations->transitions);
		  t->data.locPtr = autom->locations;
		  /*creation of the new in */
		  stmp = (string) malloc ((strlen (locName) + 3) * sizeof (char));
		  strcpy (stmp, "In");
	      strcat (stmp, locName);
	      t = makeAvlNode (idInLoc, stmp, LOCATION);
	      syst->idDic = Insert (t, syst->idDic);
	      
	      expression = makeExpr (LEQ,
				     makeExpr (TERM,
					       NULL,
					       NULL,
					       makeTerm (idClock,
							 makeRational
							 (POSITIVE, 1, 1),
							 NORMAL)),
				     makeExpr (TERM, NULL, NULL,
					       makeTerm (idParam,
							 makeRational
							 (POSITIVE, 1, 1),
							 NORMAL)), NULL);
	      autom->locations =
		makeLocation (idInLoc, expression, NULL, NULL,
			      autom->locations);
	      t->data.locPtr = autom->locations;
	      
	      autom->locations->transitions = makeTransition (idLoc,
							      -1,
							      ORDINARY,
							      NULL,
							      NULL,
							      autom->
							      locations->
							      transitions);
	      /*----------------------*/
	      
	      locTmp->id = idLoc;
	      t = makeAvlNode (idLoc, locName, LOCATION);
	      syst->idDic = Insert (t, syst->idDic);
		}
	      
	      

	      /*----------------------*/
	      
	      cptOutLoc = 1;
	      orderFlag=0;
	      for (ptrLabStat = tmpGetLabList; ptrLabStat != NULL;
		   ptrLabStat = ptrLabStat->next)
		{
		  int labPresentFlag = 0;
		  idLab = ptrLabStat->labId;
		  guardDisj = NULL;
		  
		  for (transTmp = locTmp->transitions; transTmp != NULL;
		       transTmp = transTmp->next)
		    {
		      int idOutLoc;

		      if (transTmp->idLabel == idLab)
			{
			  labPresentFlag = 1;
			  /*collecting the guards into one big disjunction*/
			  if (!guardDisj)
			    guardDisj =
			      keepOnlyClockConstraints (copyExpression
							(transTmp->guard),
							syst->idDic);
			  else
			    if (keepOnlyClockConstraints
				(copyExpression (transTmp->guard),
				 syst->idDic) != NULL)
			      guardDisj =
				makeExpr (OR, guardDisj,
					  keepOnlyClockConstraints
					  (copyExpression (transTmp->guard),
					   syst->idDic), NULL);
			  
			  expression = makeExpr (EQU,
						 makeExpr (TERM,
							   NULL,
							   NULL,
							   makeTerm (idClock,
								     makeRational
								     (POSITIVE,
								      1, 1),
								     POST)),
						 makeExpr (TERM, NULL, NULL,
							   makeTerm (-1,
								     makeRational
								     (POSITIVE,
								      0, 1),
								     CONSTANT)),
						 NULL);
			  expression = makeExpr (AND, 
						 makeExpr(EQU,
							  makeExpr (TERM,
								    NULL,
								    NULL,
								    makeTerm (idWatcherClock,
									      makeRational
									      (POSITIVE,
									       1, 1),
									      POST)),
							  makeExpr (TERM, NULL, NULL,
								    makeTerm (-1,
									      makeRational
									      (POSITIVE,
									       0, 1),
									      CONSTANT)),
							  NULL),
						 expression,
						 NULL);
;
			  			  
			  if (transTmp->update)
			    transTmp->update =
			      makeExpr (AND, expression, transTmp->update,
					NULL);
			  else
			    transTmp->update = expression;
			  if (eventFlag)
			    {
			      /*creation of the output location */
			      idOutLoc = getFreshId ();
			      
			      
			      scount = integerToString (cptOutLoc);
			      cptOutLoc++;
			      stmp =
				(string)
				malloc ((strlen (locName) + strlen (scount) +
					 7) * sizeof (char));
			      strcpy (stmp, "Post");
			      strcat (stmp, locName);
			      strcat (stmp, "Nr");
			      strcat (stmp, scount);
			      t = makeAvlNode (idOutLoc, stmp, LOCATION);
			      syst->idDic = Insert (t, syst->idDic);
			      
			      expression = makeExpr (LEQ,
						     makeExpr (TERM,
							       NULL,
							       NULL,
							       makeTerm (idClock,
									 makeRational
									 (POSITIVE,
									  1, 1),
									 NORMAL)),
						     makeExpr (TERM, NULL, NULL,
							       makeTerm (-1,
									 makeRational
									 (POSITIVE,
									  0, 1),
									 CONSTANT)),
						     NULL);
			      autom->locations =
				makeLocation (idOutLoc, expression, NULL, NULL,
					      autom->locations);
			      t->data.locPtr = autom->locations;
			      expression = makeExpr (EQU,
						     makeExpr (TERM,
							       NULL,
							       NULL,
							       makeTerm (idClock,
									 makeRational
									 (POSITIVE,
									  1, 1),
									 POST)),
						     makeExpr (TERM, NULL, NULL,
							       makeTerm (-1,
									 makeRational
									 (POSITIVE,
									  0, 1),
									 CONSTANT)),
						     NULL);
			      autom->locations->transitions =
				makeTransition (transTmp->idDest,
						getStatus (idPreInLoc,
							   tmpOutLabList),
						ORDINARY, NULL, expression,
						autom->locations->transitions);
			      
			      transTmp->idDest = idOutLoc;
			    }
			  transTmp->idLabel =
			    getStatus (transTmp->idLabel, tmpGetLabList);
			}
		    }
		  tmpIdLab = getStatus (idLab, tmpGetLabList);

		  for (labTmp = autom->labList;
		       labTmp != NULL && labTmp->id != tmpIdLab;
		       labTmp = labTmp->next)
		    ;
		  if (labTmp && labTmp->type == EVENT)
		    {
		      if (guardDisj)
			makeGuardWatcher (syst,
					  getStatus (idLab, tmpGetLabList),
					  idLoc, getStatus (idPreInLoc,
							    tmpInLabList),
					  getStatus (idPreInLoc,
						     tmpOutLabList), idParam,
					  idWatcherClock, guardDisj);
		      /* else if (labPresentFlag == 1) */
/* 			makeGuardWatcher (syst, getStatus (idLab, tmpGetLabList), idLoc, getStatus (idPreInLoc, tmpInLabList), getStatus (idPreInLoc, tmpOutLabList), idParam, idWatcherClock, makeExpr (FALSE, NULL, NULL, NULL));  */
		    }
		  else
		    {
		      if (guardDisj)
			{
			  if (locTmp->inv)
			    locTmp->inv = makeExpr (AND,
						    locTmp->inv,
						    createWatcherGuardInvariant
						    (guardDisj, idParam,
						     syst->idDic), NULL);
			  else
			    locTmp->inv =
			      createWatcherGuardInvariant (guardDisj, idParam,
							   syst->idDic);
			}
		      if (labPresentFlag==1) 
			orderFlag=1;
		    }
		}
	      /*enlarge all guards ! */
	      for (transTmp = locTmp->transitions; transTmp != NULL;
		   transTmp = transTmp->next)
		enlargeGuard (transTmp->guard, idParam, syst->idDic);

	      /*rajouter un test sur les event ici*/
	      if (orderFlag==1)
		{
		  expression = makeExpr (LEQ,
					 makeExpr (TERM,
						   NULL,
						   NULL,
						   makeTerm (idClock,
							     makeRational
							     (POSITIVE, 1, 1),
							     NORMAL)),
					 makeExpr (TERM, NULL, NULL,
						   makeTerm (idParam,
							     makeRational
							     (POSITIVE, 1, 1),
							     NORMAL)), NULL);
		  if (locTmp->inv)
		    locTmp->inv = makeExpr (OR, expression, locTmp->inv, NULL);
		  else
		    locTmp->inv = expression;
		}
	      locTmp->inv = DNF (locTmp->inv);
	      
	      locTmp = splitting (autom, locTmp, syst);
	    }
	  autom->type = HYTECH;
	}
    }
  /*transforming every label in a STANDARD ones since automatons are all hytech automatons now*/
    for (autom = syst->automList; autom != NULL; autom = autom->next)
      for(labTmp=autom->labList;labTmp!=NULL;labTmp=labTmp->next)
	if (labTmp->type==EVENT || labTmp->type==INTERNAL || labTmp->type==ORDER)
	  labTmp->type=STANDARD;
}

bool testIfLocHasEvent(struct location * locTmp ,struct automaton * autom ,struct labStatus * tmpGetLabList)
{
  struct transition *transTmp;
  struct lab * labTmp;
  bool eventFlag;
  eventFlag=0;
  
  for(transTmp=locTmp->transitions;transTmp!=NULL && !eventFlag;transTmp=transTmp->next)
    {
      if (transTmp->guard && transTmp->guard->type!=TRUE)
	{
	  for(labTmp=autom->labList;(labTmp!=NULL) && (labTmp->id!=getStatus(transTmp->idLabel,tmpGetLabList));labTmp=labTmp->next)
	    ;
	  if (labTmp!=NULL && labTmp->type==EVENT)
	    eventFlag=1;
	}
    }  
  return eventFlag;
}


void  createEntryExitLabels(struct automaton * autom, struct system * syst, struct labStatus ** tmpOutLabList, struct labStatus ** tmpInLabList,struct labStatus * tmpGetLabList)
{ 
  struct location * locTmp;
  AvlTree t;
  string tmp;
  for (locTmp = autom->locations; locTmp != NULL;
       locTmp = locTmp->next)
    {
      if (testIfLocHasEvent(locTmp ,autom,tmpGetLabList ))
	{
	  int idLoc;
	  string locName;
	  int idInLocLab;
	  int idOutLocLab;
	  AvlTree t2;
	  idLoc = locTmp->id;
	  t = Find (idLoc, syst->idDic);
	  
	  locName = t->name;
	  /*creating new entry  label for the location */
	  idInLocLab = getFreshId ();
	  tmp = (string) malloc (sizeof (char) * (strlen (locName) + 3));
	  strcpy (tmp, "in");
	  strcat (tmp, locName);
	  t2 = makeAvlNode (idInLocLab, tmp, LABEL);
	  
	  syst->idDic = Insert (t2, syst->idDic);
	  autom->labList =
	    makeLabel (idInLocLab, STANDARD, autom->labList);
	  t2->data.labPtr = autom->labList;
	  
	  *tmpInLabList = makeLabStatus (idLoc, idInLocLab, *tmpInLabList);

	  /*creating new exit  label for the location */
	  idOutLocLab = getFreshId ();
	  tmp = (string) malloc (sizeof (char) * (strlen (locName) + 4));
	  strcpy (tmp, "out");
	  strcat (tmp, locName);
	  t2 = makeAvlNode (idOutLocLab, tmp, LABEL);
	  
	  syst->idDic = Insert (t2, syst->idDic);
	  autom->labList =
	    makeLabel (idOutLocLab, STANDARD, autom->labList);
	  t2->data.labPtr = autom->labList;
	  
	  *tmpOutLabList =
	    makeLabStatus (idLoc, idOutLocLab, *tmpOutLabList);
	}
    }
}

bool
containsVType (expr e, AvlTree idDic, enum varType vType)
     /*return true if the expression contains a variable of type vType */
{
  AvlTree t;
  if (e)
    {
      if (e->type == TERM)
	{
	  if (e->te->status != CONSTANT)
	    {
	      t = Find (e->te->id, idDic);
	      return (t->data.vType == vType);
	    }
	  else
	    return (0);
	}
      else
	return (containsVType (e->ls, idDic, vType)
		|| containsVType (e->rs, idDic, vType));
    }
  else
    return (0);
}

void
enlargeGuard (expr e, int idParam, AvlTree idDic)
{
  if (e)
    {
      switch (e->type)
	{
	case LEQ:
	  {
	    if (containsVType (e->ls, idDic, CLOCK))
	      e->rs = makeExpr (PLUS,
				e->rs,
				makeExpr (TERM,
					  NULL,
					  NULL,
					  makeTerm (idParam,
						    makeRational (POSITIVE, 1,
								  1),
						    NORMAL)), NULL);

	    else if (containsVType (e->rs, idDic, CLOCK))
	      e->ls = makeExpr (MINUS,
				e->ls,
				makeExpr (TERM,
					  NULL,
					  NULL,
					  makeTerm (idParam,
						    makeRational (POSITIVE, 1,
								  1),
						    NORMAL)), NULL);

	    break;
	  }
	case GEQ:
	  {
	    if (containsVType (e->ls, idDic, CLOCK))
	      e->rs = makeExpr (MINUS,
				e->rs,
				makeExpr (TERM,
					  NULL,
					  NULL,
					  makeTerm (idParam,
						    makeRational (POSITIVE, 1,
								  1),
						    NORMAL)), NULL);

	    else if (containsVType (e->rs, idDic, CLOCK))
	      e->ls = makeExpr (PLUS,
				e->ls,
				makeExpr (TERM,
					  NULL,
					  NULL,
					  makeTerm (idParam,
						    makeRational (POSITIVE, 1,
								  1),
						    NORMAL)), NULL);

	    break;
	  }
	case AND:
	case OR:
	  {
	    enlargeGuard (e->ls, idParam, idDic);
	    enlargeGuard (e->rs, idParam, idDic);
	  }
	default:
	  break;
	}
    }
}

expr
keepOnlyClockConstraints (expr e, AvlTree idDic)
{
  if (e)
    {
      switch (e->type)
	{
	case AND:
	  {
	    if (containsVType (e->ls, idDic, CLOCK))
	      {
		if (containsVType (e->rs, idDic, CLOCK))
		  {
		    e->ls = keepOnlyClockConstraints (e->ls, idDic);
		    e->rs = keepOnlyClockConstraints (e->rs, idDic);
		    return e;
		  }
		else
		  {
		    return keepOnlyClockConstraints (e->ls, idDic);
		  }
	      }
	    else
	      {
		if (containsVType (e->rs, idDic, CLOCK))
		  return keepOnlyClockConstraints (e->rs, idDic);
		break;
	      }
	default:
	    {
	      if (containsVType (e, idDic, CLOCK))
		return e;
	      break;
	    }

	  }

	}
    }
  return NULL;
}

expr
createWatcherGuardInvariant (expr e, int idParam, AvlTree idDic)
{
  if (e)
    {
      switch (e->type)
	{
	case OR:
	  {
	    e->type = AND;
	    createWatcherGuardInvariant (e->ls, idParam, idDic);
	    createWatcherGuardInvariant (e->rs, idParam, idDic);
	    break;
	  }
	case AND:
	  {
	    e->type = OR;
	    createWatcherGuardInvariant (e->ls, idParam, idDic);
	    createWatcherGuardInvariant (e->rs, idParam, idDic);
	    break;
	  }
	case LEQ:
	  {
	    if (containsVType (e->ls, idDic, CLOCK))
	      e->type = GEQ;
	    else if (containsVType (e->rs, idDic, CLOCK))
	      {
		e->ls = makeExpr (PLUS,
				  e->ls,
				  makeExpr (TERM,
					    NULL,
					    NULL,
					    makeTerm (idParam,
						      makeRational (POSITIVE,
								    1, 1),
						      NORMAL)), NULL);
		e->type = GEQ;
	      }
	    break;
	  }
	case GEQ:
	  {
	    if (containsVType (e->ls, idDic, CLOCK))
	      {
		e->rs = makeExpr (PLUS,
				  e->rs,
				  makeExpr (TERM,
					    NULL,
					    NULL,
					    makeTerm (idParam,
						      makeRational (POSITIVE,
								    1, 1),
						      NORMAL)), NULL);
		e->type = LEQ;
	      }

	    else if (containsVType (e->rs, idDic, CLOCK))
	      e->type = LEQ;
	    break;
	  }
	default:
	  break;
	}
      e = DNF (e);
    }
  return e;
}

void
makeGuardWatcher (struct system *syst, int idLab, int idLoc,
		  int idInLocLab, int idOutLocLab, int idParam,
		  int idWatcherClock, expr invar)
{
  expr expression;
  int idAutom;
  int idLoc1;
  int idLoc2;
  int idLoc3;
  int idLoc4;
  AvlTree t;
  string labName;
  string locName;
  string tmp;

  struct automaton *autom;
  struct location *loc1;
  struct location *loc2;
  struct location *loc3;
  struct location *loc4;
  struct lab *labTmp;

  idAutom = getFreshId ();
  idLoc1 = getFreshId ();
  idLoc2 = getFreshId ();
  idLoc3 = getFreshId ();
  idLoc4 = getFreshId ();

  
  /*insertion of the new locations */
  /*only one invariant on loc2 */


  expression = makeExpr (LEQ,
			 makeExpr (TERM,
				   NULL,
				   NULL,
				   makeTerm (idWatcherClock,
					     makeRational (POSITIVE, 1, 1),
					     NORMAL)),
			 makeExpr (TERM,
				   NULL,
				   NULL,
				   makeTerm (-1,
					     makeRational (POSITIVE, 0, 1),
					     CONSTANT)), NULL);
  loc4 = makeLocation (idLoc4, expression, NULL, NULL, NULL);
  loc3 = makeLocation (idLoc3,
		       createWatcherGuardInvariant (invar, idParam,
						    syst->idDic), NULL, NULL,
		       loc4);
  loc2 = makeLocation (idLoc2, NULL, NULL, NULL, loc3);
  loc1 = makeLocation (idLoc1, NULL, NULL, NULL, loc2);

  /*creation of the transitions */
  /*no guards, only one update */
  expression = makeExpr (EQU,
			 makeExpr (TERM,
				   NULL,
				   NULL,
				   makeTerm (idWatcherClock,
					     makeRational (POSITIVE, 1, 1),
					     POST)),
			 makeExpr (TERM,
				   NULL,
				   NULL,
				   makeTerm (-1,
					     makeRational (POSITIVE, 0, 1),
					     CONSTANT)), NULL);
  loc1->transitions = makeTransition (idLoc1,
				      idLab, ORDINARY, NULL, NULL, NULL);

  loc1->transitions = makeTransition (idLoc2,
				      idInLocLab,
				      ORDINARY,
				      NULL, NULL, loc1->transitions);
  loc2->transitions = makeTransition (idLoc3, -1, ORDINARY, NULL, NULL, NULL);
  loc2->transitions = makeTransition (idLoc4,
				      idLab,
				      ASAP,
				      NULL, expression, loc2->transitions);
  loc2->transitions = makeTransition (idLoc1,
				      idOutLocLab,
				      ORDINARY,
				      NULL, NULL, loc2->transitions);
  loc3->transitions = makeTransition (idLoc2, -1, ORDINARY, NULL, NULL, NULL);
  loc4->transitions = makeTransition (idLoc1,
				      idOutLocLab,
				      ORDINARY, NULL, NULL, NULL);

  /*insertion of the new automaton in the system */
  /*creation of the label list */
  labTmp =
    makeLabel (idLab, STANDARD,
	       makeLabel (idInLocLab, STANDARD,
			  makeLabel (idOutLocLab, STANDARD, NULL)));
  autom =
    makeAutomaton (idAutom, HYTECH, NULL, idLoc1, labTmp, loc1,
		   syst->automList);
  syst->automList = autom;

  labName = getName (idLab, syst->idDic);
  locName = getName (idLoc, syst->idDic);

  tmp =
    (string) malloc ((strlen (labName) + strlen (locName) + 4) *
		     sizeof (char));
  strcpy (tmp, "GW");
  strcat (tmp, locName);
  strcat (tmp, labName);
  t = makeAvlNode (idAutom, tmp, AUTOMATON);
  t->data.automPtr = autom;
  syst->idDic = Insert (t, syst->idDic);

  tmp =
    (string) malloc ((strlen (labName) + strlen (locName) + 5) *
		     sizeof (char));
  strcpy (tmp, "GW");
  strcat (tmp, locName);
  strcat (tmp, labName);
  strcat (tmp, "0");
  t = makeAvlNode (idLoc1, tmp, LOCATION);
  t->data.locPtr = loc1;
  syst->idDic = Insert (t, syst->idDic);

  tmp =
    (string) malloc ((strlen (labName) + strlen (locName) + 5) *
		     sizeof (char));
  strcpy (tmp, "GW");
  strcat (tmp, locName);
  strcat (tmp, labName);
  strcat (tmp, "1");
  t = makeAvlNode (idLoc2, tmp, LOCATION);
  t->data.locPtr = loc2;
  syst->idDic = Insert (t, syst->idDic);

  tmp =
    (string) malloc ((strlen (labName) + strlen (locName) + 5) *
		     sizeof (char));
  strcpy (tmp, "GW");
  strcat (tmp, locName);
  strcat (tmp, labName);
  strcat (tmp, "2");
  t = makeAvlNode (idLoc3, tmp, LOCATION);
  t->data.locPtr = loc3;
  syst->idDic = Insert (t, syst->idDic);

  tmp =
    (string) malloc ((strlen (labName) + strlen (locName) + 6) *
		     sizeof (char));
  strcpy (tmp, "GW");
  strcat (tmp, locName);
  strcat (tmp, labName);
  strcat (tmp, "3");
  t = makeAvlNode (idLoc4, tmp, LOCATION);
  t->data.locPtr = loc4;
  syst->idDic = Insert (t, syst->idDic);

  splitting (autom, loc3, syst);

}

void
makeEventWatcher (struct system *syst, int idLab, int idGetLab, int idParam)
{
  expr expression;
  int idAutom;
  int idLoc1;
  int idLoc2;
  int idLoc3;
  int idLoc4;
  int idVar;

  struct automaton *autom;
  struct location *loc1;
  struct location *loc2;
  struct location *loc3;
  struct location *loc4;
  struct lab *labTmp;
  AvlTree t;
  string labName;
  string tmp;

  idAutom = getFreshId ();
  idLoc1 = getFreshId ();
  idLoc2 = getFreshId ();
  idLoc3 = getFreshId ();
  idLoc4 = getFreshId () ;
  idVar = getFreshId  ();

  
  /*insertion of the new locations */
  /*only one invariant on loc2 */
  expression = makeExpr (LEQ,
			 makeExpr (TERM,
				   NULL,
				   NULL,
				   makeTerm (idVar,
					     makeRational (POSITIVE, 1, 1),
					     NORMAL)),
			 makeExpr (TERM,
				   NULL,
				   NULL,
				   makeTerm (idParam,
					     makeRational (POSITIVE, 1, 1),
					     NORMAL)), NULL);
  if (syst->receptivenessChecking)
    {
      loc4 = makeLocation(idLoc4,NULL, NULL,NULL,NULL);
      loc4->inv=makeExpr (LEQ,
			  makeExpr (TERM,
				    NULL,
				    NULL,
				    makeTerm (idVar,
					     makeRational (POSITIVE, 1, 1),
					      NORMAL)),
			  makeExpr (TERM,
				    NULL,
				    NULL,
				    makeTerm (-1,
					      makeRational (POSITIVE, 0, 1),
					      CONSTANT)), NULL);
      loc3 = makeLocation (idLoc3, NULL, NULL, NULL, loc4);
    }
  else
    loc3 = makeLocation (idLoc3, NULL, NULL, NULL, NULL);
  
  loc2 = makeLocation (idLoc2, expression, NULL, NULL, loc3);
  loc1 = makeLocation (idLoc1, NULL, NULL, NULL, loc2);
  
  /*creation of the transitions */
  /*no guards, only one update */
  expression = makeExpr (EQU,
			 makeExpr (TERM,
				   NULL,
				   NULL,
				   makeTerm (idVar,
					     makeRational (POSITIVE, 1, 1),
					     POST)),
			 makeExpr (TERM,
				   NULL,
				   NULL,
				   makeTerm (-1,
					     makeRational (POSITIVE, 0, 1),
					     CONSTANT)), NULL);
  loc1->transitions = makeTransition (idLoc2,
				      idLab,
				      ORDINARY, NULL, expression, NULL);
  if (syst->receptivenessChecking)
    {
      expression =  makeExpr (EQU,
			 makeExpr (TERM,
				   NULL,
				   NULL,
				   makeTerm (idVar,
					     makeRational (POSITIVE, 1, 1),
					     POST)),
			 makeExpr (TERM,
				   NULL,
				   NULL,
				   makeTerm (-1,
					     makeRational (POSITIVE, 0, 1),
					     CONSTANT)), NULL);
    loc2->transitions = makeTransition (idLoc4,
					idLab, ORDINARY, NULL, expression, NULL);
    }
  else  
    loc2->transitions = makeTransition (idLoc2,
					idLab, ORDINARY, NULL, NULL, NULL);
  loc2->transitions = makeTransition (idLoc3,
				      -1,
				      ORDINARY,
				      NULL, NULL, loc2->transitions);
  if (syst->receptivenessChecking)
 {
      expression =  makeExpr (EQU,
			 makeExpr (TERM,
				   NULL,
				   NULL,
				   makeTerm (idVar,
					     makeRational (POSITIVE, 1, 1),
					     POST)),
			 makeExpr (TERM,
				   NULL,
				   NULL,
				   makeTerm (-1,
					     makeRational (POSITIVE, 0, 1),
					     CONSTANT)), NULL);

    loc3->transitions = makeTransition (idLoc4,
					idLab, ORDINARY, NULL, expression, NULL);
 }
  else
    loc3->transitions = makeTransition (idLoc3,
					idLab, ORDINARY, NULL, NULL, NULL);
  loc3->transitions = makeTransition (idLoc1,
				      idGetLab,
				      ASAP, NULL, NULL, loc3->transitions);

  /*insertion of the new automaton in the system */
  /*creation of the label list */
  labTmp = makeLabel (idLab, STANDARD, makeLabel (idGetLab, STANDARD, NULL));
  autom =
    makeAutomaton (idAutom, HYTECH, NULL, idLoc1, labTmp, loc1,
		   syst->automList);
  syst->automList = autom;
  /*initialisation expression */
  autom->initialisation = makeExpr (EQU,
				    makeExpr (TERM,
					      NULL,
					      NULL,
					      makeTerm (idVar,
							makeRational
							(POSITIVE, 1, 1),
							NORMAL)),
				    makeExpr (TERM, NULL, NULL,
					      makeTerm (-1,
							makeRational
							(POSITIVE, 0, 1),
							CONSTANT)), NULL);

  /*registering of the new names in the system dictionnary */
  labName = getName (idLab, syst->idDic);
  tmp = (string) malloc ((strlen (labName) + 4) * sizeof (char));
  strcpy (tmp, "EW");
  strcat (tmp, labName);
  t = makeAvlNode (idAutom, tmp, AUTOMATON);
  t->data.automPtr = autom;
  syst->idDic = Insert (t, syst->idDic);

  tmp = (string) malloc ((strlen (labName) + 3) * sizeof (char));
  strcpy (tmp, "z");
  strcat (tmp, labName);
  t = makeAvlNode (idVar, tmp, VAR);
  t->data.vType = CLOCK;
  syst->idDic = Insert (t, syst->idDic);

  tmp = (string) malloc ((strlen (labName) + 5) * sizeof (char));
  strcpy (tmp, "EW");
  strcat (tmp, labName);
  strcat (tmp, "0");
  t = makeAvlNode (idLoc1, tmp, LOCATION);
  t->data.locPtr = loc1;
  syst->idDic = Insert (t, syst->idDic);

  tmp = (string) malloc ((strlen (labName) + 5) * sizeof (char));
  strcpy (tmp, "EW");
  strcat (tmp, labName);
  strcat (tmp, "1");
  t = makeAvlNode (idLoc2, tmp, LOCATION);
  t->data.locPtr = loc2;
  syst->idDic = Insert (t, syst->idDic);

  tmp = (string) malloc ((strlen (labName) + 5) * sizeof (char));
  strcpy (tmp, "EW");
  strcat (tmp, labName);
  strcat (tmp, "2");
  t = makeAvlNode (idLoc3, tmp, LOCATION);
  t->data.locPtr = loc3;
  syst->idDic = Insert (t, syst->idDic);
  if (syst->receptivenessChecking)
    {
      tmp = (string) malloc ((strlen (labName) + 6) * sizeof (char));
      strcpy (tmp, "EW");
      strcat (tmp, labName);
      strcat (tmp, "ERR");
      t = makeAvlNode (idLoc4, tmp, LOCATION);
      t->data.locPtr = loc4;
      syst->idDic = Insert (t, syst->idDic);
      expression = makeExpr(LOCINIT,NULL,NULL,makeTerm(idAutom,NULL,idLoc4));
      if (syst->badStates)
	syst->badStates = makeExpr(OR,syst->badStates, expression,NULL);
      else
	syst->badStates = expression;

    }

  /*insertion of the new variable in the system variables list */
  syst->varList = makeVar (idVar, syst->varList);
}

struct location *
splitting (struct automaton *autom, struct location *l, struct system *syst)
{
  struct location *locList = NULL;
  struct location *locTmp;
  struct location *locTmp2;
  struct transition *transTmp;
  struct location *res;
  res = l;
  /*the new locations are included BEFORE l this  is important for the correctness of toWatchers */
  /*to avoid useless work */
 if (l && l->inv) 
     simplifyDisjunction(&(l->inv),&(l->inv)); 
  if (l && l->inv && l->inv->type == OR)
    locList = generateLocList (l->inv, l, syst);
  if (locList)
    {
      /*add the transitions between the splitted locations */
      for (locTmp = locList; locTmp != NULL; locTmp = locTmp->next)
	for (locTmp2 = locList; locTmp2 != NULL; locTmp2 = locTmp2->next)
	  if (locTmp != locTmp2)
	    locTmp->transitions = makeTransition (locTmp2->id,
						  -1,
						  ORDINARY,
						  NULL,
						  NULL, locTmp->transitions);
      /*replace the transition to the initial location by transitions to each one of 
         the splitted locations */
      for (locTmp = autom->locations; locTmp != NULL; locTmp = locTmp->next)
	for (transTmp = locTmp->transitions; transTmp != NULL;
	     transTmp = transTmp->next)
	  {
	    if (transTmp->idDest == l->id)
	      {
		struct transition *transTmp2;
		int flag = 0;
		transTmp->idDest = locList->id;
		transTmp2 = transTmp;
		for (locTmp2 = locList->next; locTmp2 != NULL;
		     locTmp2 = locTmp2->next)
		  {
		    transTmp->next = makeTransition (locTmp2->id,
						     transTmp->idLabel,
						     transTmp->ts,
						     copyExpression
						     (transTmp->guard),
						     copyExpression
						     (transTmp->update),
						     transTmp->next);

		    if (flag == 0)
		      {
			transTmp2 = transTmp->next;
			flag = 1;
		      }
		  }
		transTmp = transTmp2;
	      }
	  }

      for (locTmp = locList; locTmp->next != NULL; locTmp = locTmp->next)
	;
      locTmp->next = l->next;
      res = locTmp;
      l->next = locList;
      if (autom->locations == l)
	autom->locations = l->next;
      else
	{
	  for (locTmp = autom->locations; locTmp->next != l;
	       locTmp = locTmp->next)
	    ;
	  locTmp->next = locTmp->next->next;
	}



    }
  return res;
}

struct location *
generateLocList (expr e, struct location *l, struct system *syst)
{
  /*function generating a list of "splitting" locations from a location with an invariant in DNF */
  struct location *res = NULL;
  if (e)
    switch (e->type)
      {
      case OR:
	{
	  struct location *locTmp = NULL;
	  res = generateLocList (e->ls, l, syst);
	  for (locTmp = res; locTmp->next != NULL; locTmp = locTmp->next)
	    ;
	  locTmp->next = generateLocList (e->rs, l, syst);
	  break;
	}
      default:
	{
	  int idLoc;
	  string locName;
	  string tmp;
	  struct transition *transTmp;
	  struct transition *resTrans;
	  string numberS;
	  AvlTree t;
	  resTrans = NULL;
	  locName = getName (l->id, syst->idDic);
	  idLoc = getFreshId ();

	  for (transTmp = l->transitions; transTmp != NULL;
	       transTmp = transTmp->next)
	    resTrans =
	      makeTransition (transTmp->idDest, transTmp->idLabel,
			      transTmp->ts, copyExpression (transTmp->guard),
			      copyExpression (transTmp->update), resTrans);
	  res =
	    makeLocation (idLoc, e, copyExpression (l->rateCond), resTrans,
			  NULL);

	  /*registering into the dictionnary */
	  numberS = integerToString (idLoc);
	  tmp =
	    (string) malloc ((strlen (locName) + 2 + strlen (numberS)) *
			     sizeof (char));
	  strcpy (tmp, locName);
	  strcat (tmp, "S");
	  strcat (tmp, numberS);
	  t = makeAvlNode (idLoc, tmp, LOCATION);
	  t->data.locPtr = res;
	  syst->idDic = Insert (t, syst->idDic);
	  break;
	}
      }
  return res;
}

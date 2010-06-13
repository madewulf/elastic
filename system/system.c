#include "system.h"
struct system * makeEmptySystem()
{
  struct system * res;
  res=(struct system *) malloc(sizeof (struct system));
  if (!res)
    {
      fprintf(stderr,"Not enough memory!\n");
      exit(-1);
    }

  res->varList=NULL;
  res->automList=NULL;
  res->paramInit=NULL;
  res->badStates=NULL;
  res->idDic=NULL;
  res->viewList=NULL;
  res->receptivenessChecking=0;
  return res;
}

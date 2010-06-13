#include "labStatus.h"
#include <stdlib.h>
#include <stdio.h>
#include "../enums.h"
/*structure utilisée dans writeUppaal, et dans toUppaal
  certaines fonctions ne sont utiles que pour l'un des deux*/

struct labStatus * makeLabStatus(int labId, int emitted ,struct labStatus * next)
{
  struct labStatus * res;
  res = (struct labStatus * ) malloc(sizeof(struct labStatus));
  if (!res)
    {
      fprintf(stderr,"Not enough memory !");
      exit(-1);
    }
  res->labId=labId;
  res->status=emitted;
  res->count=0;
  res->next=next;
  return res;
}

int getStatus(int labId, struct labStatus * labList)
{
  struct labStatus * tmp;
  tmp=labList;
  while(tmp && (tmp->labId!=labId))
    tmp=tmp->next;
  if (tmp)
    return tmp->status;
  else 
    return -1;
}

struct labStatus * insertIntoLabList(int labId,struct labStatus * labList)
{
  struct labStatus * res=NULL;
  res=labList;
  if (getStatus(labId,labList)==-1)
    {
      res=makeLabStatus(labId,0,labList);
    }
  return res;
}

struct labStatus * insertWithStatus(int labId, int status, struct labStatus * labList)
{
 struct labStatus * res=NULL;
  res=labList;
  if (getStatus(labId,labList)==-1)
    {
      res=makeLabStatus(labId,status,labList);
    }
  return res;
}

void inverseStatus(int labId, struct labStatus * labList)
{
  
  struct labStatus * tmp;
  tmp=labList;
  
  while(tmp->labId!=labId)
    tmp=tmp->next;
  
  tmp->status= ((tmp->status) +1)%2;
  
}

struct labStatus * insertIncrement(int labId,struct labStatus * lablist)
{
  struct labStatus * tmp;
  for(tmp=lablist; tmp!=NULL && tmp->labId!=labId;tmp=tmp->next)
    ;
  if (tmp==NULL)
    {
      tmp= makeLabStatus(labId,ORDINARY,lablist);
      tmp->count=1;
      return tmp;
    }
  else
    {
      (tmp->count) ++;
      return lablist;
    }
}

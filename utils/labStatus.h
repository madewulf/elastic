#ifndef __LABSTATUS_H
#define __LABSTATUS_H
struct labStatus{ 
  int labId;
  int status;
  int count;
  struct labStatus * next ;
};

struct labStatus * makeLabStatus(int labId, int emitted,struct labStatus * next);

int getStatus(int labId, struct labStatus * labList);

struct labStatus * insertIntoLabList(int labId,struct labStatus * labList);

struct labStatus * insertWithStatus(int labId, int status, struct labStatus * labList);

struct labStatus * insertIncrement(int labId,struct labStatus * lablist);

void inverseStatus(int labId, struct labStatus * labList);
#endif

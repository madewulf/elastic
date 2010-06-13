#ifndef __WRITE_UPPAAL_H
#define __WRITE_UPPAAL_H
#include "../system/system.h"

void writeUppaal(struct system *, string);

void writeUppaalAutomaton(struct automaton * autom, FILE * idFile,AvlTree idDic);

void writeUppaalLocation(struct location * loc,FILE * idFile,AvlTree idDic);

void writeUppaalTransition(struct transition * trans, int idSource, FILE * idFile, AvlTree idDic);

#endif

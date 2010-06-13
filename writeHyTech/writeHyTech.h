#ifndef __WRITE_HYTECH_H
#define __WRITE_HYTECH_H
#include "../system/system.h"
enum constraintType{GUARD,UPDATE};
void writeHyTech(struct system *, string);

void writeHyTechAutomaton(struct automaton * autom, FILE * idFile,AvlTree idDic);

void writeHyTechLocation(struct location * loc,FILE * idFile,AvlTree idDic);

void writeHyTechTransition(struct transition * trans, FILE * idFile, AvlTree idDic);

void writeConstraintHyTech(expr e, FILE * idFile, AvlTree idDic, enum constraintType ct);
void writeCommands(FILE * idFile, struct system * syst);

#endif

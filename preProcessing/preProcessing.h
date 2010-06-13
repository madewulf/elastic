
#ifndef __PRE_PROCESSING_H
#define __PRE_PROCESSING_H


#include "../utils/utils.h"

int ppparse();
/* procède au pré-processing du fichier d'input */

void setInputPreProc(FILE * idFile);
/* spécifie que l'input du pré-processing est situé dans le fichier
   de descripteur <idFile> */

void setOutputPreProc(string tempFileName);
/* précise que l'output du pré-processing sera écrit dans le fichier
nommé <tempFileName> */

void writeNewInput(string text);
/* écrit le texte <text> dans le fichier d'output du pre-processing */


#endif

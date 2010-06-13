
#ifndef __PRE_PROCESSING_H
#define __PRE_PROCESSING_H


#include "../utils/utils.h"

int ppparse();
/* proc�de au pr�-processing du fichier d'input */

void setInputPreProc(FILE * idFile);
/* sp�cifie que l'input du pr�-processing est situ� dans le fichier
   de descripteur <idFile> */

void setOutputPreProc(string tempFileName);
/* pr�cise que l'output du pr�-processing sera �crit dans le fichier
nomm� <tempFileName> */

void writeNewInput(string text);
/* �crit le texte <text> dans le fichier d'output du pre-processing */


#endif

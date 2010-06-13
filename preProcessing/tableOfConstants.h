
#ifndef __TABLE_OF_CONSTANTS_H
#define __TABLE_OF_CONSTANTS_H


#include "../utils/utils.h"

struct NodePP;

typedef struct NodePP *TableOfConstants;

TableOfConstants insertConstant(TableOfConstants T, string id, string constant);
/* ins�re <id> et <constant> dans <T>, puis retourne <T> */

string findConstant(TableOfConstants T, string id);
/* recherche <id> dans <T> et retourne le string nomm� "constant" associ� � <id>.
Si <id> non trouv�, NULL retourn� */

void deleteTableOfConstants(TableOfConstants T);
/* supprime la table des constantes <T> */


#endif

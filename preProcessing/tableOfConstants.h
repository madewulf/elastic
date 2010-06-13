
#ifndef __TABLE_OF_CONSTANTS_H
#define __TABLE_OF_CONSTANTS_H


#include "../utils/utils.h"

struct NodePP;

typedef struct NodePP *TableOfConstants;

TableOfConstants insertConstant(TableOfConstants T, string id, string constant);
/* insère <id> et <constant> dans <T>, puis retourne <T> */

string findConstant(TableOfConstants T, string id);
/* recherche <id> dans <T> et retourne le string nommé "constant" associé à <id>.
Si <id> non trouvé, NULL retourné */

void deleteTableOfConstants(TableOfConstants T);
/* supprime la table des constantes <T> */


#endif

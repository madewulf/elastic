
#ifndef __PARSER_H
#define __PARSER_H




int yyparse();
/* procède à la construction de l'arbre de parsing à partir du fichier d'input */

void setInputParser(char * tempFileName);
/* précise que la construction de l'arbre de parsing se fera à partir du contenu
   du fichier <tempFileName> */

struct system * getSystem();
#endif

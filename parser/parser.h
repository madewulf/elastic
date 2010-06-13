
#ifndef __PARSER_H
#define __PARSER_H




int yyparse();
/* proc�de � la construction de l'arbre de parsing � partir du fichier d'input */

void setInputParser(char * tempFileName);
/* pr�cise que la construction de l'arbre de parsing se fera � partir du contenu
   du fichier <tempFileName> */

struct system * getSystem();
#endif

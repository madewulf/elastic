
#ifndef __UTILS_H
#define __UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef char * string;

struct stringElem{
  int id;
  string s;
  struct stringElem * next;
};

struct stringElem * insertStringElem(string, int, struct stringElem *);

int getId(string s, struct stringElem * list);

struct stringElem * appendStringList(struct stringElem* list1, struct stringElem * list2);

void printStringList(struct stringElem * list);

string integerToString(int i);
/* retourne un string contenant la représentation décimale de l'entier <i> */

void replaceString(string * s1, string s2, string s3);
/* remplace chaque occurence de s2 dans s1 par s3 */

void deleteStringEnum(string * stringEnum, int taille);
/* supprime l'énumération <stringEnum> contenant <taille> strings */

void writeCode(string text, int idFile);
/* écrit le texte <text> dans le fichier d'id <idFile> */

void newLine(int indentLevel, int idFile);
/* écrit dans le fichier d'id <idFile> un retour à la ligne suivi de <indentLevel> tabulations */

string append(string ,string);
/*returns the string that is the concatenation of the two parameter strings*/

string integerToString(int i);
/* retourne un string contenant la représentation décimale de l'entier <i> */

#endif

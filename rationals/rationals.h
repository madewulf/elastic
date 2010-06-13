
#ifndef __RATIONALS_H
#define __RATIONALS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../utils/utils.h"
#include "../enums.h"

typedef enum sign signType;
typedef int bool;

struct rational{
  signType sign;
  int posNumerator;
  int posDenominator;
};

struct rational * makeRational(signType sign, int num, int denom);
/* retourne le rationel construit à partir du signe <sign>, du numérateur <num>
   et du dénominateur <denom> */

void deleteRational(struct rational * rat);
/* supprime le rationel <rat> */

struct rational * copyRational(struct rational * rat);
/* retourne une copie du contenu du rationel <rat> */

int computePgcd(int x, int y);
/* retourne le pgcd des entiers <x> et <y> */

int computePpcm(int x, int y);
/* retourne le ppcm des entiers <x> et <y>  */

struct rational * makeAddition(struct rational * rat1, struct rational * rat2);
/* retourne la somme des rationels <rat1> et <rat2> */

bool isRationalsEqual(struct rational * rat1, struct rational * rat2);
/* retourne 1 si les rationels <rat1> et <rat2> sont égaux, 0 sinon */

void printRational(struct rational * rat);
/* affiche le rationel <rat> */

bool greaterR(struct rational * A, struct rational * B);

#endif

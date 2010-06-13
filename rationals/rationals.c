

#include "rationals.h"

struct rational * makeRational (signType sign, int num, int denom){
  /* retourne le rationel construit à partir du signe <sign>, du numérateur <num>
     et du dénominateur <denom> */
  struct rational *result;
  /* vérification des paramètres */
  if (num < 0 || denom < 0){
    fprintf(stderr, "function makeRational error ! The parameters 'num' and 'denom' must be >= 0.\n");
    exit(-1);
  }
  /* fin vérification des paramètres */
  result = (struct rational *)malloc(sizeof(struct rational));
  result->sign = sign;
  result->posNumerator = num;
  result->posDenominator = denom;
  return result;
}

void deleteRational(struct rational * rat){
/* supprime le rationel <rat> */
  if (rat){
    free(rat);
    rat = NULL;
  }
}

struct rational * copyRational(struct rational * rat){
/* retourne une copie du contenu du rationel <rat> */
  struct rational * result = NULL;
  if (rat != NULL){
    result = (struct rational *)malloc(sizeof(struct rational));
    result->sign = rat->sign;
    result->posNumerator = rat->posNumerator;
    result->posDenominator = rat->posDenominator;
  }
  return result;
}

int computePgcd(int x, int y){
  /* retourne le pgcd des entiers <x> et <y> */
  int a, b;
  int quotient, rest;
  /* vérification des paramètres */
  if (x <= 0 || y <= 0){
    fprintf(stderr, "function computePgcd error ! The parameters 'x ' and 'y' must be > 0.\n");
    exit(-1);
  }
  /* fin vérification des paramètres */
  if (x > y){
    a = x;
    b = y;
  }else{
    a = y;
    b = x;
  }
  do{
    quotient = a / b;
    rest = a - quotient * b;
    a = b;
    b = rest;
  }
  while(rest != 0);
  return a;
}

int computePpcm(int x, int y){
/* retourne le ppcm des entiers <x> et <y> */
  /* vérification des paramètres */
  if (x <= 0 || y <= 0){
    fprintf(stderr, "function computePpcm error ! The parameters 'x ' and 'y' must be > 0.\n");
    exit(-1);
  }
  if (x * y < 0){
    /* overflow !!! */
    fprintf(stderr, "function computePpcm error ! Overflow during the computation of the ppcm.\n");
    exit(-1);
  }
  /* fin vérification des paramètres */
  return (x * y) / computePgcd(x, y);
}

struct rational * makeAddition(struct rational * rat1, struct rational * rat2){
  /* retourne la somme des rationels <rat1> et <rat2> */
  int num1, num2;
  int denom1, denom2;
  signType sign1, sign2;
  int ppcm, pgcd;
  struct rational * result;
  /* vérification des paramètres */
  if (rat1 == NULL && rat2 != NULL){
    return rat2;
  }
  if (rat1 != NULL && rat2 == NULL){
    return rat1;
  }
  if (rat1 == NULL && rat2 == NULL){
    return NULL;
  }
  /* fin vérification des paramètres */
  result = (struct rational *)malloc(sizeof(struct rational));
  num1 = rat1->posNumerator;
  num2 = rat2->posNumerator;
  denom1 = rat1->posDenominator;
  denom2 = rat2->posDenominator;
  sign1 = rat1->sign;
  sign2 = rat2->sign;
  /* les 2 fractions sont mises au même dénominateur */
  ppcm = (denom1 * denom2) / computePgcd(denom1, denom2);
  num1 = num1 * (ppcm / denom1);
  num2 = num2 * (ppcm / denom2);
  denom1 = ppcm;
  denom2 = ppcm;
  /* on effectue la somme des 2 fractions */
  if (num1 > num2){
    if(sign1 == POSITIVE && sign2 == POSITIVE){
      result->sign = POSITIVE;
      result->posNumerator = num1 + num2;    
    }else if(sign1 == NEGATIVE && sign2 == POSITIVE){
      result->sign = NEGATIVE;
      result->posNumerator = num1 - num2;    
    }else if(sign1 == POSITIVE && sign2 == NEGATIVE){
      result->sign = POSITIVE;
      result->posNumerator = num1 - num2;    
    }else{ /* sign1 == NEGATIVE && sign2 == NEGATIVE  */
      result->sign = NEGATIVE;
      result->posNumerator = num1 + num2;    
    }
  }else{ /* num1 <= num2 */
    if(sign1 == POSITIVE && sign2 == POSITIVE){
      result->sign = POSITIVE;
      result->posNumerator = num1 + num2;    
    }else if(sign1 == NEGATIVE && sign2 == POSITIVE){
      result->sign = POSITIVE;
      result->posNumerator = num2 - num1;    
    }else if(sign1 == POSITIVE && sign2 == NEGATIVE){
      result->sign = NEGATIVE;
      result->posNumerator = num2 - num1;
    }else{ /* sign1 == NEGATIVE && sign2 == NEGATIVE  */
      result->sign = NEGATIVE;
      result->posNumerator = num1 + num2;
    }
  }
  /* on simplifie la fraction résultat */
  pgcd = computePgcd(result->posNumerator, denom1);
  result->posNumerator = result->posNumerator / pgcd;
  result->posDenominator = denom1 / pgcd;
  return result;
}

bool isRationalsEqual(struct rational * rat1, struct rational * rat2){
/* retourne 1 si les rationels <rat1> et <rat2> sont égaux, 0 sinon */
  bool result;
  /* vérification des paramètres */
  if (rat1 == NULL){
    fprintf(stderr, "function isRationalsEqual error ! The parameter 'rat1' can't be NULL\n");
    exit(-1);
  }
  if (rat2 == NULL){
    fprintf(stderr, "function isRationalsEqual error ! The parameter 'rat2' can't be NULL\n");
    exit(-1);
  }
  /* fin vérification des paramètres */
  if (rat1->sign == rat2->sign
      && rat1->posNumerator == rat2->posNumerator
      && rat1->posDenominator == rat2->posDenominator){
    result = 1;
  }else{
    result = 0;
  }
  return result;
}

void printRational(struct rational * rat){
/* affiche le rationel <rat> */
  /* vérification des paramètres */
  if (rat == NULL){
    fprintf(stderr, "function printRational error ! The parameter 'rat' can't be NULL\n");
    exit(-1);
  }
  /* fin vérification des paramètres */
    if (rat->sign == NEGATIVE){
      printf("-");
    }
    printf("%i", rat->posNumerator);
    if (rat->posDenominator != 1){
      printf("/");
      printf("%i", rat->posDenominator);
    }
}

bool greaterR(struct rational * A, struct rational * B)
{

  /*warning : is it always right ? */ 
  int multA, multB;
  if (A && B)
    {
      multA=A->sign==POSITIVE ? 1:-1;
      multB=B->sign==POSITIVE ? 1:-1;
      return ( multA * ((float) A->posNumerator / A->posDenominator) 
	       >= multB * ((float) B->posNumerator / B->posDenominator) );
    }
  else
    return 0;
}


#include "utils.h"

void printStringList(struct stringElem * list)
{
  while(list!=NULL)
    {
      printf("Elément d'id %i : %s \n", list->id,list->s);
      list=list->next;
    }
}

struct stringElem * makeStringElem(string s, struct stringElem * ne,int no)
{
  struct stringElem * res;
  res= (struct stringElem *) malloc (sizeof(struct stringElem));
  if (!res)
    {
      fprintf(stderr,"Not enough memory!\n");
      exit(-1);
    }
  res->next =ne;
  res->s= (string) malloc (sizeof(char) * (strlen(s)+1));
  if (!s)
    {
      fprintf(stderr,"Not enough memory!\n");
      exit(-1);
    }
  strcpy(res->s,s);
  res->id=no;
  return res;
}

struct stringElem * insertStringElem(string s, int id , struct stringElem * list)
{
  return makeStringElem(s,list,id);
}


int getId(string s, struct stringElem * list)
{
  struct stringElem * p;
  for (p=list;p!=NULL && strcmp(s,p->s);p=p->next)
    ;
  if (p)
    return p->id;
  else 
    return -1;
}
      
struct stringElem * appendStringList(struct stringElem* list1, struct stringElem * list2)
{
  struct stringElem * res;
  if(!list1)
    res=list2;
  else
    {
      res=list1;
      for( ; list1->next!=NULL; list1=list1->next)
	;
      list1->next=list2;
    }  
  return res;
}
  

string integerToString(int number){
/* retourne un string contenant la représentation décimale de l'entier <i> */
  string result;
  int lg, nbr;
  int i;
  /* vérification des paramètres */
  if (number < 0){
    fprintf(stderr, "function integerToString error ! The parameter 'number' can't be negative.\n");
    exit(-1);
  }
  /* fin vérification des paramètres */
  if (number == 0){
    result = (char *)malloc(2);
    result[0] = '0';
    result[1] = '\0';
  }else{
    lg = 0;
    for (nbr = number ; nbr != 0 ; nbr /= 10){
      lg++;
    }
    result = (char *)malloc(lg + 1);
    nbr = number;
    for (i = lg - 1 ; 0 <= i ; i--){
      result[i] = '0' + nbr % 10;
      nbr /= 10;
    }
    result[lg] = '\0';
  }
  return result;
}

void replaceString(string * ps1, string s2, string s3){ 
/* remplace toutes les  occurence de s2 dans s1 par s3 */
  int i;
  int j;
  int l2;
  int l3;
  string s1;
  string tmp;

  s1=*ps1;
  l2=strlen(s2);
  l3=strlen(s3);

  for(i=0; s1[i]!='\0' ; i++){

    if (!strncmp(s1+i,s2,l2)){ /* si occurence de s2 en i dans s1 */
      if (l2 >= l3){

	for(j=0;j<l3;j++)
	  s1[i+j]=s3[j];
	i=i+l3;
	for (j=i; s1[j+l2-l3]!='\0'; j++)
	  s1[j]=s1[j+l2-l3];
	s1[j]='\0';
      }
      else { /* l3 > l2 dans ce cas, il faut réallouer de la mémoire*/

	tmp=(string) malloc( (strlen(s1)+1+ l3-l2) * sizeof(char)*2);
	strcpy(tmp,s1);

	for(j=strlen(s1)+l3-l2; j!=i;j--)
	  tmp[j]=tmp[j-(l3-l2)];

	for(j=0;j<l3;j++)
	  tmp[i+j]=s3[j];

	free(s1);
	*ps1=tmp;
	s1=tmp;
      }
    }
  }
}

string append(string s1 ,string s2 )
{
  string res;
  res= (string) malloc ((strlen(s1) + strlen(s2)+1)*sizeof(char));
  strcpy(res,s1);
  strcat(res,s2);
  /*free(s1);
    free(s2);*/
  return res;
}

void deleteStringEnum(string * stringEnum, int taille){
/* supprime l'énumération <stringEnum> contenant <taille> strings */
  int i;
  /* vérification des paramètres */
  if (taille < 0){
    fprintf(stderr, "function deleteStringEnum error ! The parameter 'taille' can't be negative.\n");
    exit(-1);
  }
  /* fin vérification des paramètres */
  if (stringEnum != NULL){
    for (i = 0 ; i < taille ; i++){
      free(stringEnum[i]);
      stringEnum[i] = NULL;
    }
    free(stringEnum);
    stringEnum = NULL;
  }
}


void writeCode(string text, int idFile){
/* écrit le texte <text> dans le fichier d'id <idFile> */
  int writeResult;
  /* vérification des paramètres */
  if (text == NULL){
    fprintf(stderr, "function writeCode error ! The parameter 'text' can't be NULL\n");
    exit(-1);
  }
  if (idFile == -1){
    fprintf(stderr, "function writeCode error ! The parameter 'idFile' can't be negative\n");
    exit(-1);
  }
  /* fin vérification des paramètres */
  writeResult = write(idFile, text, strlen(text));
  if (writeResult <= 0){
      fprintf(stderr, "function writeCode error ! Error while writing output file\n");
      exit(-1);      
  }
}

void newLine(int indentLevel, int idFile){
/* écrit dans le fichier d'id <idFile> un retour à la ligne suivi de <indentLevel> tabulations */
  int i;
  /* vérification des paramètres */
  if (indentLevel < 0){
    fprintf(stderr, "function newLine error ! The parameter 'indentLevel' can't be negative\n");
    exit(-1);
  }
  if (idFile == -1){
    fprintf(stderr, "function newLine error ! The parameter 'idFile' can't be negative\n");
    exit(-1);
  }
  /* fin vérification des paramètres */
  writeCode("\n", idFile);
  for (i = 0 ; i < indentLevel ; i++){
    writeCode("\t", idFile);
  }
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "parser/parser.h"
#include "writeUppaal/writeUppaal.h"
#include "toUppaal/toUppaal.h"

typedef struct InputParameters{
  int languageToProduce;
  bool receptivenessChecking ;
  /* 0 : Uppaal , 1 : HyTech (DEFAULT) */
}InputParameters;


InputParameters * analyseInputParameters(int paramNumber, char **textParam){
  /* traitement des param�tres fournis � notre programme. Cette fonction analyse les
     <paramNumber> param�tres stock�s dans <textParam> et retourne la configuration
     d�finie par ces param�tres */
  InputParameters * result;
  int i;
  /* v�rification des param�tres */
  if (textParam == NULL){
    fprintf(stderr, "function analyseInputParameters error ! The parameter 'textParam' can't be NULL.\n");
    exit(-1);
  }
  /* fin v�rification des param�tres */
  result = (InputParameters *)malloc(sizeof(InputParameters));
  result->languageToProduce=0;
  result->receptivenessChecking=0;
  for (i = 1 ; i < paramNumber ; i++){
    if (strcmp(textParam[i], "-H") == 0){
      result->languageToProduce = 0;    
    }else if (strcmp(textParam[i], "-U") == 0){
      result->languageToProduce = 1;    
    }
    else if (strcmp(textParam[i], "-R")==0) 
    {
      result->receptivenessChecking=1;
    }
    else{
      fprintf(stderr, "- Warning : unknown option \"%s\". Ignored.\n", textParam[i]);
    }
  }
  return result;
}

void printToolUsage(){
  /* affiche les diff�rentes options avec lesquelles notre programme peut �tre lanc� */
  printf("Tool for translating Elastic specifications to Hytech or Uppaal language\n");
  printf("Usage: elastic [-options] <filename>\n");
  printf("\t[-H/-U]\t translation option\n");
  printf("\t\t H : Elastic to HyTech language [def], \n\t\t U : Elastic to Uppaal language.\n");
  printf("\t[-R] receptiveness checking for controllers\n");
}

string makeOutputFileName(string inputFileName, int languageToProduce){
  /* a partir du langage d'ouput d�fini par <languageToProduce> et du nom <inputFileName>
     repr�sentant le fichier d'input, construit et retourne le nom du fichier d'output */
  string outputFileName;
  int slashPosition = -1;
  int pointPosition = -1;
  int i;
  /* v�rification des param�tres */
  if (inputFileName == NULL){
    fprintf(stderr, "function makeOutputFileName error ! The parameter 'inputFileName' can't be NULL.\n");
    exit(-1);
  }
  /* fin v�rification des param�tres */
  for (i = 0 ; i < strlen(inputFileName) ; i++){
    if (inputFileName[i] == '/'){
      slashPosition = i;
    }else if (inputFileName[i] == '.'){
      pointPosition = i;
    }
  }
  /* SI : nom input = "directory/fileName" ou "fileName" */
  if (pointPosition <= slashPosition){
    /* SI : langage = Uppaal */
    if (languageToProduce == 1 || languageToProduce==2){
      outputFileName = (char *)malloc(sizeof(char) *(strlen(inputFileName) + 1));
      strcpy(outputFileName, inputFileName);
    }
    /* SINON : langage = Hytech */
    else{
      outputFileName = (char *)malloc(sizeof(char) *(strlen(inputFileName) + 6));
      strcpy(outputFileName, inputFileName); 
      strcat(outputFileName, ".hy\0");
    }
  }
  /* SINON : nom input = "directory/fileName.ext" ou "fileName.ext" */
  else{
    /* SI : langage = Uppaal */
    if (languageToProduce == 1 || languageToProduce==2){
      outputFileName = (char *)malloc(sizeof(char) * (pointPosition + 1));
      strncpy(outputFileName, inputFileName, pointPosition);
    }
    /* SINON : langage = Hytech */
    else{
      outputFileName = (char *)malloc(sizeof(char) * (pointPosition + 6));
      strncpy(outputFileName, inputFileName, pointPosition);
      strcat(outputFileName, ".hy\0");
    }
  }
  return outputFileName;
}


void systemTreatment(InputParameters *inputParam, string outputFileName){
  /* l'arbre de parsing pr�alablement construit, cette fonction va lui appliquer les
     diff�rentes transformations pr�cis�es par <inputParam>, puis g�n�rer le fichier
     d'ouput nomm� <outputFileName> */
  /* v�rification des param�tres */
  string tmp;
  struct system * syst;
  if (inputParam == NULL){
    fprintf(stderr, "function systemTreatment error ! The parameter 'inputParam' can't be NULL.\n");
    exit(-1);
  }
  if (outputFileName == NULL){
    fprintf(stderr, "function systemTreatment error ! The parameter 'outputFileName' can't be NULL.\n");
    exit(-1);
  }
  /* fin v�rification des param�tres */
  syst= getSystem();

  syst->receptivenessChecking=inputParam->receptivenessChecking;
  if (inputParam->languageToProduce==1)
    { 
      toWatchers(syst);
      toUppaal(syst);
      writeUppaal(syst,outputFileName);
    }
  else if (inputParam->languageToProduce==0)
    {
     toWatchers(syst);
     writeHyTech(syst,outputFileName);
   }
  else
    {
      toUppaal(syst);
      writeUppaal(syst,outputFileName);
    }

 
}

int main(int argc, char **argv){


  FILE *idFile;
  InputParameters *inputParam;
  if (argc == 1
      || (argc == 2 && (strcmp(argv[1], "-H") == 0 ||strcmp(argv[1], "-U") == 0 ||strcmp(argv[1], "-HU") == 0 )  )){
    printToolUsage();
  }else{
    inputParam = analyseInputParameters(argc - 1, argv);
    idFile = fopen(argv[argc - 1], "r");
    if (idFile != NULL){
      setInputPreProc(idFile);
      setOutputPreProc("tmpInput");
      if (ppparse() == 1){
	printf("- Error : incorrect input.\n");
	unlink("tmpInput");
	fclose(idFile);
      }
      else{	
	setInputParser("tmpInput");
	if (yyparse() == 1){
	  printf("- Error : incorrect input.\n");
	  unlink("tmpInput");
	  fclose(idFile);
	}
	else{
	  string outputFileName;
	  printf("- Correct Input.\n");
	  unlink("tmpInput");
	  fclose(idFile);
	  outputFileName = makeOutputFileName(argv[argc - 1], inputParam->languageToProduce);
	  systemTreatment(inputParam, outputFileName);
	}
      }      
    }else{
      fprintf(stderr, "- Erreur : ouverture du fichier \"%s\" echouee.\n", argv[argc - 1]);
      exit(-1);
    }
  }
  return 0;
}

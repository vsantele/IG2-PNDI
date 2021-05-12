#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define NB_MODELS 6
#define NB_VAR_MAX 600
#define LINE_LENGTH_MAX 8000
#define NB_MAX 400
#define ROOT_FOLDER "../../out"
#define DATA_FOLDER "data"
#define MODEL_FOLDER "model"
#define FI_MODEL_FILENAME "models.csv"
#define FI_TEST_FILENAME "testSet.csv"

typedef struct model Model;
struct model
{
	int movement;
	double vAccs[NB_VAR_MAX];
};

int findBestModels(Model models[], FILE *pFiTest, int estimateClasses[], int realClasses[]);
double getDistance(double modelVAccs[], double vAccs[], int nbVAccs);
void convertFileToTable(FILE *pFiMode, Model models[]);
int extractVAcc(double vAccs[], char line[], int startColumn);
int getMovement(char line[]);
double getField(char line[], int num);

void main(void)
{
  FILE *pFiTest = NULL;
  FILE *pFiModel = NULL;
  char path[512];
  errno_t err;

  sprintf_s(path, 512, "%s/%s/%s", ROOT_FOLDER, DATA_FOLDER, FI_TEST_FILENAME);
  err = fopen_s(&pFiTest, path, "r");
  if (err != 0)
  {   
    printf("Erreur lors de l'ouverture du fichier %s : %d'", path, err);
    return;
  }

  sprintf_s(path, 512, "%s/%s/%s", ROOT_FOLDER, MODEL_FOLDER, FI_MODEL_FILENAME);
  err = fopen_s(&pFiModel, path, "r");
  if (err != 0) {
    fclose(pFiTest);
    printf("Erreur lors de l'ouverture du fichier %s : %d'", path, err);
    return;
  }

  Model models[NB_MODELS];
  int estimateClasses[NB_MAX];
  int realClasses[NB_MAX];

  convertFileToTable(pFiModel, models);

  int nbTests = findBestModels(models, pFiTest, estimateClasses, realClasses);
  
  displayResultForEachClass(realClasses, estimateClasses, nbTests);
  displayAccuracy(realClasses, estimateClasses, nbTests);
  displayClass(realClasses, estimateClasses, nbTests);

  fclose(pFiTest);
  fclose(pFiModel);
}

int findBestModels(Model models[], FILE *pFiTest, int estimateClasses[], int realClasses[])
{
  int nbTests = 0;
  char line[LINE_LENGTH_MAX];
  double vAccs[NB_VAR_MAX];
  int iModel;
  double distance;
  fgets(line, LINE_LENGTH_MAX, pFiTest); // 1er getLine pour le remove header
  fgets(line,LINE_LENGTH_MAX,pFiTest);
  while(!feof(pFiTest)){
    int movement = getMovement(line);
    double closestDistance = INT_MAX;
    int bestMovement = -1;
    int nbVAccs = extractVAcc(vAccs, line, 4);
    iModel = 0;
    while(iModel < NB_MODELS){
        distance = getDistance(models[iModel].vAccs, vAccs, nbVAccs);
        if(distance < closestDistance){ 
            closestDistance = distance; 
            bestMovement = models[iModel].movement;
          }
          iModel++;
    }
    estimateClasses[nbTests] = bestMovement;
    realClasses[nbTests] = movement;
    fgets(line,LINE_LENGTH_MAX,pFiTest);
    nbTests++;
  }
  return nbTests;
}

double getDistance(double modelVAccs[], double vAccs[], int nbVAccs){
  double distance = 0;

  int iVector = 0;

  while(iVector < nbVAccs){
    distance += pow(vAccs[iVector] - modelVAccs[iVector], 2);
    
    iVector++;
  }

  return sqrt(distance);
}

void convertFileToTable(FILE * pFiModel, Model models[]){
  Model model;
  int nbModels = 0;
  char line[LINE_LENGTH_MAX];
  double vAccs[NB_VAR_MAX];
  int movement;
  
  fgets(line, LINE_LENGTH_MAX, pFiModel); // remove header

  fgets(line, LINE_LENGTH_MAX, pFiModel);

  while(!feof(pFiModel)){
    movement = getMovement(line);
    extractVAcc(model.vAccs, line, 2);

    model.movement = movement;
    models[nbModels] = model;
    nbModels++;
    
    fgets(line, LINE_LENGTH_MAX, pFiModel);
  }
}

int extractVAcc(double vAccs[], char line[], int startColumn) {
  int nbVAccs = 0;
  double vAcc;

  do
  {
    vAcc = getField(line, startColumn + nbVAccs);
    if (!isnan(vAcc)) {
      vAccs[nbVAccs] = vAcc;
      nbVAccs++;
    }
  } while (nbVAccs < NB_VAR_MAX && !isnan(vAcc));
  return nbVAccs;
}

int getMovement(char line[])
{
  double movement = getField(line, 1);
  if (!isnan(movement))
    return (int)getField(line, 1);
  return -1;
}

double getField(char line[], int num)
{
  char lineCpy[LINE_LENGTH_MAX];
  strcpy_s(lineCpy, LINE_LENGTH_MAX, line);
  const char *tok;
  char *nextToken;
  for (tok = strtok_s(lineCpy, ",", &nextToken);
       tok && *tok;
       tok = strtok_s(NULL, ",\n", &nextToken))
  {
    if (!--num)
      return atof(tok);
  }
  return NAN;
}
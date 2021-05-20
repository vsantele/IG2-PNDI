#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "../headers/library.h"

#define START_VACC_MODEL 2
#define START_VACC_TESTSET 4

typedef struct model Model;
struct model
{
  int movement;
  double vAccs[NB_VACC_MAX];
};


double getDistance(double modelVAccs[], double vAccs[], int nbVAccs);
void convertFileToTable(FILE *pFiMode, Model models[]);
int extractVAcc(double vAccs[], char line[], int startColumn);
int findBestModels(Model models[], FILE* pFiTest, int estimateClasses[], int realClasses[]);

// nbTests est un pointeur car la fonction useModel retourne une erreur mais on doit connaitre le nombre de valeur dans real et estimateClasses
errno_t useModel(int* pNbTests, int realClasses[], int estimateClasses[])
{
  FILE *pFiTest = NULL;
  FILE *pFiModel = NULL;
  char path[PATH_LENGTH];
  errno_t err;

  sprintf_s(path, PATH_LENGTH, "%s/%s/%s", ROOT_OUT_PATH, DATA_FOLDER, TEST_FILENAME);
  err = fopen_s(&pFiTest, path, "r");
  if (err != 0)
  {   
    printf("Erreur lors de l'ouverture du fichier %s : %d'", path, err);
    return err;
  }

  sprintf_s(path, PATH_LENGTH, "%s/%s/%s", ROOT_OUT_PATH, MODEL_FOLDER, MODEL_FILENAME);
  err = fopen_s(&pFiModel, path, "r");
  if (err != 0) 
  {
    fclose(pFiTest);
    printf("Erreur lors de l'ouverture du fichier %s : %d'", path, err);
    return err;
  }

  Model models[NB_MODELS];

  convertFileToTable(pFiModel, models);

  *pNbTests = findBestModels(models, pFiTest, estimateClasses, realClasses);

  fclose(pFiTest);
  fclose(pFiModel);
  return 0;
}

int findBestModels(Model models[], FILE *pFiTest, int estimateClasses[], int realClasses[])
{
  int nbTests = 0;
  char line[LINE_LENGTH_MAX];
  double vAccs[NB_VACC_MAX];
  int iModel;
  double distance;
  removeHeader(pFiTest);
  while(!feof(pFiTest))
  {
    fgets(line, LINE_LENGTH_MAX, pFiTest);
    if (nbTests % 10 == 0) printf(".");
    int movement = (int) getField(line, MOVEMENT_FIELD);
    double closestDistance = DBL_MAX;
    int bestMovement = -1;
    int nbVAccs = extractVAcc(vAccs, line, START_VACC_TESTSET);
    iModel = 0;
    while(iModel < NB_MODELS)
    {
      distance = getDistance(models[iModel].vAccs, vAccs, nbVAccs);
      if(distance < closestDistance)
      { 
        closestDistance = distance; 
        bestMovement = models[iModel].movement;
      }
      iModel++;
    }
    estimateClasses[nbTests] = bestMovement;
    realClasses[nbTests] = movement;
    
    nbTests++;
  }
  return nbTests;
}

double getDistance(double modelVAccs[], double vAccs[], int nbVAccs)
{
  double distance = 0;

  int iVector = 0;

  while(iVector < nbVAccs)
  {
    distance += pow(vAccs[iVector] - modelVAccs[iVector], 2);
    
    iVector++;
  }

  return sqrt(distance);
}

void convertFileToTable(FILE * pFiModel, Model models[])
{
  Model model;
  int nbModels = 0;
  char line[LINE_LENGTH_MAX];
  
  removeHeader(pFiModel);

  while( !feof(pFiModel) && nbModels < NB_MODELS)
  {
    fgets(line, LINE_LENGTH_MAX, pFiModel);
    model.movement = (int)getField(line, MOVEMENT_FIELD);
    extractVAcc(model.vAccs, line, START_VACC_MODEL);

    models[nbModels] = model;
    nbModels++;
    
  }
}

int extractVAcc(double vAccs[], char line[], int startColumn) 
{
  int nbVAccs = 0;
  double vAcc;

  vAcc = getField(line, startColumn + nbVAccs);
  while (nbVAccs < NB_VACC_MAX && !isnan(vAcc)){
    vAccs[nbVAccs] = vAcc;
    nbVAccs++;

    vAcc = getField(line, startColumn + nbVAccs);
  }
  return nbVAccs;
}

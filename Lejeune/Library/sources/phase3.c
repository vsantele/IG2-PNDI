#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "../headers/library.h"

typedef struct model Model;
struct model
{
  int movement;
  double vAccs[NB_VAR_MAX];
};


double getDistance(double modelVAccs[], double vAccs[], int nbVAccs);
void convertFileToTable(FILE *pFiMode, Model models[]);
int extractVAcc(double vAccs[], char line[], int startColumn);
int findBestModels(Model models[], FILE* pFiTest, int estimateClasses[], int realClasses[]);
void convertFileToTable(FILE* pFiModel, Model models[]);

errno_t useModel(int* nbTests, int realClasses[], int estimateClasses[])
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

  *nbTests = findBestModels(models, pFiTest, estimateClasses, realClasses);

  fclose(pFiTest);
  fclose(pFiModel);
  return 0;
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
  while(!feof(pFiTest))
  {
    if (nbTests % 10 == 0) printf(".");
    int movement = (int) getField(line, MOVEMENT_FIELD);
    double closestDistance = INT_MAX;
    int bestMovement = -1;
    int nbVAccs = extractVAcc(vAccs, line, 4);
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
    fgets(line, LINE_LENGTH_MAX, pFiTest);
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
  
  fgets(line, LINE_LENGTH_MAX, pFiModel); // remove header

  fgets(line, LINE_LENGTH_MAX, pFiModel);

  while(!feof(pFiModel))
  {
    model.movement = (int)getField(line, MOVEMENT_FIELD);
    extractVAcc(model.vAccs, line, 2);

    models[nbModels] = model;
    nbModels++;
    
    fgets(line, LINE_LENGTH_MAX, pFiModel);
  }
}

int extractVAcc(double vAccs[], char line[], int startColumn) 
{
  int nbVAccs = 0;
  double vAcc;

  do
  {
    vAcc = getField(line, startColumn + nbVAccs);
    if (!isnan(vAcc)) 
    {
      vAccs[nbVAccs] = vAcc;
      nbVAccs++;
    }
  } while (nbVAccs < NB_VAR_MAX && !isnan(vAcc));
  return nbVAccs;
}

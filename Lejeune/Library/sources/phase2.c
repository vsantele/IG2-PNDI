#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "../headers/library.h"

#define GENDER_FIELD 2

typedef enum gender Gender;
enum gender {WOMAN, MAN};

errno_t createModels();
void writeHeader(FILE* file);
Gender getGender(char line[]);
void processLine(char line[], double tab[], int nbTab[]);
void writeFile(FILE *fiModel, FILE *fiMenModel, FILE *fiWomenModel, double modelMen[], int nbModelMen[], double modelWomen[], int nbModelWomen[], int movement);

errno_t createModels() 
{
  FILE* fiTrain = NULL;

  FILE* fiModel = NULL;
  FILE* fiMenModel = NULL;
  FILE* fiWomenModel = NULL;

  char path[PATH_LENGTH];
  sprintf_s(path, PATH_LENGTH, "%s/%s/%s", ROOT_OUT_PATH, DATA_FOLDER, TRAIN_FILENAME);
  errno_t err = fopen_s(&fiTrain, path, "r");

  if(fiTrain == NULL)
  {
    printf("Erreur ouverture %s, code : %d\n",path, err);
    return err;
  }
  sprintf_s(path, PATH_LENGTH, "%s/%s/%s", ROOT_OUT_PATH, MODEL_FOLDER, MODEL_FILENAME);
  err = fopen_s(&fiModel, path, "w");

  if(fiModel == NULL)
  {
    printf("Erreur ouverture %s, code : %d\n", path, err);
    fclose(fiTrain);
    return err;
  }

  sprintf_s(path, PATH_LENGTH, "%s/%s/%s", ROOT_OUT_PATH, MODEL_FOLDER, MODEL_MEN_FILENAME);
  err = fopen_s(&fiMenModel, path, "w");

  if(fiMenModel == NULL)
  {
    printf("Erreur ouverture %s, code : %d\n", path, err);
    fclose(fiTrain);
    fclose(fiModel);
    return err;
  }

  sprintf_s(path, PATH_LENGTH, "%s/%s/%s", ROOT_OUT_PATH, MODEL_FOLDER, MODEL_WOMEN_FILENAME);
  err = fopen_s(&fiWomenModel, path, "w");

  if(fiWomenModel == NULL)
  {
    fclose(fiTrain);
    fclose(fiModel);
    fclose(fiMenModel);
    return err;
  }

  writeHeader(fiModel);
  writeHeader(fiMenModel);
  writeHeader(fiWomenModel);
  
  removeHeader(fiTrain);

  char line[LINE_LENGTH_MAX];
  fgets(line, LINE_LENGTH_MAX, fiTrain);
  while (!feof(fiTrain)) 
  {
    int curMovement = (int) getField(line, MOVEMENT_FIELD);
    int movement = curMovement;
    printf("Traitement du mouvement: %d\n", curMovement);
    // tableau de somme des valeurs existantes par VACC pour un homme
    double modelMen[NB_VACC_MAX] = {0};
    //  tableau de nombre de valeur par VACC pour un homme
    int nbModelMen[NB_VACC_MAX] = {0};
    //  tableau de somme des valeurs existantes par VACC pour une femme
    double modelWomen[NB_VACC_MAX] = {0};
    //  tableau de nombre de valeur par VACC pour une femme
    int nbModelWomen[NB_VACC_MAX] = {0};
    
    while (!feof(fiTrain) && movement == curMovement) // bloc logique
    {
      Gender gender = getGender(line);
      
      if (gender == MAN) 
      {
        processLine(line, modelMen, nbModelMen);
      } else 
      {
        processLine(line, modelWomen, nbModelWomen);
      }

      fgets(line, LINE_LENGTH_MAX, fiTrain);
      curMovement = (int) getField(line, MOVEMENT_FIELD);
    }

    writeFile(fiModel, fiMenModel, fiWomenModel, modelMen, nbModelMen, modelWomen, nbModelWomen, movement);
  }

  fclose(fiTrain);
  fclose(fiModel);
  fclose(fiMenModel);
  fclose(fiWomenModel);

  return 0;
}

Gender getGender(char line[])
{
  int gender = (int)getField(line, GENDER_FIELD);
  if(gender == 0)
  {
    return WOMAN;
  }
  
  return MAN;
}

void writeHeader(FILE* file)
{
  fprintf_s(file, "movement");
  for (int i = 0; i < NB_VACC_MAX; i++)
    {
      fprintf_s(file, ",VACC %d", i);
    }
}

void processLine(char line[], double tab[], int nbTab[])
{
  int iValue = 0;
  double value = getField(line, iValue + 4);
  while ( iValue < NB_VACC_MAX && !isnan(value))
  {
    tab[iValue] += value;
    nbTab[iValue]++;

    iValue++;
    value = getField(line, iValue + 4);
  }
}

void writeFile(FILE *fiModel, FILE *fiMenModel, FILE *fiWomenModel, double modelMen[], int nbModelMen[], double modelWomen[], int nbModelWomen[], int movement)
{
  fprintf_s(fiModel, "\n");
  fprintf_s(fiMenModel, "\n");
  fprintf_s(fiWomenModel, "\n");
  fprintf_s(fiModel, "%d", movement);
  fprintf_s(fiMenModel, "%d", movement);
  fprintf_s(fiWomenModel, "%d", movement);

  for(int i = 0; i < NB_VACC_MAX; i++)
  {
    double menMean = modelMen[i] / nbModelMen[i];
    double womenMean = modelWomen[i] / nbModelWomen[i];
    double mean = (modelMen[i] + modelWomen[i]) / (nbModelMen[i] + nbModelWomen[i]);
    fprintf_s(fiModel, ",%f", mean);
    fprintf_s(fiMenModel, ",%f", menMean);
    fprintf_s(fiWomenModel, ",%f", womenMean);
  }

}
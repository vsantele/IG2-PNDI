#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "../headers/library.h"


char folderNames[][FOLDER_NAME_LENGTH] = { "dws_1", "dws_11", "dws_2", "ups_12", "ups_3", "ups_4", "jog_16", "jog_9", "sit_13", "sit_5", "std_14", "std_6", "wlk_15", "wlk_7", "wlk_8" };
char movements[][MVT_LENGTH] = {"dws", "ups", "jog", "sit", "std", "wlk"};

errno_t extractData();
void createHeader(FILE* file);
errno_t readUsersGender(int usersGender[]);
int getMovement(char folderName[]);
int getGenderFromUsers(int userCode, int users[]);
FILE * getRandomWriteFile(FILE *testFile, FILE *trainFile);
double getAccVector(char line[]);
errno_t processFile(char path[], FILE *fiOut, int userId, int movement, int gender);


errno_t extractData() 
{
  srand(time(NULL)); // améliore le pseudo-aléatoire
  
  FILE* pFiTestData = NULL;
  FILE* pFiTrainData = NULL;
  int usersGender[NB_USERS];
  errno_t err;
  char path[PATH_LENGTH];
  errno_t errReadUsersGender = readUsersGender(usersGender);
  if (errReadUsersGender != 0)
    return errReadUsersGender;

  sprintf_s(path, PATH_LENGTH, "%s/%s/%s", ROOT_OUT_PATH, DATA_FOLDER, TEST_FILENAME);
  err = fopen_s(&pFiTestData, path, "w+");
  if (pFiTestData == NULL) 
  {
    printf("Erreur lors de l'ouverture du fichier %s : %d\n", path, err);
    return err;
  }

  sprintf_s(path, PATH_LENGTH, "%s/%s/%s", ROOT_OUT_PATH, DATA_FOLDER, TRAIN_FILENAME);
  err = fopen_s(&pFiTrainData, path, "w+");
  if (pFiTrainData == NULL)
  {
    printf("Erreur lors de l'ouverture du fichier %s : %d\n", path, err);
    fclose(pFiTestData);
    return err;
  }

  createHeader(pFiTestData);
  createHeader(pFiTrainData);

  for (int iFolder = 0; iFolder < NB_FOLDERS; iFolder++)
  {
    int idMovement = getMovement(folderNames[iFolder]);
    if(idMovement != NB_MODELS + 1) // vérifie si le mouvement est valide
    {
      for (int iFile = 0; iFile < NB_USERS; iFile++) 
      {
        //if ((iFile * iFolder) % 10 == 0) printf(".");
        char path[PATH_LENGTH];
        int userCode = iFile + 1;
        int gender = getGenderFromUsers(userCode, usersGender);
        sprintf_s(path, PATH_LENGTH, "%s/%s/sub_%d.csv", ROOT_DATA_PATH, folderNames[iFolder], userCode);
        FILE *fiOut = getRandomWriteFile(pFiTestData, pFiTrainData);
        err = processFile(path, fiOut, userCode, idMovement, gender );
        if (err != 0) 
        {
          printf("Erreur lors de l'ouverture du fichier %s : %d\n", path, err);
          return err;
        }
      }
      printf("."); // affichage du "chargement" (uniquement pour la beauté)
    }
  }

  fclose(pFiTestData);
  fclose(pFiTrainData);

  return 0;
}

void createHeader(FILE* file) 
{
  fprintf_s(file, "movement,gender,userCode");
  for (int i = 0; i < NB_VACC_MAX; i++)
  {
    fprintf_s(file, ",VACC %d", i);
  }
}

errno_t readUsersGender(int usersGender[])
{ // fonctionne
  FILE* fiDataSubjectInfos = NULL;
  char path[PATH_LENGTH];
  sprintf_s(path, PATH_LENGTH, "%s/%s", ROOT_DATA_PATH, SUBJECT_INFO_FILENAME);
  errno_t err = fopen_s(&fiDataSubjectInfos, path, "r");
  if (fiDataSubjectInfos == NULL)
  {
    printf("Erreur lors de l'ouverture du fichier %s : %d", path, err);
    return err;
  }
  int iUser = 0;
  char line[64];
  removeHeader(fiDataSubjectInfos);
  for (int iUser = 0; iUser < NB_USERS; iUser++)
  {
    fgets(line, 64, fiDataSubjectInfos);
    usersGender[iUser] = (int)getField(line, 5);
  }
  fclose(fiDataSubjectInfos);
  return 0;
}

int getMovement(char folderName[]) 
{
  char movement[MVT_LENGTH];
  strncpy_s(movement, MVT_LENGTH, folderName, 3); // on garde les 3 premiers char du nom de dossier
  int iMovement = 0;
  while(iMovement < NB_MODELS && strcmp(movement,movements[iMovement]) != 0)
  {
    iMovement++;
  }
  return iMovement + 1; // si mouvement innexistant retourne NB_MODELS + 1
}

int getGenderFromUsers(int userCode, int users[]) 
{
  return users[userCode - 1];
}

errno_t processFile(char path[], FILE *fiOut, int userCode, int movement, int gender)
{
  FILE *fiIn = NULL;
  errno_t err = fopen_s(&fiIn, path, "r");
  if (fiIn == NULL)
  {
    printf("Erreur lors de l'ouverture du fichier %s : %d", path, err);
    return err;
  }
  char line[LINE_LENGTH_MAX];
  removeHeader(fiIn); 
  int nLines = 0;
  fprintf_s(fiOut, "\n");
  fprintf_s(fiOut, "%d,%d,%d", movement, gender, userCode);
  while ( !feof(fiIn) && nLines < NB_VACC_MAX) 
  {
    fgets(line, LINE_LENGTH_MAX, fiIn);
    double accVec = getAccVector(line);
    fprintf_s(fiOut, ",%f", accVec); // write vector
    nLines++;
  }
  fclose(fiIn);
  return 0;
}

FILE *getRandomWriteFile( FILE *testFile, FILE *trainFile)
{
  if (rand() % 100 < PERCENT_TO_TESTSETS) 
  {
    return testFile;
  }
  return trainFile;
}

double getAccVector(char line[]) 
{
  double accX, accY, accZ;
  accX = getField(line, ACC_X_NUM_FIELD);
  accY = getField(line, ACC_Y_NUM_FIELD);
  accZ = getField(line, ACC_Z_NUM_FIELD);

  return sqrt(accX * accX + accY * accY + accZ * accZ);
}

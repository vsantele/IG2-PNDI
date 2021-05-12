#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "library.h"

#define LINES_COUNT 600
#define DATA_SUBJECT_INFO_FILENAME "../../TDD2021/data_subjects_info.csv"
#define TEST_OUTPUT_FILENAME "../../out/data/testSet.csv"
#define TRAIN_OUTPUT_FILENAME "../../out/data/trainSet.csv"
#define ROOT_DATA_PATH "../../TDD2021"
#define ROOT_OUT_PATH "../../out"
#define FOLDER_NAME_LENGTH 16
#define NB_USERS 24
#define NB_FOLDERS 15
#define MVT_LENGTH 4
#define PATH_LENGTH 128
#define NB_MOVEMENTS 6
#define ACC_X_NUM_FIELD 11
#define ACC_Y_NUM_FIELD 12
#define ACC_Z_NUM_FIELD 13
char folderNames[][FOLDER_NAME_LENGTH] = { "dws_1", "dws_11", "dws_2", "jog_16", "jog_9", "sit_13", "sit_5", "std_14", "std_6", "ups_12", "ups_3", "ups_4", "wlk_15", "wlk_7", "wlk_8" };
char movements[][MVT_LENGTH] = {"dws", "ups", "jog", "sit", "std", "wlk"};

errno_t extractData();
void createHeader(FILE* file);
errno_t readUsersGender(int usersGender[]);
int getMovement(char folderName[]);
int getGender(int userCode, int users[]);
FILE * getRandomWriteFile(FILE *testFile, FILE *trainFile);
void processFile(char fileName[], FILE* outputData, char movement[], int id, int gender);
double getAccVector(char line[]);
void writeVector(double vector, FILE* outputData);
double getField(char* line, int num);
errno_t readFile(char path[], FILE *fiOut, int userId, int movement, int gender);

void main(void)
{
  extractData();
}

errno_t extractData() {
  srand(time(NULL));
  FILE* fiTestData = NULL;
  FILE* fiTrainData = NULL;
  int usersGender[NB_USERS];
  errno_t err;
  errno_t errReadUsersGender = readUsersGender(usersGender);
  if (errReadUsersGender != 0)
    return errReadUsersGender;

  err = fopen_s(&fiTestData, TEST_OUTPUT_FILENAME, "w+");
  if (fiTestData == NULL) {
    printf("Erreur lors de l'ouverture du fichier %s : %d", TEST_OUTPUT_FILENAME, err);
    return err;
  }
  err = fopen_s(&fiTrainData, TRAIN_OUTPUT_FILENAME, "w+");
  if (fiTrainData == NULL)
  {
    printf("Erreur lors de l'ouverture du fichier %s : %d", TRAIN_OUTPUT_FILENAME, err);
    fclose(fiTestData);
    return err;
  }

  createHeader(fiTestData);
  createHeader(fiTrainData);

  for (int iFolder = 0; iFolder < NB_FOLDERS; iFolder++)
  {
    int idMovement = getMovement(folderNames[iFolder]);
    if(idMovement != NB_MOVEMENTS+1){
      for (int iFile = 0; iFile < NB_USERS; iFile++) {
        char path[PATH_LENGTH];
        int userCode = iFile + 1;
        int gender = getGender(userCode, usersGender);
        sprintf_s(path, PATH_LENGTH, "%s/%s/sub_%d.csv", ROOT_DATA_PATH, folderNames[iFolder], userCode);
        FILE *fiOut = getRandomWriteFile(fiTestData, fiTrainData);
        int err = readFile(path, fiOut, userCode, idMovement, gender );
      }
    }
  }

  fclose(fiTestData);
  fclose(fiTrainData);

  return 0;
}

void createHeader(FILE* file) {
  fprintf_s(file, "movement,gender,id,");
  for (int i = 0; i < LINES_COUNT; i++)
  {
    if (i < LINES_COUNT - 1)
    {
      fprintf_s(file, "VAR %d,", i);
    }
    else
    {
      fprintf_s(file, "VAR %d", i);
    }
  }
  fprintf_s(file, "\n");
}

double getField(char line[], int num)
{
 char lineCpy[1024];
 strcpy_s(lineCpy, 1024, line);
 const char* tok;
 char* nextToken;
 for (tok = strtok_s(lineCpy, ",", &nextToken);
   tok && *tok;
   tok = strtok_s(NULL, ",\n", &nextToken))
 {
   if (!--num)
     return atof(tok);
 }
 return -1;
}

errno_t readUsersGender(int usersGender[])
{ // fonctionne
  FILE* fiDataSubjectInfos = NULL;
  errno_t err = fopen_s(&fiDataSubjectInfos, DATA_SUBJECT_INFO_FILENAME, "r");
  if (fiDataSubjectInfos == NULL) {
    printf("Erreur lors de l'ouverture du fichier %s : %d", DATA_SUBJECT_INFO_FILENAME, err);
    return err;
  }
  int iUser = 0;
  char line[64];
  fgets(line, 64, fiDataSubjectInfos); // Remove header
  for (int iUser = 0; iUser < NB_USERS; iUser++) {
    fgets(line, 64, fiDataSubjectInfos);
    usersGender[iUser] = (int)getField(line, 5);
  }
  fclose(fiDataSubjectInfos);
  return 0;
}

int getMovement(char folderName[]) {
  char movement[MVT_LENGTH];
  strncpy_s(movement, MVT_LENGTH, folderName, 3);
  int iMovement = 0;
  while(iMovement < NB_MOVEMENTS && strcmp(movement,movements[iMovement]) != 0){
    iMovement++;
  }
  return iMovement+1;
}

int getGender(int userCode, int users[]) {
  return users[userCode - 1];
}

errno_t readFile(char path[], FILE *fiOut, int userId, int movement, int gender)
{
  FILE *fiIn = NULL;
  errno_t err = fopen_s(&fiIn, path, "r");
  if (fiIn == NULL)
  {
    printf("Erreur lors de l'ouverture du fichier %s : %d", path, err);
    return err;
  }
  char line[256];
  fgets(line, 256, fiIn); // remove header
  int nLines = 0;
  fprintf_s(fiOut, "%d,%d,%d", movement, gender, userId);
  while (!feof(fiIn) > 0 && nLines < LINES_COUNT) {
    fgets(line, 256, fiIn);
    double accVec = getAccVector(line);
    writeVector(accVec, fiOut);
    nLines++;
  }
  fprintf_s(fiOut, "\n");
  return 0;
}

FILE *getRandomWriteFile( FILE *testFile, FILE *trainFile)
{
  if (rand() % 100 < 10) {
    return testFile;
  }
  return trainFile;
}

double getAccVector(char line[]) {
  double accX, accY, accZ;
  accX = getField(line, ACC_X_NUM_FIELD);
  accY = getField(line, ACC_Y_NUM_FIELD);
  accZ = getField(line, ACC_Z_NUM_FIELD);

  return sqrt(accX * accX + accY * accY + accZ * accZ);
}

void writeVector(double vector, FILE *outputData) {
  fprintf_s(outputData, ",%f", vector);
}

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define LINES_COUNT 600
#define DATA_SUBJECT_INFO_FILENAME "../TDD2021/data_subjects_info.csv"
#define TEST_OUTPUT_FILENAME "../out/data/test_data.csv"
#define TRAIN_OUTPUT_FILENAME "../out/data/train_data.csv"
#define ROOT_DATA_PATH "../TDD2021"
#define FOLDER_NAME_LENGTH 16
#define NB_USERS 24
#define NB_FOLDERS 15
#define MVT_LENGTH 4
#define PATH_LENGTH 128
char folderNames[][FOLDER_NAME_LENGTH] = {"dws_1", "dws_11", "dws_2", "jog_16", "jog_9", "sit_13", "sit_5", "std_14", "std_6", "ups_12", "ups_3", "ups_4", "wlk_15", "wlk_7", "wlk_8"};

errno_t extractData();
void createHeader(FILE *file);
errno_t readUsersGender(int usersGender[]);
void getMovement(char movement[], char folderName[]);
int getGender(char filename[], int users[]);
FILE* randomWriteFile(FILE* test_data, FILE* train_data);
void processFile(char fileName[], FILE* outputData,char movement[],int id, int gender);
FILE* openFile(char filename[], char mode);
double getAccVector(char line[]);
void writeVector(double vector, FILE *outputData);
double getfield(char *line, int num);

void main(void)
{
  extractData();
  getchar();
}

errno_t extractData() {
  FILE *fiTestData = NULL;
  FILE *fiTrainData = NULL;
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
    return err;
  }
  createHeader(fiTestData);
  createHeader(fiTrainData);

  for (int iFolder = 0; iFolder < NB_FOLDERS; iFolder++)
  {
    char movement[MVT_LENGTH];
    getMovement(movement, folderNames[iFolder]);
    for (int iFile = 0; iFile < NB_USERS; iFile++) {
      char path[PATH_LENGTH];
      sprintf_s(path, PATH_LENGTH, "%s/%s/sub_%d", ROOT_DATA_PATH, folderNames[iFolder], iFile + 1);
      printf("%s\n",path);
    }
  }

    fclose(fiTestData);
  fclose(fiTrainData);

  return 0;
}

void createHeader(FILE* file){
  fprintf_s(file, "id,movement,gender,");
  for(int i = 0; i < LINES_COUNT; i++){
    if (i < LINES_COUNT - 1) {
      fprintf_s(file, "VAR %d,", i);
    } else {
      fprintf_s(file, "VAR %d", i);
    }
  }
  fprintf_s(file, "\n");
}

double getfield(char line[], int num)
{
  char lineCpy[1024];
  strcpy_s(lineCpy, 1024, line);
  const char *tok;
  for (tok = strtok(lineCpy, ",");
       tok && *tok;
       tok = strtok(NULL, ",\n"))
  {
    if (!--num)
      return atof(tok);
  }
  return -1;
}

errno_t readUsersGender(int usersGender[])
{ // fonctionne
  FILE *fiDataSubjectInfos = NULL;
  errno_t err = fopen_s(&fiDataSubjectInfos, DATA_SUBJECT_INFO_FILENAME, "r");
  if(fiDataSubjectInfos == NULL){
    printf("Erreur lors de l'ouverture du fichier %s : %d", DATA_SUBJECT_INFO_FILENAME, err);
    return err;
  }
    int iUser = 0;
    char line[64];
    fgets(line, 64, fiDataSubjectInfos); // Remove header
    for(int iUser = 0; iUser < NB_USERS; iUser++) {
      fgets(line, 64, fiDataSubjectInfos);
      usersGender[iUser] = (int)getfield(line, 5);
    }
    fclose(fiDataSubjectInfos);
    return 0;
}

void getMovement(char movement[], char folderName[]) {
  char folderCpy[FOLDER_NAME_LENGTH];
  strcpy_s(folderCpy, FOLDER_NAME_LENGTH, folderName);
  char *out = strtok(folderCpy, "_");
  strcpy_s(movement, 4, out);
}

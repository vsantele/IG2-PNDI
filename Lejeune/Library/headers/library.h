#ifndef LIBRARY_H
#define LIBRARY_H


  #define NB_VAR_MAX 600
  #define NB_MODELS 6
  #define NB_USERS 24

  #define LINE_LENGTH_MAX 10 * NB_VAR_MAX + 100

  #define MOVEMENT_FIELD 1
  #define ACC_X_NUM_FIELD 11
  #define ACC_Y_NUM_FIELD 12
  #define ACC_Z_NUM_FIELD 13

  #define PATH_LENGTH 128
  #define ROOT_OUT_PATH "../../out"
  #define ROOT_DATA_PATH "../../TDD2021"
  #define DATA_FOLDER "data"
  #define MODEL_FOLDER "model"
  #define MODEL_FILENAME "models.csv"
  #define TRAIN_FILENAME "trainSet.csv"
  #define TEST_FILENAME "testSet.csv"
  #define MODEL_MEN_FILENAME "modelsMen.csv"
  #define MODEL_WOMEN_FILENAME "modelsWomen.csv"

  // Utils
  double getField(char line[], int num);

  // Phase 1
  errno_t extractData();

  // Phase 2
  errno_t createModels();

  // Phase 3
  errno_t useModel(int* nbTests, int realClasses[], int estimateClasses[]);
#endif //LIBRARY_H
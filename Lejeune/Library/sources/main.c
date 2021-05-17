#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../headers/classificationStatistics.h"
#include "../headers/library.h"

#define NB_MAX 400

void func(void);

void main(void) {
  clock_t begin;
  clock_t end;
  double line[100];
  int iValue = 0;
  for (int i = 0; i < 1; i++) {
    begin = clock();
    func();
    end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    line[iValue] = time_spent;
    iValue++;
  }
  
  for (int i = 0; i < 1; i++) {
    printf(",%f", line[i]);
  }
}

void func(void)
{
  //char line[] = "0,0.8627940000000001,-1.1365399999999999,-0.009956999999999999,0.31961100000000003,0.907183,-0.273606,-0.49759799999999993,0.492856,0.154923,0.06697,-0.098803,0.149308";
  /*char line[] = "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20";

  for (int i = 1; i <= 15; i++) {
    double field = getField(line, i);
    printf("Champs %d : %f\n", i, field);
  }*/
  

  // Phase 1
  printf("Debut phase 1\n");
  errno_t err = extractData();

  if (err == 0) {
    // Phase 2
    printf("\n\n");
    printf("Debut phase 2\n");
    err = createModels();

    if (err == 0) {
      // Phase 3
      printf("\n\n");
      printf("Debut phase 3\n");
      int realClasses[NB_MAX];
      int estimateClasses[NB_MAX];
      int nbTests = 0;

      err = useModel(&nbTests, realClasses, estimateClasses);
      if (err == 0) {
        printf("\n\n");
        printf("Statistiques: \n");
        displayResultForEachClass(realClasses, estimateClasses, nbTests);
        printf("\n");
        displayAccuracy(realClasses, estimateClasses, nbTests);
        printf("\n");
        displayClass(realClasses, estimateClasses, nbTests);
      }
      else {
        printf("Erreur: %d", err);
      }
    }
    else {
      printf("Erreur: %d", err);
    }
  }
  else {
    printf("Erreur: %d", err);
  }
  
}
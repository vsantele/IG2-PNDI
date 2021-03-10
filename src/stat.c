#include<stdio.h>
#include<stdbool.h>
#include<malloc.h>
#include "./stat.h"

typedef struct resultClass ResultClass;
struct resultClass {
	int numClass;
	int nbCorrect;
	int nbTotal;
	ResultClass* pSuiv;
};

double accuracy(int realClasses[], int estimateClasses[], int nbTests);
ResultClass* resultsForEachClass(int realClasses[], int estimateClasses[], int nbTests);
bool classExist(int numClass, ResultClass* results);
ResultClass* createClass(int numClass/*, ResultClass** results */);
bool correctEstimation(int real, int estimation);
double toPercent(double ratio);
double ratio(int num, int dem);
ResultClass* searchClass(int numClass, ResultClass* pFirstResults);

void displayResultForEachClass(int realClasses[], int estimateClasses[], int nbTests) {
	ResultClass* pFirstResults = resultsForEachClass(realClasses, estimateClasses, nbTests);

	ResultClass* pResult = pFirstResults;

	while (pResult != NULL) {
		printf("Classe %d | Bien classe : %d | Total : %d |Pourcentage : %.2f%%\n", pResult->numClass, pResult->nbCorrect, pResult->nbTotal, toPercent(ratio(pResult->nbCorrect, pResult->nbTotal)));

		pResult = pResult->pSuiv;
	}
}

void displayAccuracy(int realClasses[], int estimateClasses[], int nbTests) {
	printf("L accuracy est de %.2f%%\n", toPercent(accuracy(realClasses, estimateClasses, nbTests)));
}

void displayClass(int realClasses[], int estimateClasses[], int nbTests) {
	printf("Display class");
}

ResultClass* resultsForEachClass(int realClasses[], int estimateClasses[], int nbTests) {
	ResultClass* pFirstResults = NULL;

	for (int iClass = 0; iClass < nbTests; iClass++) {
		ResultClass* searchedClass = searchClass(realClasses[iClass], pFirstResults);
		if (searchedClass == NULL) { // Liste est vide
		  pFirstResults = createClass(realClasses[iClass]);
			searchedClass = pFirstResults;
		} else if (searchedClass->numClass != realClasses[iClass]) { // Classe cherchée n'est pas dans la liste
			searchedClass->pSuiv = createClass(realClasses[iClass]);
			searchedClass = searchedClass->pSuiv;
		}

		if (correctEstimation(realClasses[iClass], estimateClasses[iClass])) {
			searchedClass->nbCorrect++;
		}

		searchedClass->nbTotal++;
	}

	return pFirstResults;
}

ResultClass* searchClass(int numClass, ResultClass* pFirstResults) {
	if (pFirstResults == NULL) { // liste chainée est vide
		return NULL;
	}
	
	ResultClass* pResult = pFirstResults;

	while (pResult->numClass != numClass && pResult->pSuiv != NULL) {
		pResult = pResult->pSuiv;
	}

	return pResult;
}

double accuracy(int realClasses[], int estimateClasses[], int nbTests) {
	int correctEstimation = 0;

	for (int iClass = 0; iClass < nbTests; iClass++) {
		if (realClasses[iClass] == estimateClasses[iClass]) {
			correctEstimation++;
		}
	}

	return (double) correctEstimation / nbTests;
}

bool classExist(int numClass, ResultClass* results) {
	if (results == NULL) {
		return false;
	}

	ResultClass* result = results;

	while (result != NULL && result->numClass != numClass) {
		result = result->pSuiv;
	}

	return result != NULL;
}

ResultClass* createClass(int numClass/*, ResultClass** results */) {
	// ResultClass* result = *results;

	// while (result != NULL && result->pSuiv != NULL) {
	// 	result = result->pSuiv;
	// }
	
	ResultClass* pNewClass = (ResultClass*)malloc(sizeof(ResultClass));
	pNewClass->nbCorrect = 0;
	pNewClass->nbTotal = 0;
	pNewClass->numClass = numClass;
	pNewClass->pSuiv = NULL;

	// result->pSuiv = newClass;
	return pNewClass;
}

double toPercent(double ratio) {
	return ratio * 100.0;
}

double ratio(int num, int den) {
	if (den == 0) return 0;
	return (double) num/den;
}

bool correctEstimation(int real, int estimation) {
	return real == estimation;
}

int main()
{
	int realClasses[] = { 5, 2, 5, 3, 5, 3, 2, 4 };
	int estimateClasses[] = { 5, 5, 1, 2, 1, 3, 2, 4 };
	int nbTests = 8;

	displayAccuracy(realClasses, estimateClasses, nbTests);

	displayResultForEachClass(realClasses, estimateClasses, nbTests);
	getchar();
	return 0;
}
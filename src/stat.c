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

void freeResultClassLinkedList(ResultClass *pFirstResult);

double accuracy(int realClasses[], int estimateClasses[], int nbTests);
double toPercent(double ratio);
double ratio(int num, int deno);

//pourquoi proto et def mais pas de use et plus dans le mapping
bool classExist(int numClass, ResultClass* results);
bool correctEstimation(int real, int estimation);

ResultClass* createClass(int numClass);
ResultClass* resultsForEachClass(int realClasses[], int estimateClasses[], int nbTests);
ResultClass* searchClass(int numClass, ResultClass* pFirstResult);

//pourquoi protomais pas de def et pas de use, et plus dans le mapping
int statsClass(int realClasses[], int estimateClasses[], int nbTests, int tabStats[NB_CLASSES_MAX][NB_CLASSES_MAX]);
//pourquoi protomais pas de def et pas de use, et plus dans le mapping
int initTabStats(int tabStats[NB_CLASSES_MAX][NB_CLASSES_MAX]);
int sumElements(int tab[], int taille);

int main()
{
	int realClasses[] = { 5, 2, 5, 3, 5, 3, 2, 4 };
	int estimateClasses[] = { 5, 5, 1, 2, 1, 3, 2, 4 };
	int nbTests = 8;

	displayAccuracy(realClasses, estimateClasses, nbTests);

	displayResultForEachClass(realClasses, estimateClasses, nbTests);

	displayClass(realClasses, estimateClasses, nbTests);
	getchar();
	return 0;
}

void displayResultForEachClass(int realClasses[], int estimateClasses[], int nbTests)
{
	ResultClass* pFirstResults = resultsForEachClass(realClasses, estimateClasses, nbTests);

	ResultClass* pResult = pFirstResults;

	while (pResult != NULL) {
		printf("Classe %d | Bien classe : %d | Total : %d |Pourcentage : %.2f%%\n", pResult->numClass, pResult->nbCorrect, pResult->nbTotal, toPercent(ratio(pResult->nbCorrect, pResult->nbTotal)));

		pResult = pResult->pSuiv;
	}

	freeResultClassLinkedList(pFirstResults);
}

void displayAccuracy(int realClasses[], int estimateClasses[], int nbTests) {
	printf("L accuracy est de %.2f%%\n", toPercent(accuracy(realClasses, estimateClasses, nbTests)));
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

ResultClass* searchClass(int numClass, ResultClass* pFirstResult) {
	if (pFirstResult == NULL) { // liste chainée est vide
		return NULL;
	}
	
	ResultClass* pResult = pFirstResult;

	while (pResult->numClass != numClass && pResult->pSuiv != NULL) {
		pResult = pResult->pSuiv;
	}

	return pResult;
}

double accuracy(int realClasses[], int estimateClasses[], int nbTests) {
	int correctEstimationCounter = 0;

	for (int iClass = 0; iClass < nbTests; iClass++) {
		if (correctEstimation(realClasses[iClass], estimateClasses[iClass])) {
			correctEstimationCounter++;
		}
	}

	return (double)correctEstimationCounter / nbTests;
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

ResultClass* createClass(int numClass) {
	ResultClass* pNewClass = (ResultClass*)malloc(sizeof(ResultClass));
	pNewClass->nbCorrect = 0;
	pNewClass->nbTotal = 0;
	pNewClass->numClass = numClass;
	pNewClass->pSuiv = NULL;

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

void freeResultClassLinkedList(ResultClass* pFirstResult)
{
	ResultClass* pClass = pFirstResult;

	while(pClass != NULL){
		ResultClass* pClassNext = pClass->pSuiv;
		free(pClass);

		pClass = pClassNext;
	}
}

int statsClass(int realClasses[], int estimateClasses[], int nbTests, int tabStats[NB_CLASSES_MAX][NB_CLASSES_MAX]) {
	int nbMax = 0;
	
	for(int i = 0; i < nbTests; i++) {
		if (nbMax < realClasses[i]) {
			nbMax = realClasses[i];
		}
		if (nbMax < estimateClasses[i])
		{
			nbMax = estimateClasses[i];
		}
		tabStats[realClasses[i] - 1][estimateClasses[i] - 1] += 1;
	}
	return nbMax;
}

void displayClass(int realClasses[], int estimateClasses[], int nbTests) {
	int tabStats[NB_CLASSES_MAX][NB_CLASSES_MAX] = {0};
	int nbMax = statsClass(realClasses, estimateClasses, nbTests, tabStats);

	printf("\tA classer\t|");
	for (int i = 0; i < nbMax; i++) {
		printf("%d\t", i + 1);
	}
	printf("\n");
	for (int i = 0; i < 59; i++) {
		printf("=");
	}
	printf("\n");
	printf("libelle\t\tnombre\t|\n");
	for (int i = 0; i < nbMax; i++)
	{
		printf("%d\t\t%d\t|", i + 1, sumElements(tabStats[i], nbMax));
		for (int j = 0; j < nbMax; j++)
		{
			printf("%d\t", tabStats[i][j]);
		}

		printf("\n");
	}
}

int sumElements(int tab[], int taille)
{
	int sum = 0;
	for(int i=0; i < taille; i++) {
		sum += tab[i];
	}
	return sum;
}
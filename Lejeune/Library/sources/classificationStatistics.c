#include<stdio.h>
#include<stdbool.h>
#include<malloc.h>
#include "../headers/classificationStatistics.h"

typedef struct resultClass ResultClass;
struct resultClass {
	int numClass;
	int nbCorrect;
	int nbTotal;
	ResultClass* pSuiv;
};

void freeResultClassLinkedList(ResultClass* pFirstResult);

double accuracy(int realClasses[], int estimateClasses[], int nbTests);
double toPercent(double ratio);
double ratio(int num, int deno);
bool correctEstimation(int real, int estimation);

ResultClass* createClass(int numClass);
ResultClass* resultsForEachClass(int realClasses[], int estimateClasses[], int nbTests);
ResultClass* searchClass(int numClass, ResultClass* pFirstResult);

int statsClass(int realClasses[], int estimateClasses[], int nbTests, int tabStats[NB_CLASSES_MAX][NB_CLASSES_MAX]);
int sumElements(int tab[], int taille);

void displayResultForEachClass(int realClasses[], int estimateClasses[], int nbTests)
{
	ResultClass* pFirstResults = resultsForEachClass(realClasses, estimateClasses, nbTests);

	ResultClass* pResult = pFirstResults;

	while (pResult != NULL) 
	{
		printf("Classe %2d | Bien classe : %2d | Total : %2d |Pourcentage : %.2f%%\n", pResult->numClass, pResult->nbCorrect, pResult->nbTotal, toPercent(ratio(pResult->nbCorrect, pResult->nbTotal)));

		pResult = pResult->pSuiv;
	}

	freeResultClassLinkedList(pFirstResults);
}

void displayAccuracy(int realClasses[], int estimateClasses[], int nbTests) 
{
	printf("L accuracy est de %.2f%%\n", toPercent(accuracy(realClasses, estimateClasses, nbTests)));
}

ResultClass* resultsForEachClass(int realClasses[], int estimateClasses[], int nbTests) 
{
	ResultClass* pFirstResults = NULL;

	for (int iClass = 0; iClass < nbTests; iClass++) {
		ResultClass* searchedClass = searchClass(realClasses[iClass], pFirstResults);

		if (searchedClass == NULL) { // Liste est vide
			pFirstResults = createClass(realClasses[iClass]);
			searchedClass = pFirstResults;
		}
		else if (searchedClass->numClass != realClasses[iClass]) { // Classe cherch�e n'est pas dans la liste
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
	if (pFirstResult == NULL) { // liste chain�e est vide
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

ResultClass* createClass(int numClass) {
	ResultClass* pNewClass = (ResultClass*)malloc(sizeof(ResultClass));
	if (pNewClass != NULL) {
		pNewClass->nbCorrect = 0;
		pNewClass->nbTotal = 0;
		pNewClass->numClass = numClass;
		pNewClass->pSuiv = NULL;
	}
	return pNewClass;
}

double toPercent(double ratio) {
	return ratio * 100.0;
}

double ratio(int num, int den) {
	if (den == 0) return 0;
	return (double)num / den;
}

bool correctEstimation(int real, int estimation) {
	return real == estimation;
}

void freeResultClassLinkedList(ResultClass* pFirstResult)
{
	ResultClass* pClass = pFirstResult;

	while (pClass != NULL) {
		ResultClass* pClassNext = pClass->pSuiv;
		free(pClass);

		pClass = pClassNext;
	}
}

int statsClass(int realClasses[], int estimateClasses[], int nbTests, int tabStats[NB_CLASSES_MAX][NB_CLASSES_MAX]) {
	int nbMax = 0;

	for (int i = 0; i < nbTests; i++) {
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
	int tabStats[NB_CLASSES_MAX][NB_CLASSES_MAX] = { 0 };
	int nbMax = statsClass(realClasses, estimateClasses, nbTests, tabStats);

	printf("\tA classer\t|");
	for (int i = 0; i < nbMax; i++) {
		printf("%d\t", i + 1);
	}
	printf("\n");
	int lineLength = 25 + 7 * nbMax;
	for (int i = 0; i < lineLength; i++)
	{
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
	for (int i = 0; i < taille; i++) {
		sum += tab[i];
	}
	return sum;
}
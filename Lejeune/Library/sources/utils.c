#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "../headers/library.h"

double getField2(char line[], int num)
{
  char lineCpy[LINE_LENGTH_MAX];
  strcpy_s(lineCpy, LINE_LENGTH_MAX, line); 
  const char* tok;
  char* nextToken;
  for (tok = strtok_s(lineCpy, ",", &nextToken);
    tok && *tok;
    tok = strtok_s(NULL, ",\n", &nextToken))
  {
    if (!--num) {
      return atof(tok);
    }
  }
  return NAN;
}

double getField(char line[], int num) 
{
  int nbComma = 0;
  int iChar = 0;
  while (line[iChar] != '\0' && nbComma < num - 1) {
    if (line[iChar] == ',') nbComma++;
    iChar++;
  }
  if (line[iChar] == '\0') return NAN;
  char field[128];
  int size = 0;
  while (line[iChar + size] != '\0' && line[iChar + size] != ',') {
    field[size] = line[iChar + size];
    size++;
  }
  field[size] = '\0';
  return atof(field);
}

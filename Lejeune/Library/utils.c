#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "library.h"

double getField(char line[], int num)
{
  char lineCpy[LINE_LENGTH_MAX];
  strcpy_s(lineCpy, LINE_LENGTH_MAX, line);
  const char* tok;
  char* nextToken;
  for (tok = strtok_s(lineCpy, ",", &nextToken);
    tok && *tok;
    tok = strtok_s(NULL, ",\n", &nextToken))
  {
    if (!--num)
      return atof(tok);
  }
  return NAN;
}


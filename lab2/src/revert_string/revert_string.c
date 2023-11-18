#include "revert_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void RevertString(char *str)
{
	char* temp;
    temp = (char*)malloc(sizeof(char) * (strlen(str) + 1));
    int n = strlen(str);
    for (int i=0; i<strlen(str); i++)
    {
      temp[i] = str[strlen(str) - i -1];
    }
    strcpy(str,temp);
    free(temp);
}
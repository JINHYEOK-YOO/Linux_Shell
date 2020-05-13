#include <errno.h>
#include <stdlib.h>
#include <string.h>

int makeargv(const char* s, const char* delimiters, char ***argvp)
{
	int i;
	int numtokens = 0;
	const char* snew;
	char* t;

	*argvp = NULL;
	snew = s + strspn(s, delimiters);
	t = malloc(strlen(snew) + 1);
	strcpy(t, snew);
	if (strtok(t, delimiters) != NULL)
	{
		numtokens = 1;
		while(strtok(NULL, delimiters) != NULL)
			numtokens++;
	}
	*argvp = malloc((numtokens + 1) * sizeof(char*));
	if (numtokens == 0)
		free(t);
	else
	{
		strcpy(t, snew);
		**argvp = strtok(t, delimiters);
		for (i = 1; i < numtokens; i++)
			*((*argvp) + i) = strtok(NULL, delimiters);
	}
	*((*argvp) + numtokens) = NULL;
	return numtokens;
}

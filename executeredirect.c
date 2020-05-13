#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int makeargv(const char* s, const char* delimiters, char*** argvp);
int myls();

int parseandredirectin(char * cmd)
{
	int infd;
	char* infile;

	if ((infile = strchr(cmd, '<')) == NULL)
		return 0;
	*infile = 0;
	infile = strtok(infile + 1, " ");
	if (infile == NULL)
		return 0;
	if ((infd = open(infile, O_RDONLY)) == -1)
		return -1;
	if (dup2(infd, STDIN_FILENO) == -1)
	{
		close(infd);
		return -1;
	}
	return close(infd);
}

int parseandredirectout(char *cmd)
{
	int outfd;
	char* outfile;

	if ((outfile = strchr(cmd,  '>')) == NULL)
		return 0;
	*outfile = 0;
	outfile = strtok(outfile + 1, " ");
	if (outfile == NULL)
		return 0;
	if ((outfd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0664)) == -1)
		return -1;
	if (dup2(outfd, STDOUT_FILENO) == -1)
	{
		close(outfd);
		return -1;
	}
	return close(outfd);
}

void executeredirect(char* s, int in, int out)
{
	char** chargv;
	char* pin;
	char* pout;

	if (in && ((pin = strchr(s, '<')) != NULL) && out && ((pout = strchr(s, '>')) != NULL))
	{
		if (pin > pout)
		{
			if (parseandredirectin(s) == -1)
			{
				perror("Failed to redirect input");
				return;
			}
			in = 0;
		}
	}
	if (out && (parseandredirectout(s) == -1))
		perror("Failed to redirect output");
	else if (in && (parseandredirectin(s) == -1))
		perror("Failed to redirect input");
	else if (makeargv(s, " ", &chargv) <= 0)
		perror("Failed to parse command line");
	else
	{
		if ((chargv[1] == NULL) && (strcmp(chargv[0], "ls") == 0))
		{
			myls();
			exit(1);
		}	
		else
		{
			execvp(chargv[0], chargv);
			perror("Failed to execute commmand");
		}
	}
	exit(1);
}

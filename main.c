#include <limits.h>
#include <setjmp.h>
#include <signal.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "getch.h"

void executecmd(char* incmd);
int signalsetup(struct sigaction* def, sigset_t* mask, void (*handler)(int));
int myls();

int main(void)
{
	char* backp;
	sigset_t blockmask;
	pid_t childpid;
	struct sigaction defhandler;
	int background;
	char inbuf[MAX_CANON];
	char ch;
	int len;
	char* tabstr;
	char pwd[255];
	

	if (signalsetup(&defhandler, &blockmask, SIG_IGN) == -1)
	{
		perror("Failed to set up shell signal handling");
		return 1;
	}
	if (sigprocmask(SIG_BLOCK, &blockmask, NULL) == -1)
	{
		perror("Failed to block signals");
		return 1;
	}
	while(1)
	{
		memset(inbuf, 0, sizeof(inbuf));
		if (fputs("User Shell >> ", stdout) == EOF)
			continue;
		do
		{
			ch = getch();
			len = strlen(inbuf);
			if (ch == '\t')
			{
				if ((tabstr = strchr(inbuf, ' ')) == NULL)
					continue;
				tabstr = tabstr + 1;
				if ((ch = getch()) == '\t')
				{
					if((getcwd(pwd, 255) == NULL) || (chdir(tabstr) == -1))
						continue;
					printf("\n");
					myls();
					chdir(pwd);
					fputs("User Shell >> ", stdout);
					fputs(inbuf, stdout);
				}
			}
			else if (ch == 127)
			{
				if (len > 0)
				{
					inbuf[len - 1] = 0;
					printf("\b");
					printf(" ");
					printf("\b");
				}
			}
			else if ((ch == '\n') || ((ch >= 32) && (ch <= 126)))
			{
				inbuf[len] = ch;
				printf("%c", ch);
			}
		} while (ch != '\n');
		inbuf[len] = 0;
		if (strcmp(inbuf, "q") == 0)
			break;
		if ((backp = strchr(inbuf, '&')) != NULL)
		{
			background = 1;
			*backp = 0;
		}
		else
			background = 0;
		if (sigprocmask(SIG_BLOCK, &blockmask, NULL) == -1)
			perror("Failed to block signals");
		if ((childpid = fork()) == -1)
			perror("Failed to fork");
		else if (childpid == 0)
		{
			if (background && (setpgid(0, 0) == -1))
				return 1;
			if ((sigaction(SIGINT, &defhandler, NULL) == -1) ||
			(sigaction(SIGQUIT, &defhandler, NULL) == -1) ||
			(sigprocmask(SIG_UNBLOCK, &blockmask, NULL) == -1))
			{
				perror("Failed to set signal handling for command");
				return 1;
			}
			executecmd(inbuf);
			return 1;
		}
		if (!background)
			waitpid(childpid, NULL, 0);
		while (waitpid(-1, NULL, WNOHANG) > 0);
	}
	return 0;
}

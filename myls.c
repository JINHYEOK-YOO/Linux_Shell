#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

int myls()
{
	char pwd[255];
	DIR* dirp;
	struct dirent* direntp;
	int count = 0;

	if (getcwd(pwd, 255) == NULL)
	{
		perror("Failed to get pwd.");
		return 1;
	}
	if ((dirp = opendir(pwd)) == NULL)
	{
		perror("Failed to open current directory.");
		return 1;
	}
	while ((direntp = readdir(dirp)) != NULL)
	{
		if (direntp->d_name[0] == '.')
			continue;
		else
		{
			printf("%-20s", direntp->d_name);
			count++;
		}
		if ((count % 4) == 0)
			printf("\n");
	}
	printf("\n");
	closedir(dirp);
	return 0;
}

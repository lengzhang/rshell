#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <array>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int EXEC (char * args[]);
int do_EXEC (char * args);

int main (int argc, char * argv[])
{
	std :: string cmdStr;
	do
	{
		std :: cout << "$ ";
		std :: getline (std :: cin, cmdStr);

		if (!cmdStr.empty())
		{
			char * cmdCStr = new char[cmdStr.length () + 1];
			std :: strcpy (cmdCStr, cmdStr.c_str());
			cmdCStr[cmdStr.length ()] = '\0';
			printf ("%s\n", cmdCStr);
			do_EXEC (cmdCStr);
		}
	} while (!cmdStr.empty());
	return 0;
}

int EXEC (char * args[])
{
	//Execute the command
	pid_t c_pid, pid;
	int status = 0;
	c_pid = fork ();
	
	if (c_pid < 0)
	{
		perror ("Fork Failed");
		exit (1);
	}
	else if (c_pid == 0)
	{
		printf ("Executing: %s\n", args[0]);
		execvp (args[0], args);
		perror ("Execve Faild");
		exit (1);
	}
	else if (c_pid > 0)
	{
		if ((pid = wait (&status)) < 0)
		{
			perror ("wait");
			exit (1);
		}
		return status;
	}

	return status;
}

int do_EXEC (char * args)
{
	char * tmpARGS = args;
	std :: vector <char *> args_vector;
	char * pch = strtok (tmpARGS, " ");
	while (pch != NULL)
	{
		args_vector.push_back (pch);
		pch = strtok (NULL, " ");
	}
	args_vector.push_back (0);
	int args_size = args_vector.size ();
	char * args_result[10];
	for (int i = 0; i < args_size; i++)
	{
		args_result[i] = args_vector[i];
	}
	
	EXEC(args_result);







	return 0;
}

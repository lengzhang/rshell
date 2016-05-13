#include <iostream>
#include <cstring>
#include <string>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int EXEC (char * args[]);
int do_EXEC (char * args[]);

int main ( int argc, char * argv[] )
{
	std :: string cmdStr;
	std :: cout << "$ ";
	std :: getline (std :: cin, cmdStr);

	char * cmdCStr = new char[cmdStr.length () + 1];
	std :: strcpy (cmdCStr, cmdStr.c_str());
	cmdCStr[cmdStr.length ()] = '\0';
	printf("%s\n", cmdCStr);

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

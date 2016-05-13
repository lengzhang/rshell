#include <iostream>
#include <cstring>
#include <string>
#include <vector>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int Find_Connector(const char * cStr, const int start, char & result);
char * CStr_Copy(const char * cStr, const int start, const int end);
void Split_Command(std :: string cStr, std::vector<char *> & cmdVector, std::vector<int> & cntVector);

int Do_EXEC (char * args);
int EXEC (char * args[]);

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
			//printf ("%s\n", cmdCStr);
			//Do_EXEC (cmdCStr);

			std::vector<char *> cmdVector;
			std::vector<int> cntVector;
			Split_Command(cmdStr, cmdVector, cntVector);
			
			//Execute commands
			int last_status = 0;
			for (int i = 0; i < cmdVector.size(); ++i)
			{
				//std :: cout << cntVector[i] << std :: endl;
				//Do_EXEC (cmdVector[i]);
				if (cntVector[i] == 0)							//First cmd or after ';'
				{
					last_status = Do_EXEC (cmdVector[i]);
				}
				else if (cntVector[i] == 1 && last_status == 0)	//After '&&'
				{
					last_status = Do_EXEC (cmdVector[i]);
				}
				else if (cntVector[i] == 2 && last_status != 0)	//After '||'
				{
					last_status = Do_EXEC (cmdVector[i]);
				}
				if (last_status == -1)
				{
					//Exit program
					std :: cout << "Executing: exit" << std :: endl;
					return 0;
				}
			}
		}
	} while (1);
	return 0;
}

int Find_Connector(const char * cStr, const int start, char & result)
{
	int i = start;
	while (cStr[i] != '\0')
	{
		result = cStr[i];
		if (cStr[i] == ';')
			return i;
		else if ((cStr[i] == '&') && (cStr[i + 1] == '&'))
			return i;
		else if ((cStr[i] == '|') && (cStr[i + 1] == '|'))
			return i;
		i++;
	}
	result = '\0';
	return -1;
}

char * CStr_Copy(const char * cStr, const int start, const int end)
{
	return 0;
}

void Split_Command(std :: string cStr, std::vector<char *> & cmdVector, std::vector<int> & cntVector)
{
	cntVector.push_back(0);
	int i = 0;
	int j = 0;
	while (i != -1)
	{
		std :: string tmpStr;
		char tmpResult;
		int a = 0;
		int b = 0;
		j = Find_Connector(cStr.c_str(), i, tmpResult);
		if (j == -1)
		{
			b = -1;
		}
		else if (tmpResult == ';')
		{
			cntVector.push_back(0);
			b = j+2;
		}
		else if (tmpResult == '&')
		{
			cntVector.push_back(1);
			a = -1;
			b = j+3;
		}
		else if (tmpResult == '|')
		{
			cntVector.push_back(2);
			a = -1;
			b = j+3;
		}
		tmpStr = cStr.substr(i, j - i + a);
		char * tmpCStr = new char[tmpStr.length () + 1];
		std :: strcpy (tmpCStr, tmpStr.c_str());
		tmpCStr[tmpStr.length ()] = '\0';
		cmdVector.push_back(tmpCStr);
		i = b;
	}
}

int Do_EXEC (char * args)
{
	if (strcmp(args, "exit") == 0)
		return -1;

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
	
	return EXEC(args_result);
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

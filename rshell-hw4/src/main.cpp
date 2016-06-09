#include <iostream>
#include <cstring>
#include <string>
#include <vector>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <fcntl.h>


int Do_Commands(std :: string cmdStr);
int Find_Connector(const char * cStr, const int start, char & result);
void Split_Command(std :: string cStr, std::vector<char *> & cmdVector, std::vector<int> & cntVector);
bool Find_Redirection (char * args);
char * Cut_Comment(char * args);

int Do_EXEC (char * args);
int EXEC (char * args[]);
int Do_Redirection (char * args);

int main (int argc, char * argv[])
{
	char hostname[20];
	gethostname(hostname,256);
	char * username = getlogin();

	do
	{
		std :: string cmdStr;
		std :: cout << username << "@" << hostname << "$ ";
		std :: getline (std :: cin, cmdStr);
		Do_Commands(cmdStr);
		printf("\n");
	} while (true);
	return 0;
}

int Do_Commands(std :: string cmdStr)
{
	if (!cmdStr.empty())
	{
		char * cmdCStr = new char[cmdStr.length () + 1];
		std :: strcpy (cmdCStr, cmdStr.c_str());
		cmdCStr[cmdStr.length ()] = '\0';
	
		std::vector<char *> cmdVector;
		std::vector<int> cntVector;
		Split_Command(cmdStr, cmdVector, cntVector);
		
		//Execute commands
		int last_status = 0;
		for (unsigned int i = 0; i < cmdVector.size(); ++i)
		{
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
		}
		return last_status;
	}
	return -1;
}

int Find_Connector(const char * cStr, const int start, char & result)
{
	int i = start;
	if (cStr[i] == '(')
	{
		i++;
		while (cStr[i] != ')')
		{
			i++;
		}
	}
	while (cStr[i] != '\0')
	{
		result = cStr[i];
		if ((cStr[i] == ';') && (cStr[i + 1] == ' '))
			return i;
		else if ((cStr[i - 1] == ' ') && (cStr[i] == '&') && (cStr[i + 1] == '&') && (cStr[i + 2] == ' '))
			return i;
		else if ((cStr[i - 1] == ' ') && (cStr[i] == '|') && (cStr[i + 1] == '|') && (cStr[i + 2] == ' '))
			return i;
		i++;
	}
	result = '\0';
	return -1;
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

char * Cut_Comment(char * args)
{
	int i = 0;
	while (args[i] != '\0')
	{
		if (args[i] == '#')
		{
			char * tmp = new char[i+1];
			for (int j = 0; j < i; ++j)
			{
				tmp[j] = args[j];
			}
			tmp[i] = '\0';
			return tmp;
		}
		i++;
	}
	return args;
}

bool Find_Redirection (char * args)
{
	for (unsigned int i = 0; i < strlen(args); ++i)
	{
		if (((args[i] == ' ') && (args[i + 1] == '<') && (args[i + 2] == ' '))
			|| ((args[i] == ' ') && (args[i + 1] == '<') && (args[i + 2] == '<') && (args[i + 3] == ' '))
			|| ((args[i] == ' ') && (args[i + 1] == '<') && (args[i + 2] == ' '))
			|| ((args[i] == ' ') && (args[i + 1] == '<') && (args[i + 2] == '<') && (args[i + 3] == ' '))
			|| ((args[i] == ' ') && (args[i + 1] == '|') && (args[i + 2] == ' ')))
		{
			return true;
		}
	}
	return false;
}

int Do_EXEC (char * args)
{
	printf("\nCommand: %s\n", args);
	if (strncmp(args, "(", 1) == 0)
	{
		char * new_args = new char[strlen(args)-2];
		for (unsigned int i = 0; i < strlen(args)-2; ++i)
		{
			new_args[i] = args[i+1];
		}
		return Do_Commands(new_args);
	}
	//Redirection
	else if (Find_Redirection(args))
	{
		//std :: cout << "Found Redirection" << std :: endl;
		
		return Do_Redirection(args);
	}
	else if ((strncmp(args, "exit", 4) == 0) || (strncmp(args, "quit", 4) == 0))
	{
		//Exit program
		exit(0);
	}
	//Test Command
	else if (strncmp(args, "[", 1) == 0 || strncmp(args, "test", 4) == 0)
	{
		char * tmpARGS = Cut_Comment(args);
		std :: vector <char *> args_vector;
		char * pch = strtok (tmpARGS, " ");
		while (pch != NULL)
		{
			//If got "[", then push "test"
			if (strncmp(pch, "[", 1) == 0)
			{
				char temp[4] = {'t','e','s','t'};
				args_vector.push_back(temp);
			}
			//If got "]", then skip
			else if (strncmp(pch, "]", 1) != 0)
			{
				args_vector.push_back (pch);
			}
			pch = strtok (NULL, " ");
		}
		args_vector.push_back (0);
		//Check is there any flag
		if (strncmp(args_vector[1], "-e", 2) != 0 && strncmp(args_vector[1], "-f", 2) != 0 && strncmp(args_vector[1], "-d", 2) != 0 )
		{
			char temp[2] = {'-','e'};
			std :: vector <char *> :: iterator it;
			it = args_vector.begin();
			it++;
			args_vector.insert(it, temp);
		}

		int args_size = args_vector.size ();
		char * args_result[10];
		for (int i = 0; i < args_size; i++)
		{
			args_result[i] = args_vector[i];
		}
		int status = EXEC(args_result);
		if (status == 0)
		{
			std :: cout << "(True)" << std :: endl;
		}
		else
		{
			std :: cout << "(False)" << std :: endl;
		}
		return status;
	}
	else
	{
		char * tmpARGS = Cut_Comment(args);
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
		//printf ("Executing: %s\n", args[0]);
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

int Do_Redirection (char * args)
{
	//std :: cout << args << std :: endl;
	//std :: cout << "----------" << std :: endl;
	//printf("Commands: %s\n", args);
	int rc = 0;
	FILE *fp;

	char result_buf[1024];

	//Opened pipe to exe commands
	fp = popen(args, "r");
	if (fp == NULL)
	{
		std :: cout << "popen fail" << std :: endl;
		printf("popen fail");
		exit(1);
		return -1;
	}
	//Print result output
	while (fgets(result_buf, sizeof(result_buf), fp) != NULL)
	{
		if (result_buf[strlen(result_buf) - 1] == '\n')
		{
			result_buf[strlen(result_buf) - 1] = '\0';
		}
		printf("%s\n", result_buf);
	}

	rc = pclose(fp);

	if (rc == -1)
	{
		std :: cout << "fp close fail" << std :: endl;
		exit(1);
		return -1;
	}
	else
	{
		//std :: cout << "fp close succeed" << std :: endl;
		return 0;
	}

	/*
	std :: vector<char *> commands;
	char * pch = strtok (args, "|");
	while (pch != NULL)
	{
		//If first char is ' ', then ignore it.
		if (pch[0] == ' ')
		{
			pch++;
		}
		if (pch[strlen(pch) - 1] == ' ')
		{
			pch[strlen(pch) - 1] = '\0';
		}
		commands.push_back(pch);
		pch = strtok (NULL, "|");
	}

	for (int i = 0; i < commands.size(); ++i)
	{
		std :: cout << commands[i] << std :: endl;
	}


	int status = 0;
	int pipe_fd1[2], pipe_fd2[2];
	pipe(pipe_fd1);
	pipe(pipe_fd2);

	for (int i = 0; i < commands.size(); ++i)
	{
		std :: cout << commands[i] << std :: endl;
		char * args_result[10];
		int argc = 0;
		char * p_command = strtok (commands[i], " ");

		char * file_name = NULL;
		int file_action = 0; // 0 - Nothin; 1 - Input; 2 - Output; 3 - Output Append;

		while (p_command != NULL)
		{
			if (strncmp(p_command, "<", 1) == 0)
			{
				file_action = 1;
				p_command = strtok (NULL, " ");
				file_name = new char[strlen(p_command)];
				strcpy (file_name, p_command);
				p_command = strtok (NULL, " ");
			}
			else if (strncmp(p_command, ">>", 2) == 0)
			{
				file_action = 3;
				p_command = strtok (NULL, " ");
				file_name = new char[strlen(p_command)];
				strcpy (file_name, p_command);
				p_command = strtok (NULL, " ");
			}
			else if (strncmp(p_command, ">", 1) == 0)
			{
				file_action = 2;
				p_command = strtok (NULL, " ");
				file_name = new char[strlen(p_command)];
				strcpy (file_name, p_command);
				p_command = strtok (NULL, " ");
			}
			else
			{
				args_result[argc] = new char[strlen(p_command)];
				strcpy (args_result[argc], p_command);
				argc++;
				p_command = strtok (NULL, " ");
			}
		}
		args_result[argc] = 0;
		argc++;
		std :: cout << "----------" << std :: endl;
		for (int j = 0; j < argc - 1; ++j)
		{
			std :: cout << args_result[j] << " " << std :: endl;
		}
		if (file_name != NULL)
		{
			std :: cout << "File: " << file_name << std :: endl;
		}
		std :: cout << "----------" << std :: endl << std :: endl;


		pid_t c_pid;
		if ((c_pid = fork()) == 0)
		{
			//Redirect stdin
    		if (file_action == 1)
    		{
    			std :: cout << "In File" << std :: endl;
    			int in_fd = open(file_name, O_CREAT | O_RDONLY, 0666);
    			close(fileno(stdin));
    			dup2(in_fd, fileno(stdin));
    			close(in_fd);
    		}
    		else
    		{
    			std :: cout << "In Last" << std :: endl;
    			close(pipe_fd1[1]);
    			//close(fileno(stdin));
    			//dup2(pipe_fd1[0], fileno(stdin));
    			//close(pipe_fd1[1]);
    		}
    		if (file_action == 2)
    		{
    			
    		}
    		else if (file_action == 3)
    		{
    			
    		}
    		else
    		{
    			std :: cout << "Out Next" << std :: endl;
    			//close(pipe_fd2[0]);
    		}
    		std :: cout << args_result[0] << std :: endl;
			execvp (args_result[0], args_result);
			perror ("Execve Faild");
    		return 0;
		}
		else
		{
			close(pipe_fd1[0]);
			close(pipe_fd2[1]);
			waitpid(c_pid, &status, 0);
		}


	}

	*/
	//exit(0);
	//return 0;

}


/*
int Do_Redirection (char * args)
{
	char * tmpARGS = Cut_Comment(args);
	std :: vector <char *> args_vector;
	char * pch = strtok (tmpARGS, " ");
	while (pch != NULL)
	{
		args_vector.push_back (pch);
		pch = strtok (NULL, " ");
	}
	//args_vector.push_back (0);

	for (int i = 0; i < args_vector.size(); ++i)
	{
		std :: cout << args_vector[i] << std :: endl;
	}

	std :: cout << "----------" << std :: endl;

	char * in_file = NULL;
	char * out_file = NULL;
	char * out_append_file = NULL;

	std :: vector<char *> commands;
	int in_ite = -1;
	int out_ite = -1;
	int out_append_ite = -1;
	int ite = 0;
	for (int i = 0; i < args_vector.size();)
	{
    	if ((strncmp(args_vector[i], "<", 1) == 0) || (strncmp(args_vector[i], "<<", 2) == 0))
    	{
    		in_file = args_vector[i + 1];
    		in_ite = ite;
    		i+=2;
    	}
    	else if (strncmp(args_vector[i], ">>", 2) == 0)
    	{
    		out_append_file = args_vector[i + 1];
    		out_ite = ite;
    		i+=2;
    	}
    	else if (strncmp(args_vector[i], ">", 1) == 0)
    	{
    		out_file = args_vector[i + 1];
    		out_append_ite = ite;
    		i+=2;
    	}
    	else if (strncmp(args_vector[i], "|", 1) == 0)
    	{
    		commands.push_back(0);
    		ite++;
    		i++;
    	}
    	else
    	{
    		commands.push_back(args_vector[i]);
    		i++;
    	}
    }
    commands.push_back(0);

    std :: cout << in_ite << " " << out_ite << " " << out_append_ite<< std :: endl;

    if (in_file != NULL)
    {
    	std :: cout << "in_file: " << in_file << std :: endl;
    }
    if (out_file != NULL)
    {
    	std :: cout << "out_file: " << out_file << std :: endl;
    }
    if (out_append_file != NULL)
    {
    	std :: cout << "out_append_file: " << out_append_file << std :: endl;
    }
    for (int i = 0; i < commands.size(); ++i)
    {
    	if (commands[i] != 0)
    	{
    		std :: cout << commands[i] << std :: endl;
    	}
    	else
    	{
    		std :: cout << std :: endl;
    	}
    }
    



    int pipe_fd1[2], pipe_fd2[2];
    pid_t c_pid;
    int pid_ite = 0;

    std :: vector <char *> temp_commands;
	int status = 0;

	pipe(pipe_fd1);
	pipe(pipe_fd2);

    for (int i = 0; i < commands.size(); ++i)
    {
    	if (commands[i] != 0)
    	{
    		temp_commands.push_back(commands[i]);
    	}
    	else
    	{
    		temp_commands.push_back(0);
    		char * args_result[10];
    		for (int j = 0; j < temp_commands.size(); ++j)
    		{
    			args_result[j] = temp_commands[j];
    		}
    		std :: cout << "----------" << std :: endl;
    		int k = 0;
    		while (args_result[k] != 0)
    		{
    			std :: cout << args_result[k] << " ";
    			k++;
    		}
    		std :: cout << std :: endl;
    		temp_commands.clear();

    		if ((c_pid = fork()) == 0)
    		{
    			//Redirect stdin
    			if (pid_ite == in_ite)
    			{
    				std :: cout << "In File" << std :: endl;
    				int in_fd = open(in_file, O_CREAT | O_RDONLY, 0666);
    				close(fileno(stdin));
    				dup2(in_fd, fileno(stdin));
    				close(in_fd);
    			}
    			else
    			{
    				std :: cout << "In Last" << std :: endl;
    				read(pipe_fd1[0]);
    				//close(fileno(stdin));
    				//dup2(pipe_fd1[0], fileno(stdin));
    				//close(pipe_fd1[1]);
    			}

    			//Redirect stdout to pipe
    			if (pid_ite == out_ite)
    			{
    				std :: cout << "Out File" << std :: endl;
    				int out_fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    				close(fileno(stdout));
    				dup2(out_fd, fileno(stdout));
    				close(out_fd);
    			}
    			else if (pid_ite == out_append_ite)
    			{
    				std :: cout << "Out Append File" << std :: endl;
    				int out_append_fd = open(out_append_file, O_WRONLY | O_CREAT | O_APPEND, 0666);
    				close(fileno(stdout));
    				dup2(out_append_fd, fileno(stdout));
    				close(out_append_fd);
    			}
    			else
    			{
    				std :: cout << "Out Next" << std :: endl;
    				write(pipe_fd2[1]);
    				//close(fileno(stdout));
    				//dup2(pipe_fd2[1], fileno(stdout));
    				//close(pipe_fd2[0]);
    				//close(pipe_fd2[0]);
    				//pipe_fd2[1] = dup(fileno(stdout));
    			}

    			execvp (args_result[0], args_result);
				perror ("Execve Faild");
    			return 0;
    		}
    		
    		waitpid(c_pid, &status, 0);

    		close(pipe_fd1[1]);
    		dup2(pipe_fd2[0], pipe_fd1[1]);
    		close(pipe_fd2[0]);

    		pid_ite++;
    	}
    }

    std :: cout << "----------" << std :: endl;
	exit(0);
	return status;
}
*/










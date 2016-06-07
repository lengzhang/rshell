#include <iostream>
#include <cstring>
#include <string>
#include <vector>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int Do_Commands(std :: string cmdStr);
int Find_Connector(const char * cStr, const int start, char & result);
void Split_Command(std :: string cStr, std::vector<char *> & cmdVector, std::vector<int> & cntVector);
char * Cut_Comment(char * args);

int Do_EXEC (char * args);
int EXEC (char * args[]);

int main (int argc, char * argv[])
{
    char hostname[20];
    gethostname(hostname,256);

    char * username = getlogin();

    std :: string cmdStr;
    do
    {
        std :: cout << username << "@" << hostname << "$ ";
        std :: getline (std :: cin, cmdStr);
        Do_Commands(cmdStr);
        
    } while (1);

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
        for (int i = 0; i < cmdVector.size(); ++i)
        {
            if (cntVector[i] == 0)                          //First cmd or after ';'
            {
                last_status = Do_EXEC (cmdVector[i]);
            }
            else if (cntVector[i] == 1 && last_status == 0) //After '&&'
            {
                last_status = Do_EXEC (cmdVector[i]);
            }
            else if (cntVector[i] == 2 && last_status != 0) //After '||'
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

int Do_EXEC (char * args)
{
    //std :: cout << args << std :: endl;

    if (strncmp(args, "(", 1) == 0)
    {
        char * new_args = new char[strlen(args)-2];
        for (int i = 0; i < strlen(args)-2; ++i)
        {
            new_args[i] = args[i+1];
        }
        return Do_Commands(new_args);
    }
    else if ((strncmp(args, "exit", 4) == 0) || (strncmp(args, "quit", 4) == 0))
    {
        //Exit program
        std :: cout << "Executing: exit" << std :: endl;
        exit(0);
    }
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

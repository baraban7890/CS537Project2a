
#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char error_message[30] = "An error has occurred\n";
char searchPath[100][100];
int pathIndex = 0;

char* getAndRemoveNewline(char *string)
{
  int i = 0;
  while(string[i] != '\0')
    {
      if(string[i] == '\n')
        {
          string[i] = '\0';
        }
      i+=1;
    }
  return string;
}

int checkIfBuiltIn(char *line)
{
  if(strcmp(line,"exit\n") == 0 || strcmp(line, "exit") == 0) //checks if command is exit                                                                                                                   
    {
      exit(0);
    }
  else //checks if command is cd or path                                                                                                                                                                    
    {
      char *found;
      char *found2;
      int numCommands = 0;
      char *command;
      char *copy = malloc(1000);
      copy = strcpy(copy,line);
      char *arguments[100];
      int i = 0;
      while( (found = strsep(&copy, " ")) != NULL)
        {
          if(numCommands == 0)
            {
              command = found;
            }
          else
            {
              arguments[i]  = found;
              i++;
            }
          numCommands++;
        }
          if(strcmp(command, "cd") == 0 && numCommands == 2)
        {
          getAndRemoveNewline(arguments[0]);
          if(chdir(arguments[0]) == 0)
            {
              return 0;
            }
          else
            {
              write(STDERR_FILENO, error_message, strlen(error_message));
              exit(0);
            }
        }
          else if(strcmp(command,"exit") == 0)
            {
          write(STDERR_FILENO, error_message, strlen(error_message));
          exit(0);
            }
      else if(((strcmp(command, "cd\n") == 0 || strcmp(command, "cd") == 0) && numCommands != 2))
        {
          write(STDERR_FILENO, error_message, strlen(error_message));
          exit(0);
        }
      else if((strcmp(command, "path") == 0 && numCommands > 1))
        {
          int k = 0;
          pathIndex = 0;
          while(k < numCommands - 1)
            {
              strcpy(searchPath[pathIndex],arguments[k]);
              k++;
              pathIndex++;
            }
          return 0;
        }
      else if((strcmp(command, "path\n") == 0) || strcmp(command,"path") == 0)
        {
          strcpy(searchPath[0],"");
          return 0;
        }
    }
  return -1;
}

int handleIfFi(char *line)
{
  char *fiStatement = malloc(1000);
  char *operation = malloc(1000);
  char *operator = malloc(1000);
  char *operand = malloc(1000);
  char *copy = malloc(1000);
  copy = strcpy(copy,line);
  char *found = malloc(1000);
  int ifInd = 0;
  int fiInd = 0;
  int i = 0;
  int found2 = -1;
  int j = 0;
  char arguments[100][100];
  int retval;
  char argFI[100][100];
  int k = 0;
  char *argv_for_program2[4];
  while( (found = strsep(&copy, " ") ) != NULL)
    {
      if(fiInd == 1)
        {
	  if(strcmp(found,"fi") == 0)
	    {
	      fiInd = 0;
	    }
	  else
	    {
	      if(j == 0)
		{
		  strcpy(fiStatement,found);
		}
	      ifInd = 0;
	    }
	  j++;
        }
      if(strcmp(found,"then") == 0)
        {
          fiInd = 1;
          ifInd	= 0;
        }

      if(ifInd == 1)
        {
          if(i == 0)
            {
              strcpy(operator,found); //the command
	      i++;
            }
	  else if(i == 1 && (strcmp(found,"==") == 0 || strcmp(found,"!=") == 0))
	    {
	      strcpy(operation,found);
	      i++;
	    }
	  else if(i == 1 && (strcmp(found,"==") !=0 && strcmp(found,"!=") != 0))
	    {
	      strcpy(arguments[0],found);
	    }
	  else if(i==2)
	    {
	      strcpy(operand,found);
	      i++;
	    }
        }
      if(strcmp(found,"if") == 0)
        {
          ifInd = 1;
	  found2 = 0;
        }
    }
  argv_for_program2[j] = NULL;
  int child_status;
  char *argv_for_program[3];
  if(arguments[0] != NULL)
    {
      argv_for_program[0] = operator;
      argv_for_program[1] = arguments[0];
      argv_for_program[2] = NULL;
    }
    else
      {
	argv_for_program[0] = operator;
	argv_for_program[1] = NULL;
	argv_for_program[2] = NULL;
      }
  pid_t childpid = fork();
  if(childpid == 0)
    {
      execv(operator, argv_for_program);
      exit(0);
    }
  else
    {
      wait(&child_status);
      retval = WEXITSTATUS(child_status);
    }
  if(strcmp(operation, "==") == 0)
    {
      if(retval == atoi(operand))
	{
	      int child_status2;
	      pid_t childpid2 = fork();
	      if(childpid2 == 0)
		{
		  found2 = 0;
		  execv(fiStatement, argv_for_program2);
		  exit(0);
		}
	      else
		{
		  wait(&child_status2);
		}
	}
    }
  if(strcmp(operation, "!=") == 0)
    {
      if(retval != atoi(operand))
	{
	      int child_status2;
	      pid_t childpid2 = fork();
	      if(childpid2 == 0)
		{
		  execv(fiStatement, argv_for_program2);
		  exit(0);
		}
	      else
		{
		  found2 = 0;
		  wait(&child_status2);
		}
	}
    }
  return found2;
}

char *removeWhitespace(char *line)
{
  char *newString = malloc(1000);
  char *copy = malloc(1000);
  copy = strcpy(copy,line);
  int i = 0;
  int j = 0;
  if(strlen(copy) == 0)
    {
      return "";
    }
  else if(strlen(copy) == 1)
    {
      if(copy[0] == ' ' || copy[0] == '\t')
	{
	  return "";
	}
      else
	{
	  return copy;
	}
    }
  else
    {
      while(copy[i] == ' ' || copy[i] == '\t')
	{
	  i++;
	}

      while(copy[i+1] != '\0')
	{
	  if(copy[i] == ' ' && copy[i+1] > 32 && i != 0) // if first character is not whitespace
	    {
	      newString[j] = copy[i];
	      j++;
	    }
	  else if((copy[i] == ' ' || copy[i] == '\t') && i == 0) // first character is whitespace
	    {

	    }
	  else if(copy[i]> 32)
	    {
	      newString[j] = copy[i];
	      j++;
	    }
	  else if((copy[i+1] > 32 && copy[i] == ' ') || (copy[i+1] > 32 && copy[i] == '\t'))
	    {
	      newString[j] = ' ';
	      j++;
	    }
	  else if(copy[i] == ' ' && (copy[i+1]  == ' '))
	    {
	    }
	  else if((copy[i] == ' ' && copy[i+1] == '\t') || (copy[i] == '\t' && copy[i] == '\t') || (copy[i] == '\t' && copy[i+1] == ' '))
	    {
	    }
	  else if((copy[i] == ' ' || copy[i] == '\t') && i == strlen(copy)-1)
	    {
	      
	    }
	  else
	    {
	      newString[j] = copy[i];
	      j++;
	    }
	  i++;
	}
    }
  if(newString[strlen(newString)-1] == ' ' || newString[strlen(newString)- 1] == '\t')
    {
      newString[strlen(newString)-1]  = '\0';
    }
  return newString;
}

char *getFirstRedirect(char *line)
{
  char *copy = malloc(1000);
  char  *found = malloc(1000);
  char *first = malloc(1000);
  copy = strcpy(copy,line);
  int i = 0;
  while( (found = strsep(&copy,">")) && i == 0)
    {
      if(i == 0)
	{
	  strcpy(first,found);
	}
      i++;
    }
  return first;
}

char *getSecondRedirect(char *line)
{
  char *copy = malloc(1000);
  char *found = malloc(1000);
  char *second = malloc(1000);
  copy = strcpy(copy,line);
  int i = 0;
  while( (found = strsep(&copy,">")) && i < 2)
    {
      if(i == 1)
	{
	  strcpy(second,found);
	}
      i++;
    }
  return second;
}
/*
char* getAndRemoveNewline(char *string)
{
  int i = 0;
  while(string[i] != '\0')
    {
      if(string[i] == '\n')
	{
	  string[i] = '\0';
	}
      i+=1;
    }
  return string;
}
*/
int containsWhiteSpace(char *string)
{
  int i = 0;
  while(string[i] != '\0')
    {
      if(string[i] == ' ' || string[i] == '\t')
	{
	  return 1;
	}
      i += 1;
    }
  return 0;
}
/*
int checkIfBuiltIn(char *line)
{
  if(strcmp(line,"exit\n") == 0 || strcmp(line, "exit") == 0) //checks if command is exit
    {
      exit(0);
    }
  else //checks if command is cd or path
    {
      char *found;
      char *found2;
      int numCommands = 0;
      char *command;
      char *copy = malloc(1000);
      copy = strcpy(copy,line);
      char *arguments[100];
      int i = 0;
      while( (found = strsep(&copy, " ")) != NULL)
	{
	  if(numCommands == 0)
	    {
	      command = found;
	    }
	  else
	    {
	      arguments[i]  = found;
	      i++;
	    }
	  numCommands++;
	}
	  if(strcmp(command, "cd") == 0 && numCommands == 2)
	{
	  getAndRemoveNewline(arguments[0]);
	  if(chdir(arguments[0]) == 0)
	    {
	      return 0;
	    }
	  else
	    {
	      write(STDERR_FILENO, error_message, strlen(error_message));
	      exit(0);
	    }
	}
	  else if(strcmp(command,"exit") == 0)
	    {
          write(STDERR_FILENO, error_message, strlen(error_message));
          exit(0);
	    }
      else if(((strcmp(command, "cd\n") == 0 || strcmp(command, "cd") == 0) && numCommands != 2))
	{
	  write(STDERR_FILENO, error_message, strlen(error_message));
	  exit(0);
	}
      else if((strcmp(command, "path") == 0 && numCommands > 1))
	{
	  int k = 0;
	  pathIndex = 0;
	  while(k < numCommands - 1)
	    {
	      strcpy(searchPath[pathIndex],arguments[k]);
	      k++;
	      pathIndex++;
	    }
	  return 0;
	}
      else if((strcmp(command, "path\n") == 0) || strcmp(command,"path") == 0)
	{
	  strcpy(searchPath[0],"");
	  return 0;
	}
    }
  return -1;
}
*/
int handleNoArgumentCommand(char *line, int len)
{
  int i = 0;
  int found = 0;
  char *copy = malloc(1000);
  copy = getAndRemoveNewline(line);
		  while(i < pathIndex && found == 0)
		    {
		      int child_status;
		      char *filename;
		      char bin[100];
		      strcpy(bin,searchPath[i]);
		      strcpy(bin,getAndRemoveNewline(bin));
		      strcat(bin,"/");
		      strcat(bin,copy);
		      if((access(bin, X_OK) == 0))
		      {
			filename = bin;
			char *argv_for_program[] = { filename, NULL };
			pid_t childpid = fork();
			if(childpid == 0)
                        {
			  execv(filename, argv_for_program);
			  exit(0);
                        }
                      else
                        {
                            wait(&child_status);
                        }
			found = 1;
		      }

		      i++;

		    }
		  if(found == 1)
		    {
		      return 0;
		    }
		  else
		    {
		      return -1;		    }
			
}


int handleCommandsWithArguments(char *line, int len)
{
      char *found;
      char *found1;
      int found2 = 0;
      int numWords = 0;
      char *command;
      char *redirect = malloc(1000);
      char *redirectCheck = malloc(1000);
      redirectCheck = strcpy(redirectCheck,line);
      char *copy = malloc(1000);
      char *copy2 = malloc(1000);
      copy = strcpy(copy,line);
      char *arguments[100];
      int numArgs = 0;
      char *redirectFile;
      int numFiles = 0;
      int ifRedirect = 0;
      char *first = malloc(1000);
      char *second = malloc(1000);
      if(strcmp(line,strsep(&redirectCheck,">")) != 0)
        {
          redirect = strcpy(redirect,line);
          first = getFirstRedirect(line);
          second = getSecondRedirect(line);
	  second = getAndRemoveNewline(second);
          ifRedirect = 1;
          copy = strcpy(copy,first);
          if(second == NULL || strcmp(second,"") == 0 || containsWhiteSpace(second) == 1)
            {
              write(STDERR_FILENO, error_message, strlen(error_message));
              exit(0);
            }
        }
      while( (found = strsep(&copy, " ")) != NULL)
        {
          if(numWords == 0)
            {
              command = found;
            }
          else
            {
              arguments[numArgs]  = found;
              numArgs++;
            }
	  numWords++;
        }
      int i = 0;
      while(i < pathIndex && found2 == 0)
	{
	  int child_status;
	  char *filename;
	  char bin[100];
	  strcpy(bin,searchPath[i]);
	  strcpy(bin,getAndRemoveNewline(bin));
	  strcat(bin,"/");
	  strcat(bin,command);
	  if((access(bin, X_OK) == 0))
	    {
	      filename = bin;
	      char *argv_for_program[5];
	      argv_for_program[0] = command;
	      int j = 0;
	      while(j < numArgs)
		{
		  argv_for_program[j+1] = arguments[j];
		  j++;
		}
	      argv_for_program[j] = getAndRemoveNewline(argv_for_program[j]);
	      argv_for_program[j+1] = NULL;
	      pid_t childpid = fork();
	      if(childpid == 0)
		{
		if(ifRedirect == 1)
		  {
		    int fd = open(second, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		    dup2(fd, 1);
		    close(fd);
		  }
		execv(filename, argv_for_program);
		exit(0);
		}
	    
	      else
		{
		  wait(&child_status);
		  found2 = 1;
		}
	    }
	  i++;
	}
      if(found2 == 1)
	{
	  return 0;
	}
      else
	{
	  return -1;
	}
}


int main(int argc, char *argv[])
{
  strcpy(searchPath[0],"/bin");
  pathIndex = 1;
    if(argc > 2)
    {
      write(STDERR_FILENO, error_message, strlen(error_message));   
      exit(1);
    }
    else if(argc == 2)
      {
	//if some_command == 0 then some_other_command fi
      char* filename;
      filename = argv[1];
      FILE *fp;
      fp = fopen(filename, "r");
      if(fp == NULL) {
	write(STDERR_FILENO, error_message, strlen(error_message));
	exit(1);
      }
      char string[100];
      int len = 100;
      while(fgets(string,len,fp))
	{
	  strcpy(string,removeWhitespace(string));
	  if(strlen(string) == 0)
	    {

	    }
	  // printf("%s",string);
	  else
	    {
	      if(containsWhiteSpace(string) == 0) // if it has no arguments
                {
                  if(checkIfBuiltIn(string) == -1) //if command is not built in 
                    {
		      if( handleNoArgumentCommand(string,len) == -1)
			{
			  write(STDERR_FILENO, error_message, strlen(error_message));
			}
                    }
                }
	      else // if it is a command with arguments
		{
		  if(checkIfBuiltIn(string) == -1)
		    {
		      if(handleCommandsWithArguments(string,len) == -1)
			{
			  if(handleIfFi(string) == -1)
			    {
			      write(STDERR_FILENO, error_message, strlen(error_message));
			    }
			}
		    }
		}
	    }
	}
      //read in from batch file
      exit(0);
    }
    else
      {
	char *found;
	size_t len = 100;
	char *line;
	do
	  {
	    printf("wish> ");
	    getline(&line, &len, stdin);
	    line = strcpy(line,removeWhitespace(line));
	    if(strlen(line) == 0)
	      {
		
	      }
	    else
	      {
		if(containsWhiteSpace(line) == 0) //if it is a 0 argument command
		  {
		    if(checkIfBuiltIn(line) == -1)
		      {
			if( handleNoArgumentCommand(line,len)==-1)
			  {
			    write(STDERR_FILENO, error_message, strlen(error_message));
			}
		      }
		  }
		else //command has arguments
		  {
		    if(checkIfBuiltIn(line) == -1)
		      {
			if(handleCommandsWithArguments(line,len) == -1)
			  {
			    if(handleIfFi(line) == -1)
			      {
			      write(STDERR_FILENO, error_message, strlen(error_message));
			      }
			  }
		      }
		  }
	      }
	  }
	while(strcmp(line,"exit\n") != 0);
	/*access("/bin/ls", X_OK)*/
      }
    return 0;
}


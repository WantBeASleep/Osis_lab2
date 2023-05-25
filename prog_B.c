#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int countCmds(char* str);

int main()
{
  printf("Type the comand line: ");
  char input_line[999];
  fgets(input_line, 999, stdin);

  char *p;
  if ((p = strchr(input_line, '\n')) != NULL) *p = '\0';

  int ppid = getppid();
  kill(ppid, SIGUSR1);

  // 2 канала, которые попеременно будут менять друг друга сменять для связи
  int fd1[2], fd2[2];

  int cmdCounter = countCmds(input_line);

  char* strDelimCmd = strtok(input_line, "|");
  for (int i = 0; i < cmdCounter; i++)
  {
    if (i)
    {
      strDelimCmd = strtok(NULL, "|");
      fd1[0] = fd2[0];
      fd1[1] = fd2[1];
    }
    pipe(fd2);

    int pid = fork();
    // дочерний процесс
    if (!pid)
    {
      // работа с каналом связи пред.
      if (i) close(fd1[1]);
      if (i) dup2(fd1[0], STDIN_FILENO);
      
      //работа с каналом связи след.
      close(fd2[0]);
      if (i != cmdCounter - 1) dup2(fd2[1], STDOUT_FILENO);

      // разбиение на аргументы
      char** argv;
      int argsCount = 0;
      char* strDelimArgs = strtok(strDelimCmd, " ");
      while(strDelimArgs)
      {
        if (!argsCount) argv = (char**)malloc(sizeof(char*));
        else argv = (char**)reallocarray(argv, argsCount + 1, sizeof(char*));
        argv[argsCount] = strDelimArgs;
        argsCount++;
        strDelimArgs = strtok(NULL, " ");
      }
      argv = (char**)reallocarray(argv, argsCount + 1, sizeof(char*));
      argv[argsCount] = NULL;

      execvp(argv[0], argv);
    }
    // родительский процесс
    if (i) close(fd1[0]);
    close(fd2[1]);

    wait(NULL);
  }
  return 0;
}

int countCmds(char* str)
{
  int count = 0, i = 0;
  while(str[i])
  {
    if (str[i] == '|')
    {
      count++;
    }
    i++;
  }
  return count + 1;
}



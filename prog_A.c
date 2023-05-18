#include <signal.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>

#include <time.h>

#include <stdlib.h>

#define WAIT_TIME 1

pid_t childPid;

void handler(int sig)
{
  clock_t clockStart = clock();
  while ((float)(clock() - clockStart ) / CLOCKS_PER_SEC < WAIT_TIME) 
  {
    if (waitpid(childPid, NULL, WNOHANG) == childPid) 
    {
      printf("Succesfull end!\n");
      exit(0);
    }
  }
  if (waitpid(childPid, NULL, WNOHANG) == 0) {
    kill(childPid, SIGTERM);
  }
}

int main()
{
  signal(SIGUSR1, &handler);
  char* argv[] = {"./prog_B", NULL};

  childPid = fork();
  if (!childPid) execvp(argv[0], argv);

  wait(NULL);
  return 0;
}
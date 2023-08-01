/***** LAB3 base code *****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

char home[128];    // home directory
char **env1;

char gpath[128];    // hold token strings
char *name[64];     // token string pointers
int  n;             // number of token strings

char dpath[128];    // hold dir strings in PATH
char *dir[64];      // dir string pointers
int  ndir;          // number of dirs

int tokenize(char *pathname, char *delim, char *path, char *strings[], int *count) // YOU have done this in LAB2
{
  // YOU better know how to apply it from now on
  char *s;
  strcpy(path, pathname);   // copy into global gpath[]
  s = strtok(path, delim);
  *count = 0;
  while(s)
  {
    strings[(*count)++] = s;           // token string pointers
    s = strtok(0, delim);
  }
  strings[*count] =0;                // strings[n] = NULL pointer
}

int main(int argc, char *argv[ ], char *env[ ])
{
  //finds the path variable, tokenizes it into dir array
  for (int i = 0; env[i] != NULL; ++i)
  {
    if (strncmp(env[i], "PATH=", 5) == 0)
      tokenize(env[i] + 5, ":", dpath, dir, &ndir);
    else if (strncmp(env[i], "HOME=", 5) == 0)
      strcpy(home, env[i] + 5);
  }

  //printf("%s\n", home);

  env1=env;
  int i;
  int pid, status;
  char *cmd;
  char line[128];

  // The base code assume only ONE dir[0] -> "./"
  // YOU do the general case of many dirs from PATH !!!!
  //dir[0] = "./";
  //ndir=1;

  //add local path
  dir[ndir++] = "./";

  // show dirs
  for(i=0; i<ndir; i++)
    printf("dir[%d] = %s\n", i, dir[i]);

  while(1)
  {
    printf("sh %d running\n", getpid());
    printf("enter a command line : ");
    fgets(line, 128, stdin);
    line[strlen(line) - 1] = 0;
    if (line[0]==0)
      continue;

    tokenize(line, " ", gpath, name, &n);

    for (i=0; i<n; i++)   // show token strings
    {
      printf("name[%d] = %s\n", i, name[i]);
    }

    //    printf("enter a key to continue : "); getchar();

    cmd = name[0];         // line = name0 name1 name2 ....

    if (strcmp(cmd, "cd")==0)
    {
      if (name[1] == NULL)
        chdir(home);
      else
        chdir(name[1]);
      continue;
    }
    if (strcmp(cmd, "exit")==0)
      exit(0);

    pid = fork();

    if (pid)
    {
      printf("sh %d forked a child sh %d\n", getpid(), pid);
      printf("sh %d wait for child sh %d to terminate\n", getpid(), pid);
      pid = wait(&status);
      printf("ZOMBIE child=%d exitStatus=%x\n", pid, status);
      printf("main sh %d repeat loop\n", getpid());
    }
    else
    {
      printf("child sh %d running\n", getpid());

      // make a cmd line = dir[0]/cmd for execve()
      pipe_line(line);
    }
  }
}

/********************* YOU DO ***********************
1. I/O redirections:

Example: line = arg0 arg1 ... > argn-1

  check each arg[i]:
  if arg[i] = ">" {
     arg[i] = 0; // null terminated arg[ ] array
     // do output redirection to arg[i+1] as in Page 131 of BOOK
  }
  Then execve() to change image


2. Pipes:

Single pipe   : cmd1 | cmd2 :  Chapter 3.10.3, 3.11.2

Multiple pipes: Chapter 3.11.2
****************************************************/

void IOredirect()
{
  for (int j = 0; j < n; ++j)
  {
    if(strcmp(name[j], "<") == 0)
    {
      close(0);
      int fd=open(name[j + 1], O_RDONLY);
      //prevents command parameters after
      name[j] = 0;
    }
    else if(strcmp(name[j], ">") == 0)
    {
      close(1);
      int fd=open(name[j + 1], O_WRONLY|O_CREAT, 0644);
      name[j] = 0;
    }
    else if(strcmp(name[j], ">>") == 0)
    {
      close(1);
      int fd=open(name[j + 1], O_WRONLY|O_CREAT|O_APPEND, 0644);
      name[j] = 0;
    }
  }
}

void runCommand(char *line1)
{
  tokenize(line1, " ", gpath, name, &n);
  char *cmd = name[0];         // line = name0 name1 name2 ....
  char line[128];

  //redirects io if necessary
  IOredirect();

  //tries command in each path dir
  for (int i = 0; i < ndir; ++i)
  {
    strcpy(line, dir[i]);
    strcat(line, "/");
    strcat(line, cmd);
    //printf("i=%d  cmd= %s\n", i, line);

    int r = execve(line, name, env1);

    //printf("execve failed r = %d\n", r);
  }
  exit(1);
}

void splitHeadTail(char **head, char **tail, char *line)
{
  *head = line;
  *tail = 0;

  //split commandline into head and tail. If there is no pipe, tail is 0
  for (int i = 0; i < strlen(*head); ++i)
    if ((*head)[i] == '|')
    {
      (*head)[i] = 0;
      *tail = *head + i + 1;
      break;
    }
}

int pipe_line(char *line)
{
  char *head;
  char *tail;
  splitHeadTail(&head, &tail, line);

  //no piping
  if (!tail)
    runCommand(line);
  else
  {
    int pd[2];
    pipe(pd); // creates a PIPE
    int pid = fork(); // fork a child (to share the PIPE)
    if (pid)  // parent as pipe WRITER
    {
      close(pd[0]); // WRITER MUST close pd[0]
      close(1); // close 1
      dup(pd[1]); // replace 1 with pd[1]
      close(pd[1]); // close pd[1]
      runCommand(head); // change image to cmd1
    }
    else  // child as pipe READER
    {
      close(pd[1]); // READER MUST close pd[1]
      close(0);
      dup(pd[0]); // replace 0 with pd[0]
      close(pd[0]); // close pd[0]
      runCommand(tail); // change image to cmd2
    }
  }
}

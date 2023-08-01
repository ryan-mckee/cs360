/***** LAB4 client base code *****/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <libgen.h>     // for dirname()/basename()
#include <time.h>

#define MAX 1024
#define PORT 1234

char line[MAX], ans[MAX];
int n;

struct sockaddr_in saddr;
int sfd;

char gpath[128];    // hold token strings
char *name[64];     // token string pointers
int  num;             // number of token strings

int tokenize(char *l)
{
  // YOU better know how to apply it from now on
  char *s;
  strcpy(gpath, l);       // copy into global gpath[]
  s = strtok(gpath, " ");
  num = 0;
  while(s)
  {
    name[num++] = s;           // token string pointers
    s = strtok(0, " ");
  }
  name[num] = 0;                // name[n] = NULL pointer
}

int main(int argc, char *argv[], char *env[])
{
  int n;
  char how[64];
  int i;

  printf("1. create a socket\n");
  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd < 0)
  {
    printf("socket creation failed\n");
    exit(0);
  }

  printf("2. fill in server IP and port number\n");
  bzero(&saddr, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  saddr.sin_port = htons(PORT);

  printf("3. connect to server\n");
  if (connect(sfd, (struct sockaddr *)&saddr, sizeof(saddr)) != 0)
  {
    printf("connection with the server failed...\n");
    exit(0);
  }

  printf("********  processing loop  *********\n");
  while (1)
  {
    printf("input a line : ");
    bzero(line, MAX);                // zero out line[ ]
    fgets(line, MAX, stdin);         // get a line (end with \n) from stdin

    line[strlen(line)-1] = 0;        // kill \n at end
    if (line[0]==0)                  // exit if NULL line
      exit(0);




    tokenize(line);
    printf("cmd=%s\n", name[0]);

    if (strcmp(name[0], "put") == 0)
    {
      printf("put\n");
      if (num <= 1)
        printf("Enter a filename\n");
      else
        sendFile(sfd);
    }
    //local commands
    else if (strcmp(name[0], "lls") == 0)
    {
      printf("lls\n");
      struct dirent *ep;
      DIR *dp;
      if (num <= 1)
      {
        printf("contents of .\n");
        dp = opendir(".");
        while (ep = readdir(dp))
          printf("%s\t", ep->d_name);
        printf("\n");
      }
      else
        for (int i = 1; i < num; ++i)
        {
          printf("contents of %s\n", name[i]);
          dp = opendir(name[i]);
          while (ep = readdir(dp))
            printf("%s\t", ep->d_name);
          printf("\n");
        }
    }
    else if (strcmp(name[0], "lcd") == 0)
    {
      printf("lcd\n");
      if (num <= 1)
        chdir("/");
      else
        chdir(name[1]);
    }
    else if (strcmp(name[0], "lpwd") == 0)
    {
      printf("lpwd\n");
      char buf[256];
      getcwd(buf, 256);
      printf("cwd=%s\n", buf);
    }
    else if (strcmp(name[0], "lmkdir") == 0)
    {
      printf("lmkdir\n");
      if (num <= 1)
        printf("Enter a filename\n");
      for (int i = 1; i < num; ++i)
      {
        int r = mkdir(name[i], 0766);
        if (r)
          printf("mkdir %s failed\n", name[i]);
        else
          printf("mkdir %s succeeded\n", name[i]);
      }
    }
    else if (strcmp(name[0], "lrmdir") == 0)
    {
      printf("lrmdir\n");
      if (num <= 1)
        printf("Enter a filename\n");
      for (int i = 1; i < num; ++i)
      {
        int r = rmdir(name[i]);
        if (r)
          printf("rmdir %s failed\n", name[i]);
        else
          printf("rmdir %s succeeded\n", name[i]);
      }
    }
    else if (strcmp(name[0], "lrm") == 0)
    {
      printf("lrm\n");
      if (num <= 1)
        printf("Enter a filename\n");
      for (int i = 1; i < num; ++i)
      {
        int r = unlink(name[i]);
        if (r)
          printf("rm %s failed\n", name[i]);
        else
          printf("rm %s succeeded\n", name[i]);
      }
    }
    else if (strcmp(name[0], "lcat") == 0)
    {
      printf("lcat\n");
      if (num <= 1)
        printf("Enter a filename\n");
      for (int i = 1; i < num; ++i)
      {
        FILE *f = fopen(name[i], "r");
        if (f)
        {
          char c = fgetc(f);
          while (c != EOF)
          {
            printf("%c", c);
            c = fgetc(f);
          }
          printf("\n");
          fclose(f);
        }
        else
          printf("cat %s failed\n", name[i]);
      }
    }
    else
    {
      // Send ENTIRE line to server
      n = write(sfd, line, MAX);
      printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

      // Read a line from sock and show it
      n = read(sfd, ans, MAX);
      //printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
      printf("client: read  n=%d bytes; \n%s\n", n, ans);
    }

    if (strcmp(name[0], "get") == 0)
    {
      if (ans[0] == -1)
      {
        printf("file doesn't exist\n");
      }
      else if (ans[0] == 1)
      {
        writeFile(sfd);
      }
    }
  }
}


/********************* YOU DO ***********************
    1. The assignment is the Project in 13.17.1 of Chapter 13

    2. Implement 2 sets of commands:

      ********************** menu ***********************
      * get  put  ls   cd   pwd   mkdir   rmdir   rm  *  // executed by server
      * lcat     lls  lcd  lpwd  lmkdir  lrmdir  lrm  *  // executed LOACLLY (this file)
      ***************************************************

    3. EXTRA Credits: make the server MULTI-threaded by processes

    Note: The client and server are in different folders on purpose.
          Get and put should work when cwd of client and cwd of server are different.
****************************************************/


void writeFile(int sfd)
{
  short ans_len = 0;
  FILE *f = fopen(name[1], "wb");
  //check if file exists
  if (f)
  {
    do
    {
      //read buffer
      n = read(sfd, ans, MAX);

      //read buffer length
      n = read(sfd, &ans_len, sizeof(ans_len));

      if (ans_len == -1)
        fwrite(ans, MAX, 1, f);
      else
        fwrite(ans, ans_len, 1, f);

    }
    while (ans_len == -1);

    fclose(f);
  }
}


void sendFile(int cfd)
{
  short ans_len = 0;
  FILE *f = fopen(name[1], "rb");
  //check if file exists
  if (!f)
    printf("File doesn't exist");
  else
  {
    //write line to server
    n = write(cfd, line, MAX);

    //fill buffer until EOF or max
    char c;
    fread(&c, sizeof(char), 1, f);
    while(!feof(f))
    {
      ans[ans_len++] = c;
      fread(&c, sizeof(char), 1, f);

      //if buffer full before EOF
      if (ans_len >= MAX)
      {
        //write buffer
        n = write(cfd, ans, MAX);

        //write buffer length
        ans_len = -1;
        n = write(cfd, &ans_len, sizeof(ans_len));

        //set buffer length to 0
        ans_len = 0;
      }
    }
    //if EOF, write buffer and ans_len
    n = write(cfd, ans, MAX);
    n = write(cfd, &ans_len, sizeof(ans_len));

    //printf("\n");
    //printf("%s", ans);
    fclose(f);
  }
}

/***** LAB4 server base code *****/

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
#include <libgen.h>
#include <time.h>

#define MAX 1024
#define PORT 1234

int n;

char ans[MAX];
char line[MAX];

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

int main()
{
  chroot(".");
  int sfd, cfd, len;
  struct sockaddr_in saddr, caddr;
  int i, length;

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
  //saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  saddr.sin_port = htons(PORT);

  printf("3. bind socket to server\n");
  if ((bind(sfd, (struct sockaddr *)&saddr, sizeof(saddr))) != 0)
  {
    printf("socket bind failed\n");
    exit(0);
  }

  // Now server is ready to listen and verification
  if ((listen(sfd, 5)) != 0)
  {
    printf("Listen failed\n");
    exit(0);
  }
  while(1)
  {
    // Try to accept a client connection as descriptor newsock
    length = sizeof(caddr);
    cfd = accept(sfd, (struct sockaddr *)&caddr, &length);
    if (cfd < 0)
    {
      printf("server: accept error\n");
      exit(1);
    }

    printf("server: accepted a client connection from\n");
    printf("-----------------------------------------------\n");
    printf("    IP=%s  port=%d\n", "127.0.0.1", ntohs(caddr.sin_port));
    printf("-----------------------------------------------\n");

    // Processing loop
    while(1)
    {
      printf("server ready for next request ....\n");
      n = read(cfd, line, MAX);
      if (n==0)
      {
        printf("server: client died, server loops\n");
        close(cfd);
        break;
      }

      // show the line string
      printf("server: read  n=%d bytes; line=[%s]\n", n, line);

      bzero(ans, MAX);                // zero out line[ ]




      tokenize(line);
      printf("cmd=%s\n", name[0]);


      if (strcmp(name[0], "get") == 0)
      {
        printf("get\n");
        sendFile(cfd);
      }
      else if (strcmp(name[0], "put") == 0)
      {
        printf("put\n");
        writeFile(cfd);
      }
      else
      {
        if (strcmp(name[0], "ls") == 0)
        {
          printf("ls\n");
          struct dirent *ep;
          DIR *dp;
          if (num <= 1)
          {
            printf("contents of .\n");
            strcat(ans, "contents of .\n");
            dp = opendir(".");
            while (ep = readdir(dp))
            {
              printf("%s\t", ep->d_name);
              strcat(ans, ep->d_name);
              strcat(ans, "\t");
            }
            printf("\n");
            strcat(ans, "\n");
          }
          else
            for (int i = 1; i < num; ++i)
            {
              printf("contents of %s\n", name[i]);
              strcat(ans, "contents of ");
              strcat(ans, name[i]);
              strcat(ans, "\n");
              dp = opendir(name[i]);
              while (ep = readdir(dp))
              {
                printf("%s\t", ep->d_name);
                strcat(ans, ep->d_name);
                strcat(ans, "\t");
              }
              printf("\n");
              strcat(ans, "\n");
            }
        }
        else if (strcmp(name[0], "cd") == 0)
        {
          printf("cd\n");
          if (num <= 1)
            chdir("/");
          else
            chdir(name[1]);
        }
        else if (strcmp(name[0], "pwd") == 0)
        {
          printf("pwd\n");
          char buf[256];
          getcwd(buf, 256);
          printf("cwd=%s\n", buf);
          strcat(ans, "cwd=");
          strcat(ans, buf);
          strcat(ans, "\n");
        }
        else if (strcmp(name[0], "mkdir") == 0)
        {
          printf("mkdir\n");
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
        else if (strcmp(name[0], "rmdir") == 0)
        {
          printf("rmdir\n");
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
        else if (strcmp(name[0], "rm") == 0)
        {
          printf("rm\n");
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



        //strcat(ans, " ECHO");

        // send information back to client
        n = write(cfd, ans, MAX);

        printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
      }
      printf("server: ready for next request\n");
    }
  }
}

/********************* YOU DO ***********************
    1. The assignment is the Project in 13.17.1 of Chapter 13

    2. Implement 2 sets of commands:

      ********************** menu ***********************
      * get  put  ls   cd   pwd   mkdir   rmdir   rm  *  // executed by server (this file)
      * lcat     lls  lcd  lpwd  lmkdir  lrmdir  lrm  *  // executed LOACLLY
      ***************************************************

    3. EXTRA Credits: make the server MULTI-threaded by processes

    Note: The client and server are in different folders on purpose.
          Get and put should work when cwd of client and cwd of server are different.
****************************************************/

void sendFile(int cfd)
{
  short ans_len = 0;
  //check if filename is provided
  if (num <= 1)
  {
    printf("Enter a filename\n");
    ans[0] = -1;
    n = write(cfd, ans, MAX);
  }
  else
  {
    FILE *f = fopen(name[1], "rb");
    //check if file exists
    if (!f)
    {
      ans[0] = -1;
      n = write(cfd, ans, MAX);
    }
    else
    {
      //confirm file exists
      ans[0] = 1;
      n = write(cfd, ans, MAX);

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
}

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

    } while (ans_len == -1);

    fclose(f);
  }
}


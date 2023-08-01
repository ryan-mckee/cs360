#include <stdio.h>              // for I/O
#include <stdlib.h>             // for exit()
#include <libgen.h>             // for dirname()/basename()
#include <string.h>             // for string functions

typedef struct node
{
  char  name[64];         // node's name string
  char  type;             // type = 'D' or 'F'
  struct node *child, *sibling, *parent;
} NODE;


NODE *root, *cwd, *start;       // root->/, cwd->CWD, start->start_node
char line[128];                 // user input line
char command[16], pathname[64]; // command pathname strings

// you need these for dividing pathname into token strings
char gpath[128];                // token string holder
char *name[32];                 // token string pointers
int  n;                         // number of token strings

//               0         1      2     3      4       5      6      7       8
char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat", "rm", "quit", "menu", 0};

int findCmd(char *command)
{
  int i = 0;
  while(cmd[i])
  {
    if (strcmp(command, cmd[i])==0)
      return i;
    i++;
  }
  return -1;
}

NODE *search_child(NODE *parent, char *name)
{
  NODE *p;
  printf("search for %s in parent DIR %s\n", name, parent->name);
  p = parent->child;
  if (p==0)
    return 0;
  while(p)
  {
    if (strcmp(p->name, name)==0)
      return p;
    p = p->sibling;
  }
  return 0;
}

int insert_child(NODE *parent, NODE *q)
{
  NODE *p;
  printf("insert NODE %s into parent's child list\n", q->name);
  p = parent->child;
  if (p==0)
    parent->child = q;
  else
  {
    while(p->sibling)
      p = p->sibling;

    p->sibling = q;
  }
  q->parent = parent;
  q->child = 0;
  q->sibling = 0;
}

int tokenize(char *pathname)
{
  char *s;
  s = strtok(pathname, "/");
  for (n = 0; s; ++n)
  {
    //printf("%s ", s);
    name[n] = s;
    s = strtok(0, "/");
  }
  //printf("\n");

  //for (int i = 0; i < n; ++i)
    //printf("%s ", name[i]);
  //printf("\n");
}

NODE *path2node(char *pathname)
{
  //1. if pathname is absolute:
  if (pathname[0]=='/')
    start = root;
  else
    start = cwd;

  tokenize(pathname); // gpath[] =[thisAB/thatCD/hereEF0]
                                //[thisAB0thatCD0hereEF0]
                                //   |      |      |
                                //name[0] name[1] name[2]
                               //|<------ n = 3 -------->|
  NODE *p = start;
  for (int i=0; i < n; i++)
  {
    if (strcmp(name[i], ".")==0)
    {
      printf("same directory\n");
      //same directory
    }
    else if (strcmp(name[i], "..")==0)
    {
      printf("parent directory\n");
      p = p->parent;
    }
    else
    {
      p = search_child(p, name[i]);
      if (p==0)
      {
        //print error message;
        printf("not found\n");
        return 0;
      }
    }
  }
  return p;
}

/***************************************************************
 This mkdir(char *name) makes a DIR in the current directory
 You MUST improve it to mkdir(char *pathname) for ANY pathname
****************************************************************/

/*int mkdir(char *name)
{
  NODE *p, *q;
  printf("mkdir: name=%s\n", name);

  if (!strcmp(name, "/") || !strcmp(name, ".") || !strcmp(name, ".."))
  {
    printf("can't mkdir with %s\n", name);
    return -1;
  }
  if (name[0]=='/')
    start = root;
  else
    start = cwd;

  printf("check whether %s already exists\n", name);
  p = search_child(start, name);
  if (p)
  {
    printf("name %s already exists, mkdir FAILED\n", name);
    return -1;
  }
  printf("--------------------------------------\n");
  printf("ready to mkdir %s\n", name);
  q = (NODE *)malloc(sizeof(NODE));
  q->type = 'D';
  strcpy(q->name, name);
  insert_child(start, q);
  printf("mkdir %s OK\n", name);
  printf("--------------------------------------\n");

  return 0;
}*/

int mkdir(char *pathname)
{
  //1. if pathname is absolute:
  if (pathname[0]=='/')
    start = root;
  else
    start = cwd;

  tokenize(pathname); // gpath[] =[thisAB/thatCD/hereEF0]
                                //[thisAB0thatCD0hereEF0]
                                //   |      |      |
                                //name[0] name[1] name[2]
                               //|<------ n = 3 -------->|
  NODE *p = start;
  for (int i=0; i < n-1; i++)
  {
    if (strcmp(name[i], ".")==0)
    {
      printf("same directory\n");
      //same directory
    }
    else if (strcmp(name[i], "..")==0)
    {
      printf("parent directory\n");
      p = p->parent;
    }
    else
    {
      p = search_child(p, name[i]);
      if (p==0)
      {
        //print error message;
        printf("invalid path\n");
        return 0;
      }
    }
  }
  mkdir1(name[n-1], p);
}


int mkdir1(char *name, NODE *dir)
{
  NODE *p, *q;
  printf("mkdir: name=%s\n", name);

  if (!strcmp(name, "/") || !strcmp(name, ".") || !strcmp(name, ".."))
  {
    printf("can't mkdir with %s\n", name);
    return -1;
  }
  /*if (name[0]=='/')
    start = root;
  else
    start = cwd;*/
  start = dir;

  printf("check whether %s already exists\n", name);
  p = search_child(start, name);
  if (p)
  {
    printf("name %s already exists, mkdir FAILED\n", name);
    return -1;
  }
  printf("--------------------------------------\n");
  printf("ready to mkdir %s\n", name);
  q = (NODE *)malloc(sizeof(NODE));
  q->type = 'D';
  strcpy(q->name, name);
  insert_child(start, q);
  printf("mkdir %s OK\n", name);
  printf("--------------------------------------\n");

  return 0;
}

// This ls() list CWD. You MUST improve it to ls(char *pathname)
int ls(char *pathname)
{
  //NODE *p = cwd;
  NODE *p = path2node(pathname);
  if (!p)
    return 0;
  p = p->child;

  printf("cwd contents = ");
  while(p)
  {
    printf("[%c %s] ", p->type, p->name);
    p = p->sibling;
  }
  printf("\n");
}

int quit()
{
  printf("Program exit\n");
  exit(0);
  // improve quit() to SAVE the current tree as a Linux file
  // for reload the file to reconstruct the original tree
}

int initialize()
{
  root = (NODE *)malloc(sizeof(NODE));
  strcpy(root->name, "/");
  root->parent = root;
  root->sibling = 0;
  root->child = 0;
  root->type = 'D';
  cwd = root;
  printf("Root initialized OK\n");
}

int rmdir(char *pathname)
{
  NODE *p = path2node(pathname);
  if (!p)
  {
    printf("Directory doesn't exist\n");
    return 0;
  }
  if (p->type != 'D')
  {
    printf("File is not a directory\n");
    return 0;
  }
  if (p->child != 0)
  {
    printf("Directory is not empty\n");
    return 0;
  }
  NODE *parent = p->parent;
  if (parent->child == p)
  {
    parent->child = p->sibling;
  }
  else
  {
    NODE *index = parent->child;
    while (index->sibling != p)
    {
      index = index->sibling;
    }
    index->sibling = p->sibling;
  }
  free(p);
}

int cd(char *pathname)
{
  NODE *p = path2node(pathname);
  if (p)
    cwd = p;
}

int printparents(NODE *p)
{
  if (p == root)
    return 0;
  printparents(p->parent);
  printf("/%s", p->name);
}

int pwd()
{
  if (cwd == root)
  {
    printf("/\n");
    return 0;
  }
  printparents(cwd);
  printf("\n");
}

int creat(char *pathname)
{
  //1. if pathname is absolute:
  if (pathname[0]=='/')
    start = root;
  else
    start = cwd;

  tokenize(pathname); // gpath[] =[thisAB/thatCD/hereEF0]
                                //[thisAB0thatCD0hereEF0]
                                //   |      |      |
                                //name[0] name[1] name[2]
                               //|<------ n = 3 -------->|
  NODE *p = start;
  for (int i=0; i < n-1; i++)
  {
    if (strcmp(name[i], ".")==0)
    {
      printf("same directory\n");
      //same directory
    }
    else if (strcmp(name[i], "..")==0)
    {
      printf("parent directory\n");
      p = p->parent;
    }
    else
    {
      p = search_child(p, name[i]);
      if (p==0)
      {
        //print error message;
        printf("invalid path\n");
        return 0;
      }
    }
  }
  creat1(name[n-1], p);
}

int creat1(char *name, NODE *dir)
{
  NODE *p, *q;
  printf("creat: name=%s\n", name);

  if (!strcmp(name, "/") || !strcmp(name, ".") || !strcmp(name, ".."))
  {
    printf("can't creat with %s\n", name);
    return -1;
  }
  /*if (name[0]=='/')
    start = root;
  else
    start = cwd;*/
  start = dir;

  printf("check whether %s already exists\n", name);
  p = search_child(start, name);
  if (p)
  {
    printf("name %s already exists, creat FAILED\n", name);
    return -1;
  }
  printf("--------------------------------------\n");
  printf("ready to creat %s\n", name);
  q = (NODE *)malloc(sizeof(NODE));
  q->type = 'F';
  strcpy(q->name, name);
  insert_child(start, q);
  printf("mkdir %s OK\n", name);
  printf("--------------------------------------\n");

  return 0;
}


int rm(char *pathname)
{
  NODE *p = path2node(pathname);
  if (!p)
  {
    printf("File doesn't exist\n");
    return 0;
  }
  if (p->type != 'F')
  {
    printf("File is not a regular file\n");
    return 0;
  }
  NODE *parent = p->parent;
  if (parent->child == p)
  {
    parent->child = p->sibling;
  }
  else
  {
    NODE *index = parent->child;
    while (index->sibling != p)
    {
      index = index->sibling;
    }
    index->sibling = p->sibling;
  }
  free(p);
}

int menu()
{
  printf("\tMENU\n");
  for (int i = 0; cmd[i] != 0; ++i)
    printf("%s ", cmd[i]);
  printf("\n");
}

int main()
{
  int index;

  initialize();

  while(1)
  {
    //printf("Enter command line [mkdir|ls|quit] : ");
    fgets(line, 128, stdin);     // get input line
    line[strlen(line)-1] = 0;    // kill \n at end

    pathname[0] = 0;
    sscanf(line, "%s %s", command, pathname); // extract command pathname
    printf("command=%s pathname=%s\n", command, pathname);

    if (command[0]==0)
      continue;

    index = findCmd(command);

    switch (index)
    {
    case 0:
      mkdir(pathname);
      break;
    case 1:
      rmdir(pathname);
      break;
    case 2:
      ls(pathname);
      break;
    case 3:
      cd(pathname);
      break;
    case 4:
      pwd();
      break;
    case 5:
      creat(pathname);
      break;
    case 6:
      rm(pathname);
      break;
    case 7:
      quit();
      break;
    case 8:
      menu();
      break;
    }
  }
}


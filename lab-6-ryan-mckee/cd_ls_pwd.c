/************* cd_ls_pwd.c file **************/
int cd()
{
  //printf("cd: under construction READ textbook!!!!\n");

  // READ Chapter 11.7.3 HOW TO chdir
  if (strcmp(pathname, "") == 0) 
  {
    pathname[0] = '/';
    pathname[1] = 0;
  }
  int ino = getino(pathname); //get ino of path
  if (ino == -1) //if ino doesnt exist
  {
    printf("error: inode doesn't exist\n");
    return -1;
  }
  MINODE *mip1 = iget(dev, ino);
  if (S_ISDIR(mip1->INODE.i_mode))
  {
    iput(running->cwd);
    running->cwd = mip1;
    return 0;
  }
  else
    printf("error: not a directory\n");
}

int ls_file(MINODE *mip, char *name)
{
  //printf("ls_file: to be done: READ textbook!!!!\n");
  // READ Chapter 11.7.3 HOW TO ls

  char *t1 = "xwrxwrxwr-------";
  char *t2 = "----------------";

  char ftime[64];
  /*struct stat fstat, *sp;
  int r, i;
  char ftime[64];
  sp = &fstat;
  if ((r = lstat(name, &fstat)) < 0)
  {
    printf("can't stat %s\n", name);
    return 0;
    //exit(1);
  }
  if (S_ISREG(sp->st_mode))
    printf("%c",'-');
  if (S_ISDIR(sp->st_mode))
    printf("%c",'d');
  if (S_ISLNK(sp->st_mode))
    printf("%c",'l');
  for (i=8; i >= 0; i--)
  {
    if (sp->st_mode & (1 << i)) // print r|w|x
      printf("%c", t1[i]);
    else
      printf("%c", t2[i]); // or print -
  }*/
  int mode = mip->INODE.i_mode;
  if (S_ISREG(mode))
    printf("-");
  if (S_ISDIR(mode))
    printf("d");
  if (S_ISLNK(mode))
    printf("l");
  for (int i=8; i >= 0; i--)
  {
    if (mode & (1 << i)) // print r|w|x
      printf("%c", t1[i]);
    else
      printf("%c", t2[i]); // or print -
  }
  printf("%4d%4d%4d", mip->INODE.i_links_count, mip->INODE.i_gid, mip->INODE.i_uid);

  strcpy(ftime, ctime(&(mip->INODE.i_mtime))); // print time in calendar form
  ftime[strlen(ftime)-1] = 0; // kill \n at end
  printf("  %s",ftime + 4); //print without day
  //printf("  %s",ftime);
  printf("%8d", mip->INODE.i_size);

  printf("    %s\n", name);
}

int ls_dir(MINODE *mip)
{
  //printf("ls_dir: list CWD's file names; YOU FINISH IT as ls -l\n");

  char buf[BLKSIZE], temp[256];
  DIR *dp;
  char *cp;

  get_block(dev, mip->INODE.i_block[0], buf);
  printf("ls_dir i_block[0] = %d\n", mip->INODE.i_block[0]);
  dp = (DIR *)buf;
  cp = buf;

  while (cp < buf + BLKSIZE)
  {
    strncpy(temp, dp->name, dp->name_len);
    temp[dp->name_len] = 0;

    MINODE *mip1 = iget(dev, dp->inode);

    ls_file(mip1, temp);
    iput(mip1);

    cp += dp->rec_len;
    dp = (DIR *)cp;
  }
  //printf("\n");
}

int ls(char *pathname)
{
  //printf("ls: list CWD only! YOU FINISH IT for ls pathname\n");
  if (strlen(pathname) == 0) //ls current directory
    ls_dir(running->cwd);
  else
  {
    int ino = getino(pathname); //get ino of path
    if (ino == -1) //if ino doesnt exist
    {
      printf("error: inode doesn't exist\n");
      return -1;
    }
    dev = root->dev;
    MINODE *mip1 = iget(dev, ino);
    if (S_ISDIR(mip1->INODE.i_mode))
      ls_dir(mip1);
    else
      ls_file(mip1, name[n-1]);
    iput(mip1);
  }
}

char *pwd(MINODE *wd)
{
  //printf("pwd: READ HOW TO pwd in textbook!!!!\n");
  if (wd == root)
  {
    printf("/\n");
    return;
  }
  rpwd(wd);
  printf("/\n");
}

void rpwd(MINODE *wd)
{
  if (wd == root)
    return;
  int ino;
  char buf[BLKSIZE], dirname[64];
  get_block(dev, wd->INODE.i_block[0], buf);
  int parent_ino = findino(wd, &ino);

  MINODE *pip = iget(dev, parent_ino);
  findmyname(pip, ino, dirname);
  rpwd(pip);
  iput(pip);

  printf("/%s", dirname);
  return;
}


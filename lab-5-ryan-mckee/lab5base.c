/************** lab5base.c file ******************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

typedef unsigned int u32;

SUPER *sp;
GD    *gp;
INODE *ip;
DIR   *dp;

#define BLK 1024

int fd;             // opened vdisk for READ
int inodes_block;   // inodes start block number

char gpath[128];    // token strings
char *name[32];
int n;

int get_block(int fd, int blk, char *buf)
{
  lseek(fd, blk*BLK, 0);
  read(fd, buf, BLK);
}

int search(INODE *ip, char *name)
{
  // Chapter 11.4.4  int i;
  // Exercise 6
  char *cp; // char pointer
  int blk = ip->i_block[0];
  char buf[BLK], temp[256];
  get_block(fd, blk, buf); // get data block into buf[ ]
  dp = (DIR *)buf; // as dir_entry
  cp = buf;
  int ino = 0;
  printf("i=0  i_block[0]=%d\n", blk);
  printf("   i_number rec_len name_len    name\n");
  //printf("%d %d %d %s\n", dp->inode, dp->rec_len, dp->name_len, temp);
  while(cp < buf + BLK)
  {
    strncpy(temp, dp->name, dp->name_len); // make name a string
    temp[dp->name_len] = 0; // ensure NULL at end
    printf("%8d %8d %8d \t%s\n", dp->inode, dp->rec_len, dp->name_len, temp);
    if (strcmp(temp, name) == 0)
      return dp->inode;
    //ino = dp->inode;
    cp += dp->rec_len; // advance cp by rec_len
    dp = (struct ext2_dir_entry_2 *)cp; // pull dp to next entry
  }
  return 0;
}

/*************************************************************************/
int tokenize(char *pathname)
{
  // you did this many times
  char *s;
  s = strtok(pathname, "/");
  for (n = 0; s; ++n)
  {
    //printf("%s ", s);
    name[n] = s;
    s = strtok(0, "/");
  }
}

char *disk = "vdisk";

int main(int argc, char *argv[])   // a.out pathname
{
  char buf[BLK];  // use more bufs if you need them
  sp = (SUPER *)buf;
  gp = (SUPER *)buf;

  //1. open vdisk for READ: print fd value
  printf("1. open vdisk for READ: ");
  fd = open(disk, O_RDONLY);
  printf("fd=%d\n", fd);

  //2. read SUPER block #1 to verify EXT2 fs : print s_magic in HEX
  printf("2. read SUPER block #1 to verify EXT2 fs: ");
  get_block(fd, 1, buf);
  printf("s_magic=0x%x ", sp->s_magic);
  if (sp->s_magic == 0xef53)
    printf("OK\n");
  else
    printf("not EXT2 fs\n");

  //3. read GD block #2 to get inodes_block=bg_inode_table: print inodes_block
  printf("3. read GD block #2 to get inodes_block number: ");
  get_block(fd, 2, buf);
  inodes_block = gp->bg_inode_table;
  printf("inodes_block=%d\n", inodes_block);

  //4. read inodes_block into buf[] to get root INODE #2 : set ino=2
  //INODE *ip = (INODE *)buf + 1;
  printf("4. read inodes_block=%d into buf[] to get root INODE #2 : set ino=2\n", inodes_block);
  get_block(fd, inodes_block, buf);

  int ino = 2;
  ip = (INODE *)buf + 1;

  //5. tokenize pathame=argv[1]);
  printf("5. tokenize pathname=%s   ", argv[1]);
  if (argc <= 1)
  {
    printf("enter a pathname\n");
    return -1;
  }

  tokenize(argv[1]);
  for (int i = 0; i < n; ++i)
    printf("%s ", name[i]);
  printf("\n");

  //6.
  for (int i = 0; i < n; i++)
  {
    printf("===========================================\n");
    printf("search name[%d]=%s in ino=%d\n", i, name[i], ino);
    ino = search(ip, name[i]);

    if (ino==0)
    {
      printf("name %s does not exist\n", name[i]);
      exit(1);
    }
    else
      printf("found %s   ino=%d\n", name[i], ino);
    // MAILman's algrithm:
    int blk    = (ino-1) / 8 + inodes_block;
    int offset = (ino-1) % 8;
    printf("blk=%d   offset=%d\n\n", blk, offset);

    get_block(fd, blk, buf);

    ip = (INODE *)buf + offset;
  }


  //7. HERE, ip->INODE of pathname
  printf("****************  DISK BLOCKS  *******************\n");
  for (int i=0; i<15; i++)
  {
    if (ip->i_block[i])
      printf("block[%2d] = %d\n", i, ip->i_block[i]);
  }

  printf("================ Direct Blocks ===================\n");
  for (int i = 0; i <= 11; ++i)
    if (ip->i_block[i])
      printf("%d ", ip->i_block[i]);
  printf("\n");

  char buf2[BLK];
  char buf3[BLK];
  u32 *up = (u32 *)buf2;

  if (ip->i_block[12])
  {
    printf("===============  Indirect blocks   ===============\n");
    get_block(fd, ip->i_block[12], buf2);
    for (int i = 0; i < 256; ++i, ++up)
      if (*up)
        printf("%d ", *up);
    printf("\n");
  }

  up = (u32 *)buf2;

  if (ip->i_block[13])
  {
    printf("===========  Double Indirect blocks   ============\n");
    get_block(fd, ip->i_block[13], buf2);
    for (int i = 0; i < 256; ++i, ++up)
      if (*up)
      {
        u32 *up2 = (u32 *)buf3;
        get_block(fd, *up, buf3);
        for (int j = 0; j < 256; ++j, ++up2)
          if (*up2)
            printf("%d ", *up2);
      }
    printf("\n");
  }
  close(fd);
}

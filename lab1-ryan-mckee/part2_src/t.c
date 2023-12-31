


// please put your main and code in this file. All contained.
// Use the provided mk script to compile your code. You are welcome to change the mk script if you'd like
// but know that is how we will be batch compiling code using mk on the back end so your code will need to be able
// to run when that bash script is ran.
// Most code can be grabbed from t.c in root of the github repo.
/* sample code for Part 2 */

#include <stdio.h>
#include <fcntl.h>

#include <sys/types.h>
#include <unistd.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

struct partition
{
  u8 drive;             // drive number FD=0, HD=0x80, etc.

  u8  head;             // starting head
  u8  sector;           // starting sector
  u8  cylinder;         // starting cylinder

  u8  sys_type;         // partition type: NTFS, LINUX, etc.

  u8  end_head;         // end head
  u8  end_sector;       // end sector
  u8  end_cylinder;     // end cylinder

  u32 start_sector;     // starting sector counting from 0
  u32 nr_sectors;       // number of of sectors in partition
};

char *dev = "vdisk";
int fd;

// read a disk sector into char buf[512]
int read_sector(int fd, int sector, char *buf)
{
  lseek(fd, sector*512, SEEK_SET);  // lssek to byte sector*512
  read(fd, buf, 512);               // read 512 bytes into buf[ ]
}

int main()
{
  struct partition *p;
  char buf[512];

  fd = open(dev, O_RDONLY);   // open dev for READ
  read(fd, buf, 512);         // read 512 bytes of fd into buf[ ]
  read_sector(fd, 0, buf);    // read in MBR at sector 0

  //Write C code to let p point at Partition 1 in buf[];

  //p = (struct partition *)(buf + 0x1BE);
  p = (struct partition *)(buf + 0x1BE);
  //p = buf;

  //print P1's start_sector, nr_sectors, sys_type;
  //printf("P1:%8u %8u %8u\n", p->start_sector, p->nr_sectors, p->sys_type);

  /*for (int i = 1; i <= 4; ++i)
  {
    printf("P%d:%8u\t%8u\t%8u\n", i, p->start_sector, p->end_sector, p->nr_sectors);
    ++p;
  }*/
  /*start_sector end_sector nr_sectors
  P1 :       18      359      342
  P2 :      360      719      360
  P3 :      720     1439      720
  P4 :     1440     2879     1440*/

  //Write code to print all 4 partitions;

  printf("    start_sector  end_sector  nr_sectors\n");
  for (int i = 0; i < 4; ++i)
    printf("P%d:%8u\t%8u\t%8u\n", i + 1, p[i].start_sector, p[i].start_sector + p[i].nr_sectors - 1, p[i].nr_sectors);
  printf("\n");

  // ASSUME P4 is EXTEND type:
  //Let  int extStart = P4's start_sector;
  //print extStart to see it;

  int extStart = p[3].start_sector;
  int localMBR = extStart;

  read_sector(fd, localMBR, buf);
  printf("Ext Partition 4 : start_sector=%d\n", extStart);

  p = (struct partition *)(buf + 0x1BE);

  for (int j = 5; 1; ++j)
  {
    printf("next localMBR sector=%d\n\n", localMBR);

    printf("Entry 1: start_sector=%d  nr_sectors=%d\n", p[0].start_sector, p[0].nr_sectors);
    printf("Entry 2: start_sector=%d  nr_sectors=%d\n", p[1].start_sector, p[1].nr_sectors);
    //printf("P%d:%8u\t%8u\t%8u\n", j, localMBR + p[0].start_sector, p[0].nr_sectors);
    printf("P%d:%8u\t%8u\t%8u\n", j, localMBR + p[0].start_sector, localMBR + p[0].start_sector + p[0].nr_sectors - 1, p[0].nr_sectors);
    if (p[1].start_sector == 0)
      break;

    localMBR += p[0].nr_sectors + p[0].start_sector;
    read_sector(fd, localMBR, buf);
  }

//loop:
  // partition table of localMBR in buf[ ] has 2 entries:
  //print entry 1's start_sector, nr_sector;
  //compute and print P5's begin, end, nr_sectors

  //if (entry 2's start_sector != 0){
  //if (start_sector != 0)
  //{
    //compute and print next localMBR sector;
    //continue loop;
  //}
}

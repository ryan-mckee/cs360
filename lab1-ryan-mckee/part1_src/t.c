//Ryan McKee
//Lab 1 Part 1 code 1/20/22

//included stdio library
#include <stdio.h>

// please put your main and code in this file. All contained.
// Use the provided mk script to compile your code. You are welcome to change the mk script if you'd like
// but know that is how we will be batch compiling code using mk on the back end so your code will need to be able
// to run when that bash script is ran.
// Most code can be grabbed from the .html file in the root.

typedef unsigned int u32;

char *ctable = "0123456789ABCDEF";
int  BASE = 10;

int rpu(u32 x)
{
  char c;
  if (x)
  {
    c = ctable[x % BASE];
    rpu(x / BASE);
    putchar(c);
  }
}

int printu(u32 x)
{
  (x==0)? putchar('0') : rpu(x);
  //putchar(' ');
}

//prints a string
int prints(char *s)
{
  while (*s != '\0')
  {
    putchar(*s);
    ++s;
  }
}

//recursive function for printing int in hex
int rpx(u32 x)
{
  char v = x % 16;
  if (x >> 4 != 0)
    rpx(x >> 4);
  v += (v > 9) ? 'a' - 10 : '0';
  putchar(v);
}

//prints an unsigned 32 bit integer in hex
int printx(u32 x)
{
  putchar('0');
  putchar('x');
  rpx(x);
}

//prints a signed 32 bit integer in decimal
int printd(int x)
{
  if (x > 0)
    printu(x);
  else
  {
    putchar('-');
    printu(-x);
  }
}

int myprintf(char *fmt, ...)
{
  int *ip = (int *)(&fmt) + 1;
  for (char *cp = fmt; *cp != '\0'; ++cp)
  {
    if (*cp != '%')
    {
      putchar(*cp);
      if (*cp == '\n')
        putchar('\r');
    }
    else
    {
      ++cp;
      switch (*cp)
      {
      case 'c':
        putchar(*ip);
        break;
      case 's':
        prints((char *)*ip);
        break;
      case 'u':
        printu(*ip);
        break;
      case 'x':
        printx(*ip);
        break;
      case 'd':
        printd(*ip);
        break;
      }
      ++ip;
    }
  }
}

int main(int argc, char *argv[ ], char *env[ ])
{
  myprintf("char=%c string=%s       dec=%u  hex=%x neg=%d\n",
           'A', "this is a test",  100,   100,   -100);
  //printd(-112);
  //putchar('\n');
  //myprintf("testing %u %s %d%c 64 in hex is %x\n", 123, "TESTING", -456, '!', 64);
  //myprintf("testing %u %d %c \n", 123, -456, '!');
  //printu(10);
  //prints("hello\n");
  /*for (int i = 0; i < 34; ++i)
  {
    printx(i);
    putchar('\n');
  }*/
}

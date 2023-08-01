/************* t.c file ********************/
#include <stdio.h>
#include <stdlib.h>

int *FP;

int main(int argc, char *argv[ ], char *env[ ])
{
  int a,b,c;
  printf("enter main\n");

  printf("&argc=%x argv=%x env=%x\n", &argc, argv, env);
  printf("&a=%8x &b=%8x &c=%8x\n", &a, &b, &c);

  //(1). Write C code to print values of argc and argv[] entries
  printf("argc=%u\n", argc);

  for (int i = 0; i < argc; ++i)
    printf("arg%u=%s ", i, argv[i]);
  printf("\n");

  a=1;
  b=2;
  c=3;
  A(a,b);
  printf("exit main\n");
}

int A(int x, int y)
{
  int d,e,f;
  printf("enter A\n");
  // write C code to PRINT ADDRESS OF d, e, f
  d=4;
  e=5;
  f=6;
  printf("&d=%8x &e=%8x &f=%8x\n", &d, &e, &f);
  B(d,e);
  printf("exit A\n");
}

int B(int x, int y)
{
  int g,h,i;
  printf("enter B\n");
  // write C code to PRINT ADDRESS OF g,h,i
  g=7;
  h=8;
  i=9;
  printf("&g=%8x &h=%8x &i=%8x\n", &g, &h, &i);
  C(g,h);
  printf("exit B\n");
}

int C(int x, int y)
{
  int u, v, w, i, *p;

  printf("enter C\n");
  // write C cdoe to PRINT ADDRESS OF u,v,w,i,p;
  u=10;
  v=11;
  w=12;
  i=13;
  printf("&u=%8x &v=%8x &w=%8x &i=%8x\n", &u, &v, &w, &i);

  FP = (int *)getebp();  // FP = stack frame pointer of the C() function

  //Write C code to print FP value in HEX
  printf("FP=%8x\n", FP);

  //(2). Write C code to print the stack frame link list as
    
  //FP -> previousFP -> previousFP -> .... -> NULL      (all in Hex)

  for (; FP != NULL; FP = *FP)
    printf("FP=%8x -> ", FP);
  printf("NULL\n");

  //(3). Let p = &u;
  //Print the stack contents from p to the frame of main()
  //YOU MAY JUST PRINT 128 entries of the stack contents.

  p = &u;
  for (int j = 0; j < 128; ++j)
  {
    //uncomment if desired output is in decimal
    //printf("#%d=%d\n", j, *p);
    //makes it easier to identify pointers if in hex
    printf("#%d=%8x\n", j, *p);
    ++p;
  }

  //(4). On a hard copy of the print out, identify the stack contents
  //as LOCAL VARIABLES, PARAMETERS, stack frame pointer of each function.
}

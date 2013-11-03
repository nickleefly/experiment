#include<stdio.h>
int main()
{
  int x=222,y=111,z;
  //z=x++y; // This Statement will give you an Error.
  z=x+ +y;
  printf("\nZ is %d\n",z);
  //z=x--y; // This Statement will give you an Error.
  z=x- -y;
  printf("\nZ is %d\n",z);
  //z=x+++++y; // This Statement will give you an Error.
  z=x++ + ++y;
  printf("\nZ is %d\n",z);
  return 0;
}
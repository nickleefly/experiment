#include <iostream>
using namespace std;
/*
double sum(double data[], int size)
{
  double s = 0.0; int il
  for(i = 0; i < size; i++)
    s += data[i];
  return s;
}
*/
template <class T> //T is generic type
T sum(const T data[], int size, T s = 0)
{
  for(int i = 0; i < size; ++i)
    //s += data[i]; //+= must work for T
    cout << "data[" << i << "] is " << data[i] << endl;
  return s;
}

int main()
{
  cout << "template for sum()" << endl;
  int a[] = {1, 2, 3};
  double b[] = {2.1, 2.2, 2.3};
  sum(a, 3);
  cout << "\n" << endl;
  sum(b, 3);
  //cout << sum(a, 3) << endl;
  //cout << sum(b, 3) << endl;
  return 0;
}

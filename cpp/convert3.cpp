#include <iostream>

using namespace std;

const int N=40;

template <class T>
//universal function sum : add the d table values
//inputs : reference to T p
//int n
//int d[]
inline void sum(T& p, T n, T d[])
{
  for(T i = 0; i < n; ++i)
    p += d[i];
}

int main()
{
  int accum = 0;  //or int accum(0);
  int data[N];

  //we fill the d table
  for(int i = 0; i < N; ++i) // i exists only in the for
    data[i] = i;

  sum(accum, N, data);
  cout << "sum is " << accum << endl;
  return 0;
}
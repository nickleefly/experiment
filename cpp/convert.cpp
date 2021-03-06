#include <iostream>
#include <vector>
using namespace std;
const int N = 40;

inline void sum(int& p, int n, const vector<int>& d)
{
  int i;
  p = 0;
  for(i = 0; i < n; ++i)
    p = p + d[i];
}

int main()
{
  int i;
  int accum = 0;
  vector<int> data(N);
  for(i = 0; i < N; ++i)
    data[i] = i;
  sum(accum, N, data);
  cout << "sum is " << accum << endl;
  cout << "sum hex is " << hex << accum << endl;
  cout << "sum address is " << &accum << endl;
}

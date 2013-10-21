#include <iostream>
using namespace std;

template <typename summable>
summable sum (summable data[], int size, summable s) {
  for (int i = 0; i < size; ++i)
    s+= data[i];
  return s;
}

template<typename summable>
summable sum (summable data[], int size) {
    int s = 0;
    for (int i = 0; i < size; ++i)
        s+= data[i];
    return s;
}

int main (int argc, char **argv){
  int array[] = {0,1,2,3,4};
  int size = 5;
  int start = 0;

  cout<<"Sum 3 parameter: "<<sum (array, size, start)<<endl;
  cout<<"Sum 2 parameter: "<<sum (array, size )<<endl;

  return 0;
}
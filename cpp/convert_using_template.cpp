//  Converted program to C++, which adds a list of numbers
//  Restricited the use of C++ features to that which are in Chapters 1-3
//  of C++ by Dissection or discussed in video lectures
//  Code style used is consistent with Bell Lab's as described in book
#include <iostream>
#include <vector>

// Do not have to use scope resolution operator with cout
using namespace std;

// define the immutable constant number_of_values
const int number_of_values = 40;

// Simple function to sum becomes tempalated inline function with pass
// by reference.
template <class summable>
inline summable sum(const vector<summable>& values) {
   summable total= 0;
   for(int i=0;  i<values.size(); ++i)
      total += values[i];
   return total;
}

int main(void)
{
  vector<int> data(number_of_values);   // Vectors instead of array

   for(int i=0; i<data.size(); ++i)   // Initialise vector values
      data[i] = i;
   cout << "sum is " <<  sum(data) << endl; // Output the sum
   return 0;
}
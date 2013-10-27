#include<iostream>
using namespace std;

class Test1
{
    static int testvar;
    int testvar1; //non static member
public:
    static void testfunc()
    {
        cout << endl << "Value of static variable : " << testvar;

        //Non static members can not be used in static functions.
        //cout << endl << "Value of nonstatic variable : " << testvar1;
    }
};

int Test1::testvar;

int main(int argc, char *argv[]) {
    Test1::testfunc();
    return 0;
}
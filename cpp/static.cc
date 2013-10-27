
#include<iostream>
using namespace std;

class Test {

public:
    static int objCounter;

    Test()
    {
        Test::objCounter++;
    }

    ~Test()
    {
        Test::objCounter--;
    }
};

int Test::objCounter;

int main(int argc, char * argv[]) {
    Test obj1, *ptr, obj2;
    cout << endl << "Number of Objects : " << Test::objCounter;
    //Will print -> Number of Objects : 2
    ptr= new Test;
    cout << endl << "Number of Objects : " << Test::objCounter;
    //Will print -> Number of Objects : 3
    delete ptr;
    cout << endl << "Number of Objects : " << Test::objCounter;
    //Will print -> Number of Objects : 2
    return 0;
}

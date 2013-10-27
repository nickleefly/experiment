#include<iostream>
using namespace std;

class Rational
{
    private:
        int num; // numerator
        int den; // denominator
    public:
        void show();
        Rational(int=1,int=1);
        void setnumden(int,int);
        Rational add(Rational object);
        Rational operator+(Rational object);
        bool operator==(Rational object);
        bool operator!=(Rational object);
};

void Rational::show() {
    cout << num << "/" << den << "\n";
}

Rational::Rational(int a,int b) {
    setnumden(a,b);
}

void Rational::setnumden(int x,int y) {
    int temp,a,b;
    a = x;
    b = y;
    if(b > a) {
        temp = b;
        b = a;
        a = temp;
    }
    while(a != 0 && b != 0) {
        if(a % b == 0)
            break;
        temp = a % b;
        a = b;
        b = temp;
    }
    num = x / b;
    den = y / b;
}

Rational Rational::add(Rational object) {
    int new_num = num * object.den + den * object.num;
    int new_den = den * object.den;
    return Rational(new_num, new_den);
}

Rational Rational::operator+(Rational object) {
    int new_num = num * object.den + den * object.num;
    int new_den = den * object.den;
    return Rational(new_num, new_den);
}

bool Rational::operator==(Rational object) {
    return (num == object.num && den == object.den);
}

bool Rational::operator!=(Rational object) {
    return (num != object.num || den != object.den);
}

int main() {
    Rational obj1(1,4), obj2(210,840), result1;
    result1 = obj1.add(obj2);
    result1.show();

    Rational obj3(1,3), obj4(33,99), result2;
    result2 = obj3 + obj4;
    result2.show();

    Rational obj5(10,14), obj6(25,35), obj7(2,3), obj8(1,2);

    if(obj5 == obj6) {
        cout << "The two fractions are equal." << endl;
    }
    if(obj7 != obj8) {
        cout << "The two fractions are not equal." << endl;
    }
    return 0;
}
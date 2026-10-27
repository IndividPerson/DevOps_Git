#include <iostream>
#include <string>

using namespace std;

int main() {

    int i = 10;
    cout << &i; //амперсант выводит сам адрес

    int* ptr = &i; //указатель это переменная которая хранит адрес другой переменной
    float f = 1.25;
    float* ptr2 = &f;



    


    return 0;
}
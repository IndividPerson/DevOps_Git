#include <iostream>
#include <string>

using namespace std;

int main() {

    int num = 10;
    int &a = num; //a теперь ссылка на num и мы сэкономили память

    cout << &num <<endl;
    cout << &a <<endl; 
    return 0;
}
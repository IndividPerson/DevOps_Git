#include <iostream>
#include <string>

using namespace std;

int main() {

    // ссылка
    int num = 10;
    int &a = num; //'a' теперь ссылка на num и мы сэкономили память

    a = 15;
    cout << &num <<endl;
    cout << &a <<endl; 
    
    cout << "---------------------\n";

    // указатели
    int val = 12;
    int *ptrval = &val;

    *ptrval = 20; // что бы обратиться к значению то нужно '*' ,без нее будет ссылка
    ptrval = nullptr; // очищает
    
    cout << &val << " - " << val << endl;
    cout << ptrval << " - " << *ptrval << endl;




    return 0;
}
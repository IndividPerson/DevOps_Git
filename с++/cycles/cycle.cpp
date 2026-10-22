// ctrl + f5 - start program
#include <iostream>

using namespace std;

int main() {
// cycle for --------------
    for (int i = 1; i <= 10; i++) {
        cout << "El: " << i << endl; 
    }

    cout << "cycle while" << endl << endl;
// cycle while ------------
    float a = 100;
    while(a >= 2) {
        a /= 2;
        cout << "el 2: " << a << endl;
    }

// cycle do while один раз 100 проц сработает а дальше по условию
    
    int k = 100;
    do{
        cout << "el 3: " << k << endl;
        k -= 10;
    } while (k < 10);

// операторы в циклах 1

    for(int i = 1; i < 15; i++) {
        if(i == 11)
            break;

        if(i % 2 == 0) // пропустит четные числа
            continue;

        cout << "el: " << i << endl;

    }


    return 0;
}
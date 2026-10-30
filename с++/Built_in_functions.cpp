#include <iostream>
#include <string>
#include <cstring> // строковые функцыи
#include <cmath> // математические функцыи

using namespace std;

int main() {
    
    string str1 = "hello";
    string str2 = "worldk";

    // cout << str1 + str2;
    str1.append(str2); // или так с помощью функций
    str1.pop_back(); // удаляет последний символ
    str1.push_back('!'); // добавляет
    cout << str1 << endl;
    cout << str1 << " - " << str1.length() <<endl; // выводит длину строки

    cout << pow(2, 3) << endl; // 2 возводит в 3 степень
    cout << abs(-3) << endl; // показывает число по модулю
    cout << sin(0.83) << endl; // sinus
    cout << sqrt(16) << endl; // квадратный корень
    cout << round(1.45) << endl; // округляет по нормальному
    cout << ceil(1.08) << endl; // округляет к большому
    cout << floor(1.78) << endl; // округляет к меньшему


    return 0;
}

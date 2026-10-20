#include <iostream>
int main() {
    int x = 5;
    std::cout << "Значение переменной x: " << x << std::endl;

    int a, b;
    std::cout << "введите 1 число:";
    std::cin >> a;
    std::cout << "введите 2 число:";
    std::cin >> b;

    

    // типы данных
    short num = 7; //2 byte   -32к до 32к (продуктивней)
    int num1 = 3; //4 byte   -2B до 2B
    long num2 = 5; //8 byte

    unsigned int num3 = 1; //0 до 4B без минусовых

    // числа с точкой
    float num4 = 3.14;
    std::cout << num4 << std::endl;

    // символ
    char sym = 'A'; // только 1 символ
    std::cout << sym <<std::endl;

    // логический тип
    bool isHappy = true; // 1 или 0, да или нет


    return 0;




}
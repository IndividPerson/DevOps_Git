#include <iostream>

using namespace std; // что бы не писать везде std::


int main() {

    srand(time(NULL)); // для рандома
// calculator ------------------------

    float num, num1, res;
    cout << "enter num 1: ";
    std::cin >> num;

    std::cout << "enter num 2: ";
    std::cin >> num1;

//    +, -, *, /
    
    char math; 
    std::cout << "enter math symbol: ";
    std::cin >> math;


    if(math == '+') {
        res = num + num1;
    }

    else if(math == '-') {
        res = num - num1;
    }
    
    else if(math == '*') {
        res = num * num1;
    }

    else if(math == '/') {
        res = num / num1;
    }

    else {
    std::cout << "Error 404" << std::endl;
    }

    std::cout << "result is: " << res <<std::endl;





// -------------------------  остаток при делении
    int a, b, c;

    a = 5;
    b = 3;
    c = a % b; // % дилення з остачей, показывает только остачу (2)
    std::cout << "остача: " << c <<std::endl;

// -------------------------  сокращенные операции
    a = a + 5;
    a /= 5; // текущее значение "a" делим на 5
    a += 5;
    a++; // a = a + 1; 
    a--;

// ------------------------- Random

int random = 1 + rand() % 20; // random number from 1 to 20
cout << "random number is: " << random <<endl;
// но оно каждый запуск будет одинаковое поэтому нужно написать на начале srand(time(NULL))

/*
Допустим, у вас 5 яблок. Вы не можете собрать пакет из 20 штук, поэтому все 5 штук остаются в руках. Остаток 5. (5 % 20 = 5)

1 + ... (сдвиг диапазона)
Нам нужно получить числа от 1 до 20, а у нас пока получается от 0 до 19.
*/

    return 0;
}
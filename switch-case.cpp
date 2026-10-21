#include <iostream>

int main() {

    int num;
    std::cin >> num;
    
    // switch case всегда проверяет на точное совпадение
    switch (num) {
    
    case 5:
        std::cout << "num is 5";
        break; // заканчивает конструкцию если условие верно

    case 50:
        std::cout << "num is 50";
        break;

    default: // если все не подошло, как else
        std::cout << "loh";
    }


    return 0;
}
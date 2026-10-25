#include <iostream>

int main() {
    int num;
    std::cout << "введите свой возвраст: " << std::endl;
    std::cin >> num;
    
    
    //  && - and
    //  || - or

    if(num >= 18 && num != 666) {
        std::cout << "вы совершеннолетний" << std::endl;
    }

    else if(num == 666) {
        std::cout << "you are demon sybau" << std::endl;
    }

    else {
            std::cout << "вы несовершеннолетний" << std::endl;
    }      


    return 0;
}
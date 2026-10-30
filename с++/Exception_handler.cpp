#include <iostream>
#include <string>

using namespace std;

void divide(float a, float b) {
    if(b == 0) throw 10;
    else cout << (a / b) <<endl;
}



int main() {
    
    try {
        divide(5.2, 0);
    } catch(int err) {
        if(err == 10) cout << "Ошибка деления на 0";
    }

    return 0;
}

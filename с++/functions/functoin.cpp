#include <iostream>
#include <string>

using namespace std;
// функции создаються за пределами main ведь это тоже функция

void print(string word) {
    cout << word <<endl;
}

int add(int a, int b) {
    return a + b;
}


int main() {

    string words = "World";

    print("hello"); // вызов функции с параметром
    print(words);


    /*
    если нужно возвратить какието данные из функции то так нельзя!
    int peremenna = add(7, 2);
    нужен код возврата
    */ 

    int res1 = add(7, 2);
    int res2 = add(50, 50);

    if (res2 > res1) {
        cout << res2 <<endl;
    }
  
    


    return 0;
}
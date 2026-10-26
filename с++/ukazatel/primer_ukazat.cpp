#include <iostream>
#include <string>

using namespace std;


void minimal(int *arr, int len) {
    int min = *arr; // указатель всегда показывает на первый элемент тоесть: 3
    for(int i = 0; i < len; i++) {
        if(min > *(arr + i)) { // смещаем на следующий элемент масива  
            min = *(arr + i);
        }
    }
    cout << "Minimal Num: " << min << endl; 

}



int main() {

    // Практический пример
    int arr[] = {3, 7, 3, -2, 5};
    minimal(arr, 5);

    


    return 0;
}
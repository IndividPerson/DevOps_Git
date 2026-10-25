#include <iostream>

using namespace std;

int main() {

    int nums[3]; //масив из 3 елементов
    nums[0] = 31;
    nums[1] = 32;
    nums[2] = 33;    
    cout << nums[1] <<endl;

    nums[1] = 40;
    cout << nums[1] <<endl;

    float nums2[3] = {1, 2, 3};

    for(int i = 0; i < 3; i++) { // вывести все элеманты масива
        cout << "el " << i << ":" << nums2[i] <<endl;
    }


    // Практическое примимнение
    // Прога для вычисления сумы и минимального числа из масива
    float nims[5];
    for(int i = 0; i < 5; i++) {
        cout << "Enter number " << i << ":" <<endl;
        cin >> nims[i]; 
    }


    float min = nims[0];
    float sum;
    
    for(int i = 0; i < 5; i++) {
        sum += nims[i];
        if(nims[i] < min) {
            min = nims[i];
        }
    }    

    cout << "ваши элементы:" <<endl;
    for(int i = 0; i < 5; i++) {
        cout << "El " << i << ":" << nims[i] <<endl;
    }

    cout << "сума: " << sum <<endl;
    cout << "минимальное число:" << min <<endl;


    // Многомерные массивы - это масивы которые содержат в себе масивы
    int matrix[3][2] = {
        {3, 5},
        {2, 8},
        {-2, 0}
    };
    
    matrix[1][0] = 228;
    cout << matrix[1][0] <<endl;
    
    for(int i = 0; i < 3; i++) {
        for(int n = 0; n < 2; n++) {
            cout << "данные из масива matrix: " << matrix[i][n] <<endl; 
        }
    }

    return 0;
}
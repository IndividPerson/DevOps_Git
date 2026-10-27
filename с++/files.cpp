#include <iostream>
#include <string>
#include <fstream> //work with files

using namespace std;

void output() {

ofstream file("test.txt", ios_base::out);
    
    //офстрим это тип данных, файл - назва переменной, тест файл - файл с которым работаем
    //ios_base::out - формат откритья файла
    if (file.is_open()) {
        file << "hello <3"; //вместо cout
        file.close(); // close file

        cout << "Файл успешно создан!";
    }
}

void input() {

    ifstream file2("test.txt"); // считывает с файлов
    if (file2.is_open()) {

        cout << "---------------------\n\n";
        cout << "Идет считка...\n";

        char temp[100]; // создание масива
        file2.getline(temp, 100); //считывание 100 символов и помещение в "temp"
        cout << temp << endl;
        file2.close();
    }
}


// ----------------------------------------------

int main() {

    cout << "Что вы хотите сделать? 1 - создать файл, 2 - считать файл\n";
    int ans;
    cin >> ans;

    if (ans == 1) {
        output();
    }

    else if(ans == 2) {
        input();
    }

    else {
        cout << "Введено не число!";
    }
    

    return 0;
}

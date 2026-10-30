#include <iostream>
#include <string>

using namespace std;

// Перечисление (enum) задает набор именованных констант (0, 1, 2...)
enum Options {
    open,  // 0
    close, // 1
    wait,  // 2
    del    // 3
};

// Структура объединяет переменные разных типов в один объект
struct File {
    float weight;
    string name;
    Options options;
};

int main() {
    File my_file;
    my_file.name = "test.txt";
    my_file.weight = 1.5;
    my_file.options = Options::close; // Присваиваем константу из enum
    
    // cout << my_file.options; // Выведет числовой индекс (1), а не текст

    if(my_file.options == Options::close)
        cout << "File is close" << endl;

    return 0;
}
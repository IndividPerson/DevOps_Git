#include <iostream>
#include <string>

using namespace std;

// Объявляем структуру (обычно за пределами main)
struct Point {
    string name;
    int x;
    int y;

    void get_info() {
    cout << "name: " << name << endl << "x: " << x << endl;
    }
};

struct tree {

    string name;
    Point place;

};





int main() {
    // Создаем переменную типа Point и задаем ей значения
    Point p1;
    p1.name = "lox";
    p1.x = 10;
    p1.y = 20;

    // Или можно инициализировать сразу при создании:
    Point p2 = {"dayn", 15, 30};

    // Структура в структуре
    tree yelka;
    yelka.name = "елка";
    yelka.place.x = 12;
    yelka.place.y = 18;


    // Выводим данные через оператор «точка» (.)
    cout << "Point 1: X = " << p1.x << ", Y = " << p1.y << endl;
    cout << "Point 2: X = " << p2.x << ", Y = " << p2.y << endl;

    p1.get_info();

    return 0;
}
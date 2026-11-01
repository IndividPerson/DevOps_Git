#include <iostream>
#include <string>

using namespace std;

// Прототип класса Car необходим, чтобы класс Person знал о его существовании 
// до того, как класс Car будет полностью описан в коде.
class Car; 

class Person {
private:
    int age;
    string name;
public:
    Person(int age, string name) {
        this->age = age;
        this->name = name;
    }

    // Объявление дружественной функции. Ключевое слово friend дает этой функции 
    // полный доступ к private-полям класса Person.
    friend void info(Car &car, Person &person);
};


class Car {
private:
    string name;
public:
    Car(string name) {
        this->name = name;
    }

    // Эта же функция объявляется другом и для класса Car, 
    // что позволяет ей читать private-поля и здесь.
    friend void info(Car &car, Person &person);
};


// Определение дружественной функции. 
// Она НЕ является методом ни одного из классов, но имеет доступ к private-полям обоих.
void info(Car &car, Person &person) {
    cout << "Человек с именем: " << person.name << " имеет машину " << car.name << endl;
}


int main() {
    Car bmw("BMW");
    Person max(17, "Max");
    
    // Вызов обычной функции, которой передаются объекты разных классов
    info(bmw, max);

    return 0;
}

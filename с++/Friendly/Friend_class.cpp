#include <iostream>
#include <string>

using namespace std;

// Прототип класса Person необходим, чтобы компилятор знал о его существовании 
// до того, как мы объявим его другом внутри класса Dog.
class Person;


class Dog {
private:
    int hp = 100; // Закрытое поле, скрытое от внешнего мира
public:
    // Объявление дружественного класса. 
    // Ключевое слово friend дает классу Person полный доступ ко всем private-полям класса Dog.
    friend class Person;
};


class Person {
public:
    // Методы класса Person могут свободно обращаться к private-полям класса Dog (например, dog.hp),
    // так как класс Person объявлен другом класса Dog.
    void Damage(Dog &dog) {
        dog.hp -= 20;
        cout << "Animal HP: " << dog.hp << endl;
    }

    void Heal(Dog &dog) {
        dog.hp += 10;
        cout << "Animal HP: " << dog.hp << endl;
    }

    void Kill(Dog &dog) {
        dog.hp = 0;
        cout << "Animal HP: " << dog.hp << endl;
    }
};


int main() {
    Dog mops;
    Person alex;

    // Объект класса Person управляет закрытыми данными объекта Dog благодаря дружественной связи
    alex.Damage(mops);
    alex.Damage(mops);
    alex.Heal(mops);

    return 0;
}
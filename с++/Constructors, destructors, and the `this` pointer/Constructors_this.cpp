#include <iostream>
#include <string>

using namespace std;

class Player {
public:
    string name;
    int hp;

    // Конструктор по умолчанию (без параметров)
    Player() {
        name = "Unknown";
        hp = 100;
        cout << "Сработал конструктор! Создан дефолтный игрок." << endl;
    }

    // Конструктор с параметрами (самый частый случай)
    Player(string name, int hp) {
        // this->name — это поле класса (string name выше)
        // просто name — это аргумент, который прилетел в конструктор
        this->name = name;
        this->hp = hp;
        cout << "Сработал конструктор с параметрами для: " << name << endl;
    }

    void get_info() {
        cout << "player name: " << name << endl << "player hp: " << hp << endl;
    }
};

int main() {
    // Теперь при создании объекта сразу передаем параметры в конструктор
    Player p1("Stirner", 100);
    // Или не передаем если создали конструктор по умолчанию
    Player p2;
    
    p2.get_info();
    p1.get_info();

    return 0;
}
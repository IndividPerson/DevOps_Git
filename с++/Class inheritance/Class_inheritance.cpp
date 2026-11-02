#include <iostream>
#include <string>

using namespace std;

// Клас протектед дает доступ класам наследникам 
// Базовый (родительский) класс
class PC {
private:
    int diagonal;
    string os;

public:
    // Конструктор базового класса
    PC(int diagonal, string os) {   
        this->diagonal = diagonal;
        this->os = os;
    }

    // Метод вывода информации базового класса
    void get_info() {
        cout << "OS: " << os << " Diagonal: " << diagonal << endl;
    }
};

// Производный (дочерний) класс Laptop наследует публичные члены класса PC
class Laptop: public PC {
private:
    float webka;

public:
    // Конструктор дочернего класса: через двоеточие передаем параметры в конструктор базового класса PC
    Laptop(int diagonal, string os, float webka): PC(diagonal, os) {
        this->webka = webka; // Инициализируем собственное поле дочернего класса
    }

    // Переопределение (модификация) метода get_info для дочернего класса
    void get_info() {
        PC::get_info(); // Вызываем метод базового класса, чтобы не дублировать код
        cout << "Webka: " << webka << endl; // Добавляем вывод нового поля
    }
};


int main() {
    // Создаем объект дочернего класса Laptop. Он содержит как поля родителя (PC), так и свои собственные.
    Laptop mac(16, "Mac", 1.5);
    
    // Вызывается переопределенный метод из класса Laptop
    mac.get_info();

    return 0;
}
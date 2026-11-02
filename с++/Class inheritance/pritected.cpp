#include <iostream>
#include <string>

using namespace std;

class PC {
protected: // Теперь эти поля видны внутри класса PC и во всех дочерних классах
    int diagonal;
    string os;

public:
    PC(int diagonal, string os) {   
        this->diagonal = diagonal;
        this->os = os;
    }

    void get_info() {
        cout << "OS: " << os << " Diagonal: " << diagonal << endl;
    }
};

class Laptop : public PC {
private:
    float webka;

public:
    Laptop(int diagonal, string os, float webka) {
        // Раньше здесь обязательно нужен был вызов через двоеточие : PC(diagonal, os),
        // так как поля были private. Теперь мы можем обращаться к ним напрямую, 
        // потому что они protected!
        this->diagonal = diagonal;
        this->os = os;
        this->webka = webka;
    }

    void get_info() {
        // Мы даже внутри методов Laptop можем напрямую использовать os и diagonal,
        // а не только вызывать PC::get_info()
        cout << "Laptop OS: " << os << ", Diagonal: " << diagonal << ", Webka: " << webka << endl;
    }
};

int main() {
    Laptop mac(16, "Mac", 1.5);
    mac.get_info();

    // mac.diagonal = 17; // Ошибка! Из main() к protected-полям по-прежнему нет доступа.

    return 0;
}
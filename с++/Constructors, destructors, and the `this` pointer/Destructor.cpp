class Player {
public:
    string name;

    Player(string name) {
        this->name = name;
        cout << "Объект " << name << " создан." << endl;
    }

    // Деструктор
    ~Player() {
        cout << "Объект " << name << " уничтожен (сработал деструктор)." << endl;
    }
};

int main() {
    Player p1("Stirner");
    // Когда функция main завершится, деструктор для p1 вызовется автоматически
    return 0;
}
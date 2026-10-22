#include <iostream>
// руская рулетка
using namespace std;

int main() {
    srand(time(NULL));
    int random = 1 + rand() % 6;
    int enter;

// Массив для отслеживания введенных чисел (индексы от 0 до 6)
    bool tried[7] = {false};

    while(true) {

        cout << "введите число от 1 до 6: " << endl;
        cin >> enter;

        if(enter < 1 || enter > 6) {
            cout << "число не в диапазоне 1 - 6!\n";
            continue;
        }

        if (tried[enter]) {
            cout << "Вы уже вводили это число! Выберите другое.\n\n";
            continue; // Пропускаем ход, не давая ввести повторно
        }
        
        // 3. Запоминаем, что это число теперь использовано
        tried[enter] = true;

        if(enter == random) {
            cout << "you are dead xD" << endl;
            break;
        }

        cout << "you are luсky!\n";
        
    }


    return 0;
}

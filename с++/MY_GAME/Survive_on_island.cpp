#include <iostream>
#include <string>
#include <thread> // Подключаем библиотеку для работы с паузами (sleep)
#include <chrono> // Подключаем библиотеку для указания единиц времени (миллисекунды)

using namespace std;

const string COLOR_BOLD = "\033[1m";     // Жирный текст (отлично для названий и важных событий)
const string COLOR_UNDERLINE = "\033[4m"; // Подчеркнутый текст

// Серые оттенки для основного текста и атмосферы
const string COLOR_GRAY = "\033[90m";       // Темно-серый / приглушенный (хорошо для мыслей или фона)
const string COLOR_LIGHT_GRAY = "\033[37m"; // Светло-серый (идеально для основного текста)

const string COLOR_RESET = "\033[0m";
const string COLOR_BROWN = "\033[38;5;94m"; // Классический темно-коричневый
const string COLOR_RED = "\033[31m";       // Опасность, урон, враги
const string COLOR_GREEN = "\033[32m";     // Здоровье, еда, успех
const string COLOR_YELLOW = "\033[33m";    // Предупреждения, голод, усталость
const string COLOR_BLUE = "\033[34m";      // Вода, океан, ночь
const string COLOR_MAGENTA = "\033[35m";   // Редкие предметы, магия, тайны
const string COLOR_CYAN = "\033[36m";      // Описание окружения, атмосфера
const string COLOR_WHITE = "\033[37m";     // Яркий белый текст

// Яркие версии цветов (High Intensity) — выглядят еще сочнее в темной консоли
const string COLOR_BRIGHT_RED = "\033[91m";
const string COLOR_BRIGHT_GREEN = "\033[92m";
const string COLOR_BRIGHT_YELLOW = "\033[93m";
const string COLOR_BRIGHT_BLUE = "\033[94m";
const string COLOR_BRIGHT_CYAN = "\033[96m";


class Game {
protected:

public:
    int days = 0;
};


class Player {
protected:
    int hp = 100;
    int energy = 100;


public:
    void get_hp() {
        int filled = hp / 10;                 // сколько клеток закрашено
        cout << "Health: [";
        for (int i = 0; i < 10; i++) {
            cout << (i < filled ? "█" : "░"); // условие ? значение_если_да : значение_если_нет
        }
        cout << "] " << hp << "%" << endl;
    }

    void get_energy() {
        int filled = energy / 10;
        cout << "Energy: [";
        for (int i = 0; i < 10; i++) {
            cout << (i < filled ? "█" : "░");
        }
        cout << "] " << energy << "%" << endl;
    }
    
};



void typer(string text, string color, int ms) {
    cout << color;
    for(char c : text) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(ms));
    }
    cout << "\033[0m";
}

void typer(string text, int ms) {
    for(char c : text) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(ms));
    }
}

void clearScreen() { 
    cout << "\033[2J\033[H";
}

void show_question() {
    typer("Where would you like to go?", 60);
    this_thread::sleep_for(chrono::milliseconds(1500));
    cout << endl;
    typer("1. Lake", COLOR_BLUE, 60);
    this_thread::sleep_for(chrono::milliseconds(600));
    cout << endl;
    typer("2. Forest", COLOR_GREEN, 60);
    this_thread::sleep_for(chrono::milliseconds(600));
    cout << endl;
    typer("3. Mountain", COLOR_BROWN, 60);
    this_thread::sleep_for(chrono::milliseconds(600));
    cout << endl;
    typer("4. Cave", COLOR_GRAY, 60);
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << endl << endl;
    typer("(1/2/3/4)", 60);
    this_thread::sleep_for(chrono::milliseconds(270));
    cout << endl;
    typer("Enter your answer: ", COLOR_BOLD, 60);
}

//  Functions of LOCATIONS
void lake() {
    
}

void forest() {

}

void mountain() {

}

void cave() {

}


// MAIN ----------------------------------------------------------------------------------------------

int main() {

    /*
    typer("Hello!", COLOR_BOLD, 70);
    this_thread::sleep_for(chrono::milliseconds(600));
    cout << endl;
    typer("You were flying off for a vacation in the mountains and ended up in a plane crash.", 50);
    this_thread::sleep_for(chrono::milliseconds(600));
    cout << endl;
    typer("Unfortunately, ", 60);
    this_thread::sleep_for(chrono::milliseconds(600));
    typer("everyone ", 60);
    typer("DIED.", COLOR_RED, 170);
    this_thread::sleep_for(chrono::milliseconds(600));
    cout << endl;
    typer("But not you!", 60);
    this_thread::sleep_for(chrono::milliseconds(600));
    cout << endl;
    typer("You miraculously ", 60); 
    typer("survived!", COLOR_BOLD, 60);
    this_thread::sleep_for(chrono::milliseconds(600));
    cout << endl;
    typer("Now you’ll have to fight nature for your life.", 55);
    this_thread::sleep_for(chrono::milliseconds(600));
    cout << endl;
    typer("the goal is to ", 60);
    typer("survive 7 days",COLOR_YELLOW, 60);
    this_thread::sleep_for(chrono::milliseconds(2000));
    cout << endl;
    typer("Brutal.", COLOR_BRIGHT_RED, 160);
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << endl << endl;
    typer("Day 1", COLOR_BOLD, 100);
    this_thread::sleep_for(chrono::milliseconds(2000));
    cout << endl;
    */
    cout << "-----------------------------------------" << endl << endl;
    

    string items[4] {"heal elexir", "sword", "rock", "energy drink" };

    Player player;
    player.get_hp();
    cout << endl;
    player.get_energy();
    cout << endl << endl;

    show_question();
    int ans;
    cin >> ans;








    return 0;
}

#include <iostream>
#include <string>
#include <vector>
#include <thread> // Подключаем библиотеку для работы с паузами (sleep)
#include <chrono> // Подключаем библиотеку для указания единиц времени (миллисекунды)
#include <random>

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

const string COLOR_BRIGHT_RED = "\033[91m";
const string COLOR_BRIGHT_GREEN = "\033[92m";
const string COLOR_BRIGHT_YELLOW = "\033[93m";
const string COLOR_BRIGHT_BLUE = "\033[94m";
const string COLOR_BRIGHT_CYAN = "\033[96m";


mt19937 rng(random_device{}());   // генератор, засеянный случайным числом

int randomInt(int a, int b) {
    return uniform_int_distribution<int>(a, b)(rng);
}

bool chance(int percent) {
    return randomInt(1, 100) <= percent;
}


class Game {
public:
    int days = 1;            
    bool bossAlive = true;   
};


class Player {
protected:
    int hp = 100;
    int energy = 100;
    vector<string> items;    // ИНВЕНТАРЬ: список предметов     vector это список, который сам растёт и уменьшается.

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

    // --- здоровье и энергия ---
    void takeDamage(int d) {
        hp -= d;
        if (hp < 0) hp = 0;
    }

    void heal(int h) {
        hp += h;
        if (hp > 100) hp = 100;
    }

    void changeEnergy(int e) {
        energy += e;
        if (energy > 100) energy = 100;
        if (energy < 0) energy = 0;
    }

    int getEnergy() {
        return energy;
    }

    bool isAlive() {
        return hp > 0;
    }

    // --- инвентарь ---
    void addItem(string item) {
        items.push_back(item);
    }

    bool hasItem(string item) {
        for (string i : items) {
            if (i == item) return true;
        }
        return false;
    }

    void removeItem(string item) {
        for (size_t i = 0; i < items.size(); i++) {
            if (items[i] == item) {
                items.erase(items.begin() + i);   // удаляем ОДИН предмет
                return;
            }
        }
    }

    void showInventory() {
        cout << "Inventory: ";
        if (items.empty()) cout << "(empty)";
        for (string item : items) {
            cout << "[" << item << "] ";
        }
        cout << endl;
    }
};

// Один игрок и одна игра на всю программу, чтобы все функции их видели
Player player;
Game game;


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

void waitEnter() {
    cout << endl << "Press Enter to continue...";
    cin.ignore(10000, '\n');   // убираем старый Enter из буфера
    cin.get();                 // ждём новый Enter
}

void show_question() {
    typer("Where would you like to go?", 60);
    this_thread::sleep_for(chrono::milliseconds(1300));
    cout << endl;
    typer("1. Lake", COLOR_BLUE, 60);
    this_thread::sleep_for(chrono::milliseconds(400));
    cout << endl;
    typer("2. Forest", COLOR_GREEN, 60);
    this_thread::sleep_for(chrono::milliseconds(400));
    cout << endl;
    typer("3. Cave", COLOR_GRAY, 60);
    this_thread::sleep_for(chrono::milliseconds(400));
    cout << endl;
    typer("4. Home (rest)", 60);
    this_thread::sleep_for(chrono::milliseconds(400));
    cout << endl;
    typer("5. Inventory", COLOR_YELLOW, 60);
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << endl << endl;
    typer("(1/2/3/4/5)", 60);
    this_thread::sleep_for(chrono::milliseconds(150));
    cout << endl;
    typer("Enter your answer: ", COLOR_BOLD, 60);
}


void pathway(string location, string color) {

    player.changeEnergy(-40);   // каждая поездка стоит 40 энергии

    typer("You are going to: ", 60);
    typer(location, color, 77);
    cout << endl;
    this_thread::sleep_for(chrono::milliseconds(1500));
    clearScreen();

    typer("Traveling", 65);
    this_thread::sleep_for(chrono::milliseconds(800));
    for (int i = 0; i < 4; i++) {
        
        cout << "\rTraveling .  " << flush;
        this_thread::sleep_for(chrono::milliseconds(450));
        
        cout << "\rTraveling .. " << flush;
        this_thread::sleep_for(chrono::milliseconds(450));
        
        cout << "\rTraveling ..." << flush;
        this_thread::sleep_for(chrono::milliseconds(450));
    }
    cout << endl;
    clearScreen();
}

void fishing() {
    typer("You cast the fishing line.\n", 50);
    this_thread::sleep_for(chrono::milliseconds(400));
    typer("You wait for a bite...\n\n", 50);
    this_thread::sleep_for(chrono::milliseconds(1600));


    if (chance(50)) {
        int count = randomInt(1, 2);                 // 1 или 2 рыбы
        for (int i = 0; i < count; i++) {
            player.addItem("fish");                  // кладём рыбу в инвентарь
        }
        typer("You caught " + to_string(count) + " fish!\n", COLOR_GREEN, 50);
    } else {
        typer("Nothing bites.\n", COLOR_YELLOW, 50);
    }
}


//  Functions of LOCATIONS ----------------------------------------------------------------------------

void home() {
    typer("You rest at home. You feel stronger.\n", 55);
    player.changeEnergy(40);
}

void lake() {
    
    pathway("lake", COLOR_BRIGHT_BLUE);
    cout << endl;
    typer("You arrive at the edge of a wide, flowing river.\n", 55);
    this_thread::sleep_for(chrono::milliseconds(590));
    typer("You can hear the gentle current crashing against smooth stones,\n", 50);
    this_thread::sleep_for(chrono::milliseconds(590));
    typer("offering a spark of hope for clean water...\n", 55);
    this_thread::sleep_for(chrono::milliseconds(590));
    typer("Luckily, near the river, you found a barrel containing three fishing rods!\n\n", 55);
    this_thread::sleep_for(chrono::milliseconds(1000));
    typer("Would you prefer to do a bit of fishing, or will you head back? (1 - yes / 2 - no)\n", 50);
    int ans_lake;
    cin >> ans_lake;

    if (ans_lake == 1) {
        fishing();
    }

    else if (ans_lake == 2) {
        home();
    }

    else {
        cout << "you are stupid!\n";
    }
}

void forest() {

    pathway("forest", COLOR_GREEN);
    cout << endl;
    typer("The forest is dark and quiet.\n", 55);
    this_thread::sleep_for(chrono::milliseconds(800));

    if (player.hasItem("spear")) {
        typer("You already have a spear. You find nothing new.\n", 55);
        return;   // выходим из функции
    }

    typer("You look around for something sharp...\n", 55);
    this_thread::sleep_for(chrono::milliseconds(1500));

    if (chance(60)) {
        typer("You found a strong stick and a sharp stone!\n", 55);
        this_thread::sleep_for(chrono::milliseconds(600));
        typer("You made a spear!\n", COLOR_GREEN, 55);
        player.addItem("spear");
    } else {
        typer("You search for a long time, but you find nothing.\n", COLOR_YELLOW, 55);
    }
}

void cave() {

    pathway("cave", COLOR_GRAY);
    cout << endl;
    typer("It is cold and dark inside the cave.\n", 55);
    this_thread::sleep_for(chrono::milliseconds(800));

    if (!game.bossAlive) {
        typer("The cave is quiet now. The bear is dead.\n", 55);
        return;
    }

    typer("Suddenly, you hear a loud roar!\n", COLOR_RED, 55);
    this_thread::sleep_for(chrono::milliseconds(800));
    typer("A huge bear is standing in front of you!\n\n", 55);
    this_thread::sleep_for(chrono::milliseconds(1000));

    // 1) Нет оружия - убить медведя нельзя
    if (!player.hasItem("spear")) {
        typer("You have no weapon. You must run away!\n", COLOR_YELLOW, 55);
        this_thread::sleep_for(chrono::milliseconds(800));
        player.takeDamage(40);
        typer("The bear hits you. (-40 HP)\n", COLOR_RED, 55);
        return;
    }

    // 2) Есть оружие - шанс 60%, а с едой - 100%
    typer("You hold your spear tight.\n", 55);
    this_thread::sleep_for(chrono::milliseconds(600));

    int winChance = 60;
    if (player.hasItem("fish")) {
        typer("You have a fish. You eat it during the fight and heal yourself.\n", COLOR_GREEN, 55);
        player.removeItem("fish");
        winChance = 100;
    } else {
        typer("You have no food to heal yourself. This is dangerous!\n", COLOR_YELLOW, 55);
    }
    this_thread::sleep_for(chrono::milliseconds(1000));

    typer("The fight begins!\n\n", COLOR_BRIGHT_RED, 70);
    this_thread::sleep_for(chrono::milliseconds(2000));

    if (chance(winChance)) {
        typer("You hit the bear with your spear. It falls down!\n", COLOR_GREEN, 55);
        this_thread::sleep_for(chrono::milliseconds(600));
        typer("In the back of the cave you find a first aid kit. (+40 HP)\n", COLOR_GREEN, 55);
        player.heal(40);
        game.bossAlive = false;
    } else {
        typer("You miss! The bear hits you hard. (-40 HP)\n", COLOR_RED, 55);
        player.takeDamage(40);
        player.removeItem("spear");
        typer("Your spear is broken. You must make a new one.\n", COLOR_YELLOW, 55);
    }
}


// Ночь: если есть рыба - ты ешь, если нет - теряешь здоровье
void night() {
    cout << endl;
    typer("Night falls...\n", COLOR_BLUE, 70);
    this_thread::sleep_for(chrono::milliseconds(1500));

    if (player.hasItem("fish")) {
        player.removeItem("fish");
        player.heal(15);
        typer("You eat one fish. You feel better. (+15 HP)\n", COLOR_GREEN, 55);
    } else {
        player.takeDamage(20);
        typer("You have no food. You are very hungry. (-20 HP)\n", COLOR_RED, 55);
    }
    player.changeEnergy(20);   // за ночь ты немного отдыхаешь
}


// MAIN ----------------------------------------------------------------------------------------------

int main() {

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
    typer("The goal is to ", 60);
    typer("survive 5 days.", COLOR_YELLOW, 60);
    this_thread::sleep_for(chrono::milliseconds(2000));
    cout << endl;
    typer("Brutal.", COLOR_BRIGHT_RED, 160);
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << endl << endl;


    // ГЛАВНЫЙ ЦИКЛ: один проход = один день
    while (game.days <= 5 && player.isAlive()) {

        clearScreen();
        typer("Day " + to_string(game.days), COLOR_BOLD, 100);
        cout << endl << endl;
        cout << "-----------------------------------------" << endl << endl;

        player.get_hp();
        cout << endl;
        player.get_energy();
        cout << endl;
        cout << "-----------------------------------------" << endl << endl;

        show_question();
        bool chosen = false;

        while (!chosen) {
            int ans;
            

            // Для поездки нужно 40 энергии
            if (ans >= 1 && ans <= 3 && player.getEnergy() < 40) {
                typer("You are too tired to travel. Go home and rest.\n", COLOR_YELLOW, 55);
                continue;                 // вернёмся к началу цикла while
            }

            switch (ans) {
                case 1:
                    lake();
                    chosen = true;
                    break;
                case 2:
                    forest();
                    chosen = true;
                    break;
                case 3:
                    cave();
                    chosen = true;
                    break;
                case 4:
                    home();
                    chosen = true;
                    break;
                case 5:
                    player.showInventory();
                    typer("Enter your answer: ", COLOR_BOLD, 40);
                    break;                  // chosen остаётся false, цикл повторится
                default:
                    typer("Wrong choice, try again.\n", 55);
                    break;
            }
        }

        if (!player.isAlive()) break;       // умер днём - выходим из игры

        if (game.days < 5) night();         // после 5-го дня ночь не нужна
        game.days++;

        if (!player.isAlive()) break;       // умер ночью
        waitEnter();
    }


    // КОНЕЦ ИГРЫ
    cout << endl << endl;
    if (player.isAlive()) {
        typer("You survived 5 days!", COLOR_GREEN, 70);
        cout << endl;
        this_thread::sleep_for(chrono::milliseconds(1000));
        typer("A rescue helicopter is flying over the island. You are saved!", 55);
        cout << endl;
    } else {
        typer("You died on the island...", COLOR_RED, 80);
        cout << endl;
        this_thread::sleep_for(chrono::milliseconds(1000));
        typer("GAME OVER", COLOR_BRIGHT_RED, 120);
        cout << endl;
    }

    return 0;
}
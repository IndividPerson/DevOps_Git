// ============================================================================
//  LOST ON THE ISLAND  -  текстовая пошаговая игра на выживание (C++17, Linux)
//
//  Компиляция:  g++ -std=c++17 -O2 -Wall -Wextra island.cpp -o island
//  Запуск:      ./island
//
//  Звук (по желанию): положи .wav файлы в папку sounds/ рядом с игрой.
//  Если файла нет, игра просто молчит. Нужен paplay (PulseAudio/PipeWire).
//  Имена файлов: wave, splash, hurt, eat, hammer, thunder, hiss, radio, ship
//
//  Структура файла (читай по порядку):
//    1. Цвета          4. Предметы и данные
//    2. Утилиты        5. class Game (вся логика)
//    3. Ввод/вывод     6. main()
// ============================================================================
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <thread>
#include <vector>

// ----------------------------------------------------------------------------
// 1. ЦВЕТА (ANSI escape-коды)
// ----------------------------------------------------------------------------
namespace C {
inline constexpr const char* RESET   = "\033[0m";
inline constexpr const char* BOLD    = "\033[1m";
inline constexpr const char* RED     = "\033[31m";
inline constexpr const char* GREEN   = "\033[32m";
inline constexpr const char* YELLOW  = "\033[33m";
inline constexpr const char* BLUE    = "\033[34m";
inline constexpr const char* MAGENTA = "\033[35m";
inline constexpr const char* CYAN    = "\033[36m";
inline constexpr const char* GRAY    = "\033[90m";
inline constexpr const char* ORANGE  = "\033[38;5;208m";
}  // namespace C
using namespace C;

// 256-цветная палитра: c256(208) = оранжевый, c256(27) = синий и т.д.
std::string c256(int n) { return "\033[38;5;" + std::to_string(n) + "m"; }

// ----------------------------------------------------------------------------
// 2. УТИЛИТЫ: случайные числа
// ----------------------------------------------------------------------------
std::mt19937 rng{std::random_device{}()};

int rnd(int a, int b) { return std::uniform_int_distribution<int>(a, b)(rng); }
bool chance(int percent) { return rnd(1, 100) <= percent; }

template <class T>
const T& pick(const std::vector<T>& v) {
    return v[rnd(0, static_cast<int>(v.size()) - 1)];
}

// ----------------------------------------------------------------------------
// 3. ВВОД / ВЫВОД
// ----------------------------------------------------------------------------
bool typewriter = true;

void clearScreen() { std::cout << "\033[2J\033[H"; }

// Печатная машинка. Не задерживается на цветовых кодах, на точках пауза дольше.
void typeText(const std::string& s, int ms = 18) {
    bool inEsc = false;
    for (unsigned char c : s) {
        std::cout << static_cast<char>(c);
        if (!typewriter) continue;
        if (c == '\033') { inEsc = true; continue; }
        if (inEsc) { if (c == 'm') inEsc = false; continue; }
        if ((c & 0xC0) == 0x80) continue;  // хвост юникод-символа
        int d = ms;
        if (c == '.' || c == '!' || c == '?') d = ms * 8;
        else if (c == ',') d = ms * 4;
        std::cout << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(d));
    }
    std::cout << std::endl;
}

bool soundOn = true;

// Проигрывает sounds/<name>.wav в фоне, если файл существует.
void playSound(const std::string& name) {
    if (!soundOn) return;
    std::string path = "sounds/" + name + ".wav";
    if (!std::ifstream(path)) return;
    std::string cmd = "paplay " + path + " >/dev/null 2>&1 &";
    int r = std::system(cmd.c_str());
    (void)r;
}

int askChoice(int maxN, bool allowZero = false) {
    int lo = allowZero ? 0 : 1;
    while (true) {
        std::cout << BOLD << "> " << RESET << std::flush;
        std::string line;
        if (!std::getline(std::cin, line)) { std::cout << "\n"; std::exit(0); }
        try {
            int n = std::stoi(line);
            if (n >= lo && n <= maxN) return n;
        } catch (...) {}
        std::cout << GRAY << "Enter a number from " << lo << " to " << maxN << ".\n" << RESET;
    }
}

void pressEnter() {
    std::cout << GRAY << "\n[Press Enter]" << RESET << std::flush;
    std::string s;
    std::getline(std::cin, s);
}

// ----------------------------------------------------------------------------
// 4. ПРЕДМЕТЫ И ДАННЫЕ
// ----------------------------------------------------------------------------
enum class Item {
    Coconut, Berries, Herb, Egg, RawFish, CookedFish, RawMeat, CookedMeat,
    Wood, Stone, Vine, Cloth, Spear, Axe, Bottle, CleanWater, RadioPart, Battery
};

struct ItemInfo { const char* name; const char* color; };

const std::map<Item, ItemInfo> ITEMS = {
    {Item::Coconut,    {"Coconut",      YELLOW}},
    {Item::Berries,    {"Berries",      MAGENTA}},
    {Item::Herb,       {"Healing herb", GREEN}},
    {Item::Egg,        {"Bird egg",     YELLOW}},
    {Item::RawFish,    {"Raw fish",     CYAN}},
    {Item::CookedFish, {"Cooked fish",  ORANGE}},
    {Item::RawMeat,    {"Raw meat",     RED}},
    {Item::CookedMeat, {"Cooked meat",  ORANGE}},
    {Item::Wood,       {"Wood",         GREEN}},
    {Item::Stone,      {"Stone",        GRAY}},
    {Item::Vine,       {"Vine",         GREEN}},
    {Item::Cloth,      {"Cloth",        BLUE}},
    {Item::Spear,      {"Spear",        BOLD}},
    {Item::Axe,        {"Axe",          BOLD}},
    {Item::Bottle,     {"Empty bottle", CYAN}},
    {Item::CleanWater, {"Bottle of water", CYAN}},
    {Item::RadioPart,  {"Radio part",   RED}},
    {Item::Battery,    {"Battery",      RED}},
};

std::string itemName(Item i) {
    const ItemInfo& in = ITEMS.at(i);
    return std::string(in.color) + in.name + RESET;
}

using Needs = std::map<Item, int>;  // рецепт: предмет -> сколько нужно

enum class Loc { Beach, Jungle, Cave, Wreck, Mountain };
enum class Weather { Clear, Rain, Heat, Storm };

const char* locName(Loc l) {
    switch (l) {
        case Loc::Beach:    return "The Beach";
        case Loc::Jungle:   return "The Jungle";
        case Loc::Cave:     return "The Cave";
        case Loc::Wreck:    return "The Shipwreck";
        case Loc::Mountain: return "The Mountain";
    }
    return "";
}

std::string locColor(Loc l) {
    switch (l) {
        case Loc::Beach:    return YELLOW;
        case Loc::Jungle:   return GREEN;
        case Loc::Cave:     return GRAY;
        case Loc::Wreck:    return ORANGE;
        case Loc::Mountain: return c256(250);
    }
    return RESET;
}

const char* locArt(Loc l) {
    switch (l) {
        case Loc::Beach: return R"ART(
       \ | /
        \|/
         |
 ~~~  ___|___  ~~~
~~~~~~~~~~~~~~~~~~~~)ART";
        case Loc::Jungle: return R"ART(
   &&&     &&&&&    &&&
  &&&&&   &&&&&&&  &&&&&
   ||       |||      ||)ART";
        case Loc::Cave: return R"ART(
    _.-'''''''-._
  .'   .     .   '.
 /   .    .   .    \
 |__________________|)ART";
        case Loc::Wreck: return R"ART(
         |\
         | \
     ____|__\____
     \  o  o  o  /
~~~~~~\_________/~~~~~)ART";
        case Loc::Mountain: return R"ART(
        /\
       /  \      /\
      / /\ \    /  \
     / /  \ \  /    \
    /_/    \_\/______\)ART";
    }
    return "";
}

const char* SKULL = R"ART(
      _____
     /     \
    | () () |
     \  ^  /
      |||||
      |||||)ART";

struct Action {
    std::string label;
    std::function<bool()> run;  // true = ход потрачен, false = действие бесплатное
};

// ----------------------------------------------------------------------------
// 5. ИГРА
// ----------------------------------------------------------------------------
class Game {
public:
    Game() {
        inv[Item::Bottle] = 1;
        wreckLoot = {Item::RadioPart, Item::RadioPart, Item::RadioPart,
                     Item::Cloth, Item::Cloth, Item::Cloth, Item::Cloth,
                     Item::Vine, Item::Vine, Item::Bottle, Item::Herb,
                     Item::Wood, Item::Wood};
        std::shuffle(wreckLoot.begin(), wreckLoot.end(), rng);
    }

    void run() {
        intro();
        while (!over) {
            showStatus();
            bool turnUsed = false;
            while (!turnUsed && !over) turnUsed = playerAction();
            if (over) break;
            endTurn();
            if (!over) pressEnter();
        }
        outro();
    }

private:
    // ---------------- состояние ----------------
    int health = 100, hunger = 80, thirst = 80, energy = 100, sanity = 100;
    int day = 1, phase = 0;  // phase: 0 утро, 1 день, 2 вечер, 3 ночь
    static constexpr int GOAL_DAYS = 20;

    Loc loc = Loc::Beach;
    Weather weather = Weather::Clear;
    std::map<Item, int> inv;

    int campfire = 0;      // сколько ходов ещё горит костёр
    int signalTurns = 0;   // сколько ходов ещё горит сигнальный огонь
    int raft = 0;          // стадия плота 0..3
    int radioTries = 0;
    int notesFound = 0;
    bool shelter = false, radioBuilt = false, batteryFound = false;
    bool over = false, won = false;

    std::vector<Item> wreckLoot;
    std::vector<std::string> diary, todayEvents;
    int mealsEaten = 0, itemsCrafted = 0, fishCaught = 0;
    std::string endTitle, endText, lastCause = "unknown";

    // ---------------- инвентарь ----------------
    bool has(Item i, int n = 1) const {
        auto it = inv.find(i);
        return it != inv.end() && it->second >= n;
    }
    void add(Item i, int n = 1) { inv[i] += n; }
    void take(Item i, int n = 1) {
        inv[i] -= n;
        if (inv[i] <= 0) inv.erase(i);
    }
    bool canAfford(const Needs& n) const {
        for (const auto& [it, q] : n) if (!has(it, q)) return false;
        return true;
    }
    void pay(const Needs& n) { for (const auto& [it, q] : n) take(it, q); }
    std::string needsText(const Needs& n) const {
        std::string s;
        bool first = true;
        for (const auto& [it, q] : n) {
            if (!first) s += ", ";
            first = false;
            s += std::string(has(it, q) ? GREEN : RED) + std::to_string(q) + "x " +
                 ITEMS.at(it).name + RESET;
        }
        return s;
    }

    // ---------------- статы ----------------
    void gain(int& stat, int n) { stat = std::clamp(stat + n, 0, 100); }
    void clampStats() {
        health = std::clamp(health, 0, 100);
        hunger = std::clamp(hunger, 0, 100);
        thirst = std::clamp(thirst, 0, 100);
        energy = std::clamp(energy, 0, 100);
        sanity = std::clamp(sanity, 0, 100);
    }
    void hurt(int n, const std::string& msg, const std::string& cause) {
        health -= n;
        lastCause = cause;
        playSound("hurt");
        say(RED, msg + " (-" + std::to_string(n) + " health)");
    }

    // ---------------- текст ----------------
    // Чем ниже рассудок, тем сильнее "глючит" текст.
    std::string glitch(std::string s) {
        if (sanity >= 35) return s;
        int p = (35 - sanity) * 2;
        for (char& c : s) {
            if (!chance(p)) continue;
            switch (c) {
                case 'a': c = '4'; break;
                case 'e': c = '3'; break;
                case 'i': c = '1'; break;
                case 'o': c = '0'; break;
                case 's': c = '5'; break;
                case 't': c = '7'; break;
                default: break;
            }
        }
        return s;
    }
    void say(const std::string& color, const std::string& text) {
        typeText(color + glitch(text) + RESET);
    }
    void note(const std::string& s) { todayEvents.push_back(s); }

    // ---------------- интро / финал ----------------
    void intro() {
        clearScreen();
        std::cout << c256(33) << BOLD << R"ART(
  _     ___  ____ _____    ___  _   _   _____ _   _ _____
 | |   / _ \/ ___|_   _|  / _ \| \ | | |_   _| | | | ____|
 | |  | | | \___ \ | |   | | | |  \| |   | | | |_| |  _|
 | |__| |_| |___) || |   | |_| | |\  |   | | |  _  | |___
 |_____\___/|____/ |_|    \___/|_| \_|   |_| |_| |_|_____|
              ___ ____  _        _    _   _ ____
             |_ _/ ___|| |      / \  | \ | |  _ \
              | |\___ \| |     / _ \ |  \| | | | |
              | | ___) | |___ / ___ \| |\  | |_| |
             |___|____/|_____/_/   \_\_| \_|____/
)ART" << RESET << "\n";
        std::cout << "Typewriter text effect?\n  1) On\n  2) Off (instant)\n";
        typewriter = askChoice(2) == 1;

        clearScreen();
        say(c256(33), "The storm took the Marigold in the middle of the night.");
        say(c256(33), "You remember the cold water, a splintered plank, and then... sand.");
        std::cout << "\n";
        say(YELLOW, "You wake up on a beach. The wreck of your ship lies in the surf.");
        say(GREEN, "Goal: survive " + std::to_string(GOAL_DAYS) + " days, or find a way off the island.");
        say(GRAY, "Tip: checking your inventory, eating and cooking are free. They don't use up time.");
        pressEnter();
    }

    void outro() {
        clearScreen();
        const char* col = won ? GREEN : RED;
        if (!won) std::cout << RED << SKULL << RESET << "\n";
        std::cout << "\n" << col << BOLD << "  *** " << endTitle << " ***" << RESET << "\n\n";
        typeText(endText);
        std::cout << "\n" << BOLD << "--- Your story ---" << RESET << "\n";
        std::cout << "Days survived:  " << std::min(day, GOAL_DAYS) << "\n";
        std::cout << "Meals eaten:    " << mealsEaten << "\n";
        std::cout << "Fish caught:    " << fishCaught << "\n";
        std::cout << "Things crafted: " << itemsCrafted << "\n";
        if (!diary.empty()) {
            std::cout << "\n" << BOLD << "--- Diary ---" << RESET << "\n";
            for (const auto& d : diary) std::cout << GRAY << d << RESET << "\n";
        }
        std::cout << "\n";
    }

    void endGame(bool win, const std::string& title, const std::string& text) {
        over = true;
        won = win;
        endTitle = title;
        endText = text;
    }

    // ---------------- экран статуса ----------------
    void bar(const char* name, int v, const char* color) {
        const char* col = v < 25 ? RED : color;
        std::cout << std::left << std::setw(8) << name << col << "[";
        int filled = (v + 5) / 10;
        for (int i = 0; i < 10; i++) std::cout << (i < filled ? "█" : "░");
        std::cout << "] " << std::right << std::setw(3) << v << RESET << "\n";
    }

    std::string weatherText() const {
        switch (weather) {
            case Weather::Clear: return std::string(YELLOW) + "Clear sky" + RESET;
            case Weather::Rain:  return std::string(CYAN) + "Rain" + RESET;
            case Weather::Heat:  return std::string(ORANGE) + "Scorching heat" + RESET;
            case Weather::Storm: return std::string(MAGENTA) + BOLD + "STORM" + RESET;
        }
        return "";
    }

    void showStatus() {
        static const char* names[] = {"Morning", "Noon", "Evening", "Night"};
        static const int cols[] = {220, 226, 135, 27};
        clearScreen();
        std::cout << c256(cols[phase]) << BOLD << "═══ Day " << day << "/" << GOAL_DAYS
                  << " ── " << names[phase] << " ═══" << RESET << "   " << weatherText() << "\n";
        std::cout << locColor(loc) << BOLD << locName(loc) << RESET << "\n";
        std::cout << locColor(loc) << locArt(loc) << RESET << "\n\n";

        bar("Health", health, RED);
        bar("Hunger", hunger, YELLOW);
        bar("Thirst", thirst, CYAN);
        bar("Energy", energy, GREEN);
        bar("Sanity", sanity, MAGENTA);

        std::cout << "\n" << BOLD << "Items: " << RESET;
        if (inv.empty()) std::cout << GRAY << "nothing" << RESET;
        bool first = true;
        for (const auto& [it, q] : inv) {
            if (!first) std::cout << ", ";
            first = false;
            std::cout << itemName(it) << " x" << q;
        }
        std::cout << "\n";

        std::string flags;
        if (campfire > 0) flags += std::string(ORANGE) + "[Campfire " + std::to_string(campfire) + "] " + RESET;
        if (shelter) flags += std::string(GREEN) + "[Shelter] " + RESET;
        if (signalTurns > 0) flags += std::string(RED) + "[Signal fire " + std::to_string(signalTurns) + "] " + RESET;
        if (raft > 0) flags += std::string(CYAN) + "[Raft " + std::to_string(raft) + "/3] " + RESET;
        if (radioBuilt) flags += std::string(BLUE) + "[Radio] " + RESET;
        if (!flags.empty()) std::cout << flags << "\n";
    }

    // ---------------- меню действий ----------------
    bool playerAction() {
        std::vector<Action> acts = buildActions();
        std::cout << "\n";
        for (size_t i = 0; i < acts.size(); i++)
            std::cout << BOLD << " " << (i + 1) << ") " << RESET << acts[i].label << "\n";
        int ch = askChoice(static_cast<int>(acts.size()));
        std::cout << "\n";
        return acts[ch - 1].run();
    }

    std::vector<Action> buildActions() {
        std::vector<Action> a;
        auto push = [&a](const std::string& label, std::function<bool()> fn) {
            a.push_back({label, std::move(fn)});
        };

        switch (loc) {
            case Loc::Beach:
                push("Fish in the shallows", [this] { return fish(); });
                push("Comb the shore for useful things", [this] { return combBeach(); });
                if (raft < 3)
                    push("Work on the raft: " + std::string(raftStageName()) + " (needs " +
                         needsText(raftNeeds(raft)) + ")", [this] { return workRaft(); });
                else
                    push("Set sail on the raft!", [this] { return setSail(); });
                break;
            case Loc::Jungle:
                push("Forage for food and materials", [this] { return forage(); });
                if (has(Item::Spear)) push("Hunt with your spear", [this] { return hunt(); });
                push("Chop wood" + std::string(has(Item::Axe) ? " (axe: double yield)" : ""),
                     [this] { return chopWood(); });
                push("Drink from the stream", [this] { return drinkStream(); });
                if (has(Item::Bottle)) push("Fill a bottle at the stream", [this] { return fillBottle(); });
                break;
            case Loc::Cave:
                push("Explore deeper", [this] { return exploreCave(); });
                push("Drink from dripping water", [this] { return dripWater(); });
                break;
            case Loc::Wreck:
                push("Search the wreck (" + std::to_string(wreckLoot.size()) + " spots left)",
                     [this] { return searchWreck(); });
                break;
            case Loc::Mountain:
                push("Scan the horizon", [this] { return scanHorizon(); });
                push("Hunt for bird eggs (risky climb)", [this] { return huntEggs(); });
                if (radioBuilt) push("Transmit a distress call", [this] { return transmit(); });
                break;
        }

        if (weather == Weather::Rain && loc != Loc::Cave && has(Item::Bottle))
            push("Collect rainwater in a bottle", [this] { return collectRain(); });

        push("Rest", [this] { return rest(); });
        push("Craft", [this] { return craftMenu(); });
        push("Eat / drink", [this] { return useMenu(); });
        if (campfire > 0 && (has(Item::RawFish) || has(Item::RawMeat)))
            push("Cook raw food on the fire", [this] { return cookFood(); });
        push("Travel to another place", [this] { return travel(); });
        push("Read your diary", [this] { return readDiary(); });
        push("Give up (quit)", [this] { return quitGame(); });
        return a;
    }

    // ---------------- действия: пляж ----------------
    bool fish() {
        if (weather == Weather::Storm) {
            say(MAGENTA, "The waves are too violent. You nearly get dragged out to sea.");
            gain(energy, -5);
            return true;
        }
        playSound("splash");
        say(CYAN, pick<std::string>({"You wade into the shallows and wait, motionless.",
                                     "You stand knee-deep in the surf, eyes on the water.",
                                     "The tide pools shimmer. Something moves beneath."}));
        if (chance(has(Item::Spear) ? 70 : 35)) {
            int n = rnd(1, 2);
            add(Item::RawFish, n);
            fishCaught += n;
            say(GREEN, "You caught " + std::to_string(n) + " fish!");
            note("Caught fish with my own hands. I feel like a king.");
        } else {
            say(GRAY, pick<std::string>({"Nothing bites. The fish seem to be laughing.",
                                         "A fish slips right through your fingers.",
                                         "You come back empty-handed."}));
        }
        gain(energy, -8);
        return true;
    }

    bool combBeach() {
        say(YELLOW, "You walk along the waterline, scanning the sand.");
        int r = rnd(1, 100);
        if (r <= 35) {
            int n = rnd(1, 3);
            add(Item::Wood, n);
            say(GREEN, "Driftwood! You collect " + std::to_string(n) + " pieces of " + itemName(Item::Wood) + ".");
        } else if (r <= 55) {
            add(Item::Stone, rnd(1, 2));
            say(GREEN, "You pick up a couple of flat " + itemName(Item::Stone) + "s.");
        } else if (r <= 68) {
            add(Item::Cloth);
            say(GREEN, "A scrap of sailcloth washed ashore. " + itemName(Item::Cloth) + " found!");
        } else if (r <= 78) {
            bottleNote();
        } else if (r <= 88) {
            add(Item::Coconut);
            say(GREEN, "A lonely " + itemName(Item::Coconut) + " rolled up on the beach.");
        } else {
            say(GRAY, "Just sand, shells and one very judgemental crab.");
        }
        gain(energy, -7);
        return true;
    }

    const char* raftStageName() const {
        static const char* n[] = {"hull", "sail", "provisions"};
        return n[std::min(raft, 2)];
    }
    Needs raftNeeds(int stage) const {
        switch (stage) {
            case 0:  return {{Item::Wood, 6}, {Item::Vine, 3}};
            case 1:  return {{Item::Wood, 2}, {Item::Cloth, 2}};
            default: return {{Item::CleanWater, 2}, {Item::CookedFish, 2}};
        }
    }
    bool workRaft() {
        Needs n = raftNeeds(raft);
        if (!canAfford(n)) {
            say(RED, "You don't have the materials yet. Needed: " + needsText(n));
            return false;
        }
        pay(n);
        raft++;
        playSound("hammer");
        gain(energy, -15);
        say(GREEN, "You work until your hands bleed. The raft takes shape (" + std::to_string(raft) + "/3).");
        note("Worked on the raft. Stage " + std::to_string(raft) + " of 3.");
        itemsCrafted++;
        return true;
    }
    bool setSail() {
        if (weather == Weather::Storm) {
            say(MAGENTA, "The sea is a monster today. Wait for calmer weather.");
            return false;
        }
        say(CYAN, "You push the raft into the surf and paddle away from the island...");
        int p = 55 + (health + energy) / 8 + (sanity > 50 ? 5 : 0);
        if (chance(p)) {
            endGame(true, "ESCAPED BY RAFT",
                    "Three days later a fishing boat spots your sail. You are saved, "
                    "sunburnt and half-mad, but alive.");
        } else {
            lastCause = "lost at sea";
            endGame(false, "LOST AT SEA",
                    "The current was stronger than your raft. The island shrinks behind you, "
                    "and then there is only water.");
        }
        return true;
    }

    // ---------------- действия: джунгли ----------------
    bool forage() {
        say(GREEN, "You push through thick leaves and hanging vines.");
        int r = rnd(1, 100);
        if (r <= 28) {
            int n = rnd(1, 2);
            add(Item::Coconut, n);
            say(GREEN, "You knock down " + std::to_string(n) + " " + itemName(Item::Coconut) + "(s).");
        } else if (r <= 46) {
            add(Item::Berries, rnd(2, 3));
            say(GREEN, "Bright " + itemName(Item::Berries) + ". Edible? Probably. Mostly.");
        } else if (r <= 58) {
            add(Item::Herb);
            say(GREEN, "You recognise a " + itemName(Item::Herb) + ". It might heal wounds.");
        } else if (r <= 78) {
            add(Item::Vine, rnd(1, 2));
            say(GREEN, "Strong " + itemName(Item::Vine) + " that can serve as rope.");
        } else if (r <= 90) {
            add(Item::Wood, rnd(1, 2));
            say(GREEN, "Dry fallen branches. " + itemName(Item::Wood) + " collected.");
        } else {
            snakeAttack();
        }
        gain(energy, -8);
        return true;
    }

    bool hunt() {
        say(RED, "You creep through the undergrowth, spear ready.");
        int r = rnd(1, 100);
        if (r <= 45) {
            int n = rnd(1, 2);
            add(Item::RawMeat, n);
            say(GREEN, "A clean throw! You got " + std::to_string(n) + " " + itemName(Item::RawMeat) + ".");
            note("Hunted wild game. The spear works.");
        } else if (r <= 60) {
            boarAttack();
        } else {
            say(GRAY, "The tracks go cold. Whatever it was, it got away.");
        }
        gain(energy, -12);
        return true;
    }

    bool chopWood() {
        int n = has(Item::Axe) ? 4 : 2;
        add(Item::Wood, n);
        playSound("hammer");
        say(GREEN, "Thwack! Thwack! You collect " + std::to_string(n) + " " + itemName(Item::Wood) + ".");
        gain(energy, -12);
        return true;
    }

    bool drinkStream() {
        playSound("splash");
        gain(thirst, 40);
        say(CYAN, "Cold, clear water. Best thing you have ever tasted.");
        if (chance(12)) hurt(8, "The water had something nasty in it.", "dirty water");
        gain(energy, -2);
        return true;
    }

    bool fillBottle() {
        if (!has(Item::Bottle)) return false;
        take(Item::Bottle);
        add(Item::CleanWater);
        say(CYAN, "You fill your bottle. Now you can carry water.");
        gain(energy, -2);
        return true;
    }

    // ---------------- действия: пещера ----------------
    bool exploreCave() {
        say(GRAY, "You light a branch from your fire and creep deeper into the dark.");
        int r = rnd(1, 100);
        if (!batteryFound && r <= 25) {
            batteryFound = true;
            add(Item::Battery);
            say(GREEN, "Something metallic glints in the dust. A " + itemName(Item::Battery) + "!");
            note("Found a battery in the cave. Could it power a radio?");
        } else if (r <= 55) {
            int n = rnd(1, 3);
            add(Item::Stone, n);
            say(GREEN, "You pry loose " + std::to_string(n) + " " + itemName(Item::Stone) + "(s).");
        } else if (r <= 70) {
            say(MAGENTA, "Bats explode out of the darkness, screeching around your head!");
            gain(sanity, -5);
        } else if (r <= 82) {
            hurt(12, "Part of the ceiling collapses on you!", "cave-in");
        } else if (r <= 92) {
            say(GRAY, "You find a skeleton, still clutching a bottle.");
            bottleNote();
            gain(sanity, -4);
        } else {
            say(GRAY, "Only darkness and the sound of dripping water.");
        }
        gain(energy, -10);
        return true;
    }

    bool dripWater() {
        gain(thirst, 15);
        gain(energy, -3);
        say(CYAN, "You lick water droplets off the cave wall. Slow, but it works.");
        return true;
    }

    // ---------------- действия: кораблекрушение ----------------
    bool searchWreck() {
        if (wreckLoot.empty()) {
            say(GRAY, "The wreck is stripped bare. Only rust and seagulls remain.");
            return false;
        }
        Item it = wreckLoot.back();
        wreckLoot.pop_back();
        say(ORANGE, "You squeeze through twisted beams into the flooded hold.");
        add(it);
        if (it == Item::RadioPart) {
            say(GREEN, "A " + itemName(it) + "! The ship's radio was torn apart, but this piece survived.");
            note("Found radio parts in the wreck.");
        } else {
            say(GREEN, "You find: " + itemName(it) + ".");
        }
        if (chance(20)) hurt(8, "Rusty metal slices your hand!", "infected wound");
        gain(energy, -10);
        return true;
    }

    // ---------------- действия: гора ----------------
    bool scanHorizon() {
        say(c256(250), "From the peak you see the whole island, and endless blue water.");
        say(GRAY, pick<std::string>({"No ships. No smoke. Nothing.",
                                     "A tiny dot far away... a cloud. Just a cloud.",
                                     "The wind whispers something. It sounds like home."}));
        gain(sanity, 5);
        gain(energy, -3);
        return true;
    }

    bool huntEggs() {
        say(YELLOW, "You climb the cliffs toward a nest.");
        if (chance(15)) {
            hurt(15, "Your foot slips! You tumble down the rocks.", "fall from the cliffs");
        } else if (chance(65)) {
            int n = rnd(1, 3);
            add(Item::Egg, n);
            say(GREEN, "You grab " + std::to_string(n) + " " + itemName(Item::Egg) + "(s). The mother bird is furious.");
        } else {
            say(GRAY, "The nest is empty.");
        }
        gain(energy, -12);
        return true;
    }

    bool transmit() {
        radioTries++;
        playSound("radio");
        say(CYAN, "You twist the dial. Static... static...");
        if (chance(35 + radioTries * 15)) {
            say(GREEN, "\"...copy... Marigold? Is that you? We're coming!\"");
            endGame(true, "RESCUED BY RADIO",
                    "A coast guard helicopter lands on the peak. You cry into the pilot's shoulder. "
                    "Cleverness beat the ocean.");
        } else {
            say(GRAY, "Nothing but noise. You try to stay hopeful.");
            gain(sanity, 2);
        }
        gain(energy, -3);
        return true;
    }

    bool collectRain() {
        take(Item::Bottle);
        add(Item::CleanWater);
        say(CYAN, "You hold the bottle up to the sky. Fresh rainwater!");
        gain(energy, -2);
        return true;
    }

    // ---------------- отдых ----------------
    bool rest() {
        int e = (phase == 3) ? 40 : 20;
        if (shelter) e += 10;
        if (campfire > 0) e += 5;
        gain(energy, e);
        if (shelter) gain(health, 5);
        say(BLUE, pick<std::string>({"You close your eyes and let the sound of the waves carry you off.",
                                     "You find some shade and doze off.",
                                     "You lean against a tree and breathe slowly."}));
        return true;
    }

    // ---------------- крафт ----------------
    bool craftMenu() {
        struct Recipe {
            std::string name;
            Needs needs;
            std::function<bool()> run;  // false = нельзя сейчас (ничего не тратим)
        };
        std::vector<Recipe> rs;
        rs.push_back(Recipe{"Campfire (burns 8 turns)", Needs{{Item::Wood, 3}, {Item::Stone, 1}},
            [this] {
                campfire = 8;
                say(ORANGE, "Sparks fly, smoke curls, and then: fire! Warmth and light.");
                return true;
            }});
        rs.push_back(Recipe{"Spear", Needs{{Item::Wood, 2}, {Item::Stone, 1}, {Item::Vine, 1}},
            [this] {
                add(Item::Spear);
                say(GREEN, "You lash a sharpened stone to a pole. A real weapon!");
                return true;
            }});
        rs.push_back(Recipe{"Axe", Needs{{Item::Wood, 1}, {Item::Stone, 2}, {Item::Vine, 1}},
            [this] {
                add(Item::Axe);
                say(GREEN, "A crude but sturdy stone axe.");
                return true;
            }});
        rs.push_back(Recipe{"Shelter", Needs{{Item::Wood, 6}, {Item::Vine, 3}, {Item::Cloth, 2}},
            [this] {
                if (shelter) { say(GRAY, "You already have a shelter."); return false; }
                shelter = true;
                say(GREEN, "A small hut of branches and sailcloth. Home sweet home.");
                return true;
            }});
        rs.push_back(Recipe{"Signal fire (Beach or Mountain only)", Needs{{Item::Wood, 5}, {Item::Cloth, 1}},
            [this] {
                if (loc != Loc::Beach && loc != Loc::Mountain) {
                    say(GRAY, "A signal fire must be built where ships can see it: the beach or the mountain.");
                    return false;
                }
                signalTurns = 10;
                playSound("ship");
                say(RED, "Black smoke climbs into the sky. Let them see it!");
                note("Lit a signal fire.");
                return true;
            }});
        rs.push_back(Recipe{"Radio (Needs 3 parts + battery)", Needs{{Item::RadioPart, 3}, {Item::Battery, 1}},
            [this] {
                if (radioBuilt) { say(GRAY, "The radio is already built."); return false; }
                radioBuilt = true;
                say(CYAN, "You solder the last wire with a hot nail. The radio crackles to life!");
                say(YELLOW, "...but the signal is weak here. You need height. The mountain peak?");
                note("Built a radio! It needs a high place to work.");
                return true;
            }});

        std::cout << BOLD << "What do you want to make?" << RESET << "\n";
        for (size_t i = 0; i < rs.size(); i++)
            std::cout << BOLD << " " << (i + 1) << ") " << RESET << rs[i].name << "  [" << needsText(rs[i].needs) << "]\n";
        std::cout << BOLD << " 0) " << RESET << "Cancel\n";
        int ch = askChoice(static_cast<int>(rs.size()), true);
        if (ch == 0) return false;
        Recipe& r = rs[ch - 1];
        if (!canAfford(r.needs)) {
            say(RED, "You don't have enough materials.");
            return false;
        }
        if (!r.run()) return false;
        pay(r.needs);
        itemsCrafted++;
        gain(energy, -8);
        playSound("hammer");
        return true;
    }

    // ---------------- еда и питьё ----------------
    static bool isUsable(Item i) {
        switch (i) {
            case Item::Coconut: case Item::Berries: case Item::Herb: case Item::Egg:
            case Item::RawFish: case Item::CookedFish: case Item::RawMeat:
            case Item::CookedMeat: case Item::CleanWater:
                return true;
            default:
                return false;
        }
    }

    bool useMenu() {
        std::vector<Item> usable;
        for (const auto& [it, q] : inv) if (isUsable(it)) usable.push_back(it);
        if (usable.empty()) {
            say(GRAY, "You have nothing to eat or drink.");
            return false;
        }
        for (size_t i = 0; i < usable.size(); i++)
            std::cout << BOLD << " " << (i + 1) << ") " << RESET << itemName(usable[i])
                      << " x" << inv[usable[i]] << "\n";
        std::cout << BOLD << " 0) " << RESET << "Cancel\n";
        int ch = askChoice(static_cast<int>(usable.size()), true);
        if (ch == 0) return false;
        consume(usable[ch - 1]);
        clampStats();
        return false;  // бесплатное действие
    }

    void consume(Item it) {
        take(it);
        playSound("eat");
        switch (it) {
            case Item::Coconut:
                gain(hunger, 12); gain(thirst, 10); mealsEaten++;
                say(YELLOW, "Sweet coconut water and white flesh.");
                break;
            case Item::Berries:
                gain(hunger, 10); mealsEaten++;
                if (chance(25)) {
                    hurt(15, "The berries were poisonous! Your stomach twists.", "poisonous berries");
                    gain(thirst, -10);
                } else say(MAGENTA, "Sour, but it fills your stomach.");
                break;
            case Item::Herb:
                gain(health, 20);
                say(GREEN, "Bitter, but you feel your wounds getting better.");
                break;
            case Item::Egg:
                gain(hunger, 18); mealsEaten++;
                say(YELLOW, "Raw egg. Not pleasant, but it's protein.");
                break;
            case Item::RawFish:
            case Item::RawMeat:
                gain(hunger, it == Item::RawFish ? 18 : 25); mealsEaten++;
                if (chance(35)) hurt(10, "Raw food. Your stomach rebels.", "food poisoning");
                else say(RED, "Raw, but it stays down. Barely.");
                break;
            case Item::CookedFish:
                gain(hunger, 35); mealsEaten++;
                say(ORANGE, "Warm, flaky, salty fish. A feast!");
                break;
            case Item::CookedMeat:
                gain(hunger, 45); mealsEaten++;
                say(ORANGE, "Juicy roasted meat. You almost cry.");
                break;
            case Item::CleanWater:
                gain(thirst, 40);
                add(Item::Bottle);
                say(CYAN, "You drink deeply. The empty bottle goes back in your bag.");
                break;
            default:
                break;
        }
    }

    bool cookFood() {
        int f = has(Item::RawFish) ? inv[Item::RawFish] : 0;
        int m = has(Item::RawMeat) ? inv[Item::RawMeat] : 0;
        if (f) { take(Item::RawFish, f); add(Item::CookedFish, f); }
        if (m) { take(Item::RawMeat, m); add(Item::CookedMeat, m); }
        say(ORANGE, "Everything sizzles over the flames. The smell is incredible.");
        return false;  // бесплатно
    }

    // ---------------- путешествия ----------------
    bool travel() {
        std::vector<Loc> opts;
        for (Loc l : {Loc::Beach, Loc::Jungle, Loc::Cave, Loc::Wreck, Loc::Mountain})
            if (l != loc) opts.push_back(l);
        std::cout << BOLD << "Where to?" << RESET << "\n";
        for (size_t i = 0; i < opts.size(); i++)
            std::cout << BOLD << " " << (i + 1) << ") " << RESET << locColor(opts[i]) << locName(opts[i]) << RESET
                      << (opts[i] == Loc::Mountain ? GRAY : "") << (opts[i] == Loc::Mountain ? "  (exhausting climb)" : "")
                      << RESET << "\n";
        std::cout << BOLD << " 0) " << RESET << "Stay here\n";
        int ch = askChoice(static_cast<int>(opts.size()), true);
        if (ch == 0) return false;
        Loc dest = opts[ch - 1];
        int cost = (dest == Loc::Mountain) ? 15 : 10;
        if (energy < cost) {
            say(RED, "You are too exhausted for that journey. Rest first.");
            return false;
        }
        loc = dest;
        gain(energy, -cost);
        say(locColor(dest), arrivalText(dest));
        if ((dest == Loc::Jungle || dest == Loc::Mountain) && chance(10))
            hurt(8, "You twist your ankle on the rough ground.", "twisted ankle");
        return true;
    }

    std::string arrivalText(Loc l) const {
        switch (l) {
            case Loc::Beach:    return "Warm sand under your feet. The sea breathes in and out.";
            case Loc::Jungle:   return "The air turns thick and humid. Birds scream overhead.";
            case Loc::Cave:     return "Cool darkness swallows you. Your footsteps echo.";
            case Loc::Wreck:    return "The Marigold's broken ribs rise out of the surf like a skeleton.";
            case Loc::Mountain: return "The wind is sharp up here. The whole island lies beneath you.";
        }
        return "";
    }

    // ---------------- дневник / выход ----------------
    bool readDiary() {
        if (diary.empty()) {
            say(GRAY, "Your diary is still empty. The first day is not over yet.");
        } else {
            std::cout << BOLD << "--- Diary ---" << RESET << "\n";
            for (const auto& d : diary) std::cout << GRAY << glitch(d) << RESET << "\n";
        }
        return false;
    }

    bool quitGame() {
        std::cout << "Really quit?\n 1) No\n 2) Yes\n";
        if (askChoice(2) == 2) {
            lastCause = "gave up";
            endGame(false, "YOU GAVE UP", "Sometimes the island wins.");
            return true;
        }
        return false;
    }

    // ---------------- случайные события ----------------
    void bottleNote() {
        static const std::vector<std::string> notes = {
            "\"If you read this, I did not make it. Follow the stream inland. Fresh water.\"",
            "\"Sea water makes you mad. Drink only from the stream, the cave, or the rain.\"",
            "\"The wreck still has radio parts. I found three. Never found a battery.\"",
            "\"The cave... something was breathing down there. Maybe wind. Maybe.\"",
            "\"A radio needs height. The mountain peak is the only place the signal carries.\"",
            "\"Day 40. Mr. Coco says it is time to build the raft. I think he is right.\""};
        say(CYAN, "A corked bottle with a scrap of paper inside!");
        if (notesFound < static_cast<int>(notes.size())) {
            say(YELLOW, notes[notesFound]);
            notesFound++;
            gain(sanity, 5);
            note("Found a note from another survivor.");
        } else {
            say(GRAY, "The paper is blank. Whoever wrote it ran out of words.");
        }
    }

    void snakeAttack() {
        playSound("hiss");
        say(RED, "SSSSSSS... a snake coils in the grass!");
        if (has(Item::Spear)) {
            add(Item::RawMeat);
            say(GREEN, "You pin it with your spear. Dinner: " + itemName(Item::RawMeat) + ".");
        } else {
            hurt(15, "The snake bites your leg!", "snake bite");
        }
    }

    void boarAttack() {
        say(RED, "A wild boar crashes out of the bushes, tusks lowered!");
        if (has(Item::Spear)) {
            add(Item::RawMeat, 2);
            hurt(8, "You kill it, but not before it gores you.", "boar");
            say(GREEN, "Plenty of meat, at least.");
        } else {
            hurt(12, "You dive aside, but it catches you.", "boar");
        }
    }

    void nightScare() {
        if (campfire > 0) {
            say(ORANGE, "Something rustles in the dark, but the fire keeps it away.");
            gain(sanity, -1);
            return;
        }
        say(MAGENTA, pick<std::string>({"Two red eyes glow in the bushes. They blink. They are gone.",
                                        "Footsteps circle your camp. When you look, there is nothing.",
                                        "A whisper from the trees says your name.",
                                        "Far away, something howls. It does not sound like an animal."}));
        gain(sanity, -4);
    }

    void cocoVoice() {
        say(YELLOW, pick<std::string>({"\"Psst. Over here,\" whispers a coconut. You are not sure it was the coconut.",
                                       "\"You're doing great,\" says Mr. Coco. You really should not be talking to coconuts.",
                                       "\"They aren't coming, you know,\" the coconut sighs."}));
        gain(sanity, -3);
    }

    void coconutFalls() {
        add(Item::Coconut);
        say(GREEN, "A " + itemName(Item::Coconut) + " thuds into the sand right next to you. Lucky!");
    }

    void driftwood() {
        add(Item::Wood, 2);
        say(GREEN, "A wave drops two pieces of " + itemName(Item::Wood) + " at your feet.");
    }

    void suddenStorm() {
        weather = Weather::Storm;
        playSound("thunder");
        say(MAGENTA, "The sky turns black in minutes. A storm is coming!");
    }

    void randomEvent() {
        if (!chance(phase == 3 ? 40 : 30)) return;
        std::vector<std::function<void()>> pool;
        bool outdoors = (loc == Loc::Beach || loc == Loc::Jungle);
        if (outdoors) {
            pool.push_back([this] { snakeAttack(); });
            pool.push_back([this] { coconutFalls(); });
        }
        if (loc == Loc::Beach) {
            pool.push_back([this] { bottleNote(); });
            pool.push_back([this] { driftwood(); });
        }
        if (phase == 3) {
            pool.push_back([this] { nightScare(); });
            pool.push_back([this] { nightScare(); });
        }
        if (phase == 3 && loc == Loc::Jungle) pool.push_back([this] { boarAttack(); });
        if (sanity < 45) {
            pool.push_back([this] { cocoVoice(); });
            pool.push_back([this] { cocoVoice(); });
        }
        if (weather != Weather::Storm && loc != Loc::Cave) pool.push_back([this] { suddenStorm(); });
        if (pool.empty()) return;
        std::cout << "\n";
        pick(pool)();
    }

    void shipCheck() {
        if (phase == 3 || loc == Loc::Cave || !chance(8)) return;
        std::cout << "\n";
        playSound("ship");
        say(c256(250), "A ship! A tiny silhouette on the horizon!");
        if (signalTurns > 0 && chance(85)) {
            say(RED, "Your signal fire burns bright. The ship turns toward the island!");
            endGame(true, "RESCUED BY A SHIP",
                    "A crew in orange jackets pulls you aboard. You've never loved strangers so much.");
        } else if (signalTurns > 0) {
            say(GRAY, "The smoke is thin. The ship does not turn. It slips away.");
            gain(sanity, -8);
        } else {
            say(GRAY, "It doesn't see you. If only you had a signal fire burning...");
            gain(sanity, -8);
            note("A ship passed by. I had no signal. Never again.");
        }
    }

    // ---------------- конец хода ----------------
    void endTurn() {
        hunger -= 5;
        thirst -= 7;
        energy -= 4;
        if (weather == Weather::Heat && loc != Loc::Cave) thirst -= 5;
        if (weather == Weather::Rain && loc != Loc::Cave) energy -= 2;

        std::cout << "\n";
        if (weather == Weather::Storm && loc != Loc::Cave && !shelter) {
            playSound("thunder");
            hurt(6, "The storm lashes you with wind and rain.", "exposure in a storm");
        }

        if (phase == 3 && campfire <= 0 && !shelter && loc != Loc::Cave) sanity -= 3;
        if (campfire > 0) sanity += 1;
        if (hunger < 25) sanity -= 2;
        if (thirst < 25) sanity -= 2;

        if (hunger <= 0) hurt(10, "You are starving.", "starvation");
        else if (hunger < 25) say(YELLOW, "Your stomach cramps with hunger.");
        if (thirst <= 0) hurt(15, "Your throat is cracked and dry.", "dehydration");
        else if (thirst < 25) say(CYAN, "Your mouth is like sandpaper. You need water.");
        if (energy <= 0) {
            hurt(8, "Exhaustion drops you to your knees.", "exhaustion");
            sanity -= 5;
            energy = 15;
        } else if (energy < 20) say(GREEN, "Your legs shake. You should rest.");

        if (hunger > 50 && thirst > 50 && energy > 30 && health < 100) health += 3;

        clampStats();
        randomEvent();
        if (over) return;
        shipCheck();
        if (over) return;
        clampStats();

        if (health <= 0) {
            endGame(false, "YOU DIED", "Cause of death: " + lastCause + ". The island keeps another soul.");
            return;
        }
        if (sanity <= 0) {
            endGame(false, "LOST TO MADNESS",
                    "You stopped fighting the voices. You and Mr. Coco live on the beach, forever, "
                    "waiting for a ship that is not coming.");
            return;
        }
        advanceTime();
    }

    void advanceTime() {
        if (campfire > 0) campfire--;
        if (signalTurns > 0) signalTurns--;
        phase++;
        if (phase < 4) return;

        phase = 0;
        // запись в дневник
        std::string entry = "Day " + std::to_string(day) + ": ";
        if (todayEvents.empty()) {
            entry += "Nothing worth writing. Still alive.";
        } else {
            size_t start = todayEvents.size() > 2 ? todayEvents.size() - 2 : 0;
            for (size_t i = start; i < todayEvents.size(); i++) entry += todayEvents[i] + " ";
        }
        diary.push_back(entry);
        todayEvents.clear();

        day++;
        if (day > GOAL_DAYS) {
            endGame(true, "SURVIVOR",
                    "Twenty days. You built a life from sand, driftwood and stubbornness. "
                    "Maybe the island is not so bad. Maybe it's yours now.");
            return;
        }
        newWeather();
    }

    void newWeather() {
        int r = rnd(1, 100);
        weather = r <= 45 ? Weather::Clear : r <= 70 ? Weather::Rain : r <= 90 ? Weather::Heat : Weather::Storm;
        switch (weather) {
            case Weather::Clear: say(YELLOW, "A new day dawns. The sky is clear."); break;
            case Weather::Rain:  say(CYAN, "Grey clouds. It starts to rain."); break;
            case Weather::Heat:  say(ORANGE, "The sun is merciless today. The air shimmers."); break;
            case Weather::Storm: say(MAGENTA, "Thunder rolls across the island. A storm!"); playSound("thunder"); break;
        }
    }
};

// ----------------------------------------------------------------------------
// 6. ТОЧКА ВХОДА
// ----------------------------------------------------------------------------
int main() {
    Game game;
    game.run();
    return 0;
}
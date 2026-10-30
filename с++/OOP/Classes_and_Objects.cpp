#include <iostream>
#include <string>

using namespace std;

class Building {

private:
    int cost;

public:
    int get_cost() {
        return cost;
    }

    int year;
    string type;

    void get_info() {
        cout << "Type: " << type << endl << "Year: " << year <<endl;
    }
};

int main() {

    Building school;
    school.type = "Школа";
    school.year = 2000;
    school.get_info();    

    Building house;
    house.type = "Дом";
    house.year = 2010;
    house.get_info(); 

    return 0;
}

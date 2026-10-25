#include <iostream>
#include <string> //  Вызивает новый тип переменной - строку

using namespace std;

int main() {

    char word[] = "Hi!"; // {'h', 'i', '!'};
    for(int i = 0; i < 3; i++) {
        cout << word[i];
    }

    
    string words = "hello world!";
    words[0] = 'W';
    cout << "\n" << words <<endl;


    cin >> words;
    cout << "New: " << words;

    

    return 0;
}
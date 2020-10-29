/* Главная программа транслятора */
#include "defs.hpp"
#include "scaner.hpp"

const string input_file_name = "input3.txt";

int main() {
    TScaner *scan = new TScaner(input_file_name);
    int type;
    do {
        TypeLex l;
        type = scan->scaner(l);
        cout << l << " - type " << type << endl;
    } while (type != TEnd);

    system("pause");
    return 0;

}
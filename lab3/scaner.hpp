/* Заголовки функций сканера */
#ifndef __SCANER
#define __SCANER
#include "defs.hpp"

class TScaner {
private:
    TypeMod t;  // исходный текст
    int uk;     // указатель текущей позиции в исходном тексте
    map <TypeLex, int> keyword;     // ассоциативный массив ключевое слово -> его тип

public:
    void putUK (int i);     // установить значение указателя
    int getUK ();       // получить значение указателя
    void printError(string, string);    // выдать сообщение об ошибке
    int scaner (TypeLex &); // сканирующая функция
    void getData(string);   // получение данных из файла

    TScaner(string);
    ~TScaner();
};
#endif
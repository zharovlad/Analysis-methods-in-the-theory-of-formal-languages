/* Реализация функций сканера */
#include "defs.hpp"
#include "scaner.hpp"
#include <fstream>

const string errorConst = "Incorrect constant";
const string errorFile = "Input file not found";
const string errorSymbol = "Incorrect symbol";

TScaner::TScaner(string FileName) {
    getData(FileName);
    putUK(0);
    keyword["for"] = Tfor;
    keyword["int"] = Tint; 
    keyword["short"] = Tshort;
    keyword["long"] = Tlong;
    keyword["double"] = Tdouble; 
    keyword["struct"] = Tstruct;
}

void TScaner::putUK(int i) { uk=i; } 

int TScaner::getUK() { return uk; }

void TScaner::printError(string err, string a = "") {
    if (a.length() == 0) {
        cout << "Error : " << err << endl;
        exit(0);
    }
    else
        cout << "Error : " << err << " '" << a << "'" << endl;
    
}

void TScaner::getData(string FileName) {
    // ввод данных из текстового файла
    ifstream in (FileName);
    if (!in.is_open()) { 
        printError(errorFile); 
        exit(1);
    }
    char symbol;
    while (in.get(symbol)) {
        t += symbol;
    }
    in.close();
}

int TScaner::scaner(TypeLex &l) {
    l.clear();          // очистили поле лексемы

    start:              // метка игнорируемых элементов
    while ((t[uk] == ' ') || (t[uk] == '\n') || (t[uk] == '\t'))
         uk++;

    if ((t[uk] == '/') && (t[uk + 1] == '/')) { // комментарий начинается с "//" и до конца строки
            uk += 2;
            while (t[uk] != '\n') 
                uk++;
            goto start;
    }

    if ((t[uk] == '/') && (t[uk + 1] == '*')) { // комментарий начинается с "/*" и до следующей комбинации "*/"
            uk += 2;
            while ((t[uk] != '*') && (t[uk + 1] != '/')) 
                uk++;
            goto start;
    }

    if (uk == t.size()) { // конец кода
        l = "#";
        return TEnd; 
    }

    // константы

    if ((t[uk] <= '9') && (t[uk] >= '0')) {
        l += t[uk++];
        while ((t[uk] <= '9') && (t[uk] >= '0'))
            if (l.size() < MAX_LEX) // проверка на длину лексемы, если длинная - обрезать
                l += t[uk++];
            else
                uk++;

        if (t[uk] == '.') {
            l += t[uk++];
            goto _double;    // продолжение константы для double
        }

        if((t[uk] == 'E') || (t[uk] == 'e')) {
            l += t[uk++];
            goto _exp;    // продолжение константы экспоненциальной
        }

        return TConstInt;   // константа целая
    }

    // начинается идентификатор
    else if ((t[uk] >= 'a') && (t[uk] <= 'z') || (t[uk] >= 'A') && (t[uk] <= 'Z') || t[uk] == '_') { 
        l += t[uk++];
        while ((t[uk] <= '9') && (t[uk] >= '0') || (t[uk] >= 'a') && (t[uk] <= 'z') || (t[uk] >= 'A') && (t[uk] <= 'Z') || t[uk] == '_')
            if (l.size() < MAX_LEX)
                l += t[uk++];
            else
                uk++;
        // проверка на ключевое слово
        for (auto word : keyword)
            if (word.first == l)
                return word.second;
        return Tident;
    }
    // точка
    else if (t[uk] == '.') { 
        l += t[uk++];
        if ((t[uk] <= '9') && (t[uk] >= '0')) {
            l += t[uk++]; 
            goto _double; 
        }
        return TDot;
    }
    // запятая
    else if (t[uk] == ',') {
        l += t[uk++];
        return TCom;
    }
    // точка с запятой
    else if (t[uk] == ';') {
        l += t[uk++];
        return TDC;
    }
    // (
    else if (t[uk] == '(') {
        l += t[uk++];
        return TLB;
    }
    // )
    else if (t[uk] == ')') {
        l += t[uk++];
        return TRB;
    }
    // {
    else if (t[uk] == '{') {
        l += t[uk++];
        return TFLB;
    }
    // }
    else if (t[uk] == '}') {
        l += t[uk++];
        return TFRB;
    }
    // <, <=, <<
    else if (t[uk] == '<') {
        uk++;
        if (t[uk] == '=') {
            uk++;
            return TLE;
        }
        if (t[uk] == '<') {
            uk++;
            return TBLS;
        }
        return TLT;
    }
    // >, >=, >
    else if (t[uk] == '>') {
        uk++;
        if (t[uk] == '=') {
            uk++;
            return TGE;
        }
        if (t[uk] == '>') {
            uk++;
            return TBRS;
        }
        return TGT;
    }
    // =, ==
    else if (t[uk] == '=') {
        uk++;
        if (t[uk] == '=') {
            uk++;
            return TEQ;
        }
        return TSave;
    }
    // !=
    else if (t[uk] == '!' && t[uk + 1] == '=') {
        uk += 2;
        return TNEQ;
    }
    // +
    else if (t[uk] == '+') {
        l += t[uk++];
        return TPlus;
    }
    // -
    else if (t[uk] == '-') {
        l += t[uk++];
        return TMinus;
    }
    // *
    else if (t[uk] == '*') {
        l += t[uk++];
        return TMult;
    }
    // /
    else if (t[uk] == '/') {
        l += t[uk++];
        return TDiv;
    }
    // |
    else if (t[uk] == '|') {
        l += t[uk++];
        return TBOR;
    }
    // &
    else if (t[uk] == '&') {
        l += t[uk++];
        return TBAnd;
    }
    // }
    else if (t[uk] == '^') {
        l += t[uk++];
        return TBExOr;
    }
    // символ - ошибка
    else {
        l += t[uk++];
        printError(errorSymbol, l);
        return TErr;
    }

    _double:    // продолжение константы после точки
    while ((t[uk] <= '9') && (t[uk] >= '0'))
        if (l.size() < MAX_LEX)
            l += t[uk++];
        else
            uk++;

        if ((t[uk] == 'e') || (t[uk] == 'E')) {
            l += t[uk++];
            goto _exp;
        }
        return TConstDouble;
    
    _exp:       // продолжение константы после е
    if ((t[uk] == '+') || (t[uk] == '-')) {
        l += t[uk++];

        if ((t[uk] <= '9') && (t[uk] >= '0')) {
            if (l.size() < MAX_LEX)
                l += t[uk++];
            else
                uk++;
            goto continueExpConst;
        }

        else {
            l += t[uk++];
            printError(errorConst, l);
            return TErr;
        }
    }

    continueExpConst:   // продолжение порядка числовой константы
    while ((t[uk] <= '9') && (t[uk] >= '0')) {
        if (l.size() < MAX_LEX)
            l += t[uk++];
        else
            uk++;
    }
    return TConstExp;
}
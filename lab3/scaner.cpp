/* Реализация функций сканера */
#include "defs.hpp"
#include "scaner.hpp"
#include <fstream>

TScaner::TScaner(string FileName) {
    getData(FileName);
    putUK(0);
    keyword["for"] = Tfor;
    keyword["int"] = Tint; 
    keyword["short"] = Tshort;
    keyword["long"] = Tlong;
    keyword["double"] = Tdouble; 
    keyword["struct"] = Tstruct;
    keyword["main"] = Tmain;
    numberString = 1;
}

void TScaner::putString(int s) { numberString = s; }

int TScaner::getStringNumber() { return numberString; }

void TScaner::putUK(int i) { uk=i; } 

int TScaner::getUK() { return uk; }

void TScaner::printError(const string type, string err, string a) {
    cout << type << " error. " << endl;
    cout << "String #" << numberString << endl;

    if (type == "Lexical") {

        if (a.empty()) {
            cout << err << endl;
        }
        else {
            cout << err << " '" << a << "'" << endl << endl;
        }
        return;
    }

    if (type == "Syntax") {
        cout << "Lexem - " << err << endl;
        cout << "Expexted lexem - " << a << endl;
        exit(2);	// при ошибке - сразу выходим, т.к. синтаксический анализатор работает до первой ошибки
    }

    if (type == "Semant") {
        cout << "Identifier " << a << " " << err << endl;
        exit(3);
    }

    if (type == "Wrong type") {
        cout << "Type must be int" << endl;
    }
}

void TScaner::getData(string FileName) {
    // ввод данных из текстового файла
    ifstream in (FileName);
    if (!in.is_open()) { 
        printError("Lexical", errorFile);
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
    while ((t[uk] == ' ') || (t[uk] == '\n') || (t[uk] == '\t')) {
        if (t[uk] == '\n') 
            numberString += 1;
        uk++;
    }

    if ((t[uk] == '/') && (t[uk + 1] == '/')) { // комментарий начинается с "//" и до конца строки
            uk += 2;
            if (uk == t.size()) 
                goto _end;
            while (t[uk] != '\n') 
                if (++uk == t.size()) 
                    goto _end;
            numberString += 1;
            goto start;
    }

    if ((t[uk] == '/') && (t[uk + 1] == '*')) { // комментарий начинается с "/*" и до следующей комбинации "*/"
            uk += 2;
            if (uk == t.size()) {
                printError("Lexical", errorComment);
                l = "/**/";
                return TErr;
            }
            while ((t[uk] != '*') && (t[uk + 1] != '/')) {
                if (t[uk] == '\n') 
                    numberString += 1;
                if (++uk == t.size()) {
                    printError("Lexical", errorComment);
                    l = "/**/";
                    return TErr;
                }
            }
            uk += 2;
            goto start;
    }

    if (uk == t.size()) { // конец кода
        _end:
        l = "#";
        return TEnd; 
    }

    // константы

    bool isCut = false;     // проверка - обрезанная константа или нет

    if ((t[uk] <= '9') && (t[uk] >= '0')) 
    {
        l += t[uk++];
        while ((t[uk] <= '9') && (t[uk] >= '0'))
            if (l.size() < MAX_LEX) // проверка на длину лексемы, если длинная - обрезать
                l += t[uk++];
            else {
                uk++;
                isCut = true;
            }
                

        if (t[uk] == '.') {
            l += t[uk++];
            goto _double;    // продолжение константы для double
        }

        if((t[uk] == 'E') || (t[uk] == 'e')) {
            l += t[uk++];
            goto _exp;    // продолжение константы экспоненциальной
        }

        if (isCut) {
            printError("Lexical", errorTruncated, l);
            return TErr;
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
        l += t[uk++];
        if (t[uk] == '=') {
            l += t[uk++];
            return TLE;
        }
        if (t[uk] == '<') {
            l += t[uk++];
            return TBLS;
        }
        return TLT;
    }
    // >, >=, >
    else if (t[uk] == '>') {
        l += t[uk++];
        if (t[uk] == '=') {
            l += t[uk++];
            return TGE;
        }
        if (t[uk] == '>') {
            l += t[uk++];
            return TBRS;
        }
        return TGT;
    }
    // =, ==
    else if (t[uk] == '=') {
        l += t[uk++];
        if (t[uk] == '=') {
            l += t[uk++];
            return TEQ;
        }
        return TSave;
    }
    // !=
    else if (t[uk] == '!' && t[uk + 1] == '=') {
        l = "!=";
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
    // ~
    else if (t[uk] == '~') {
        l += t[uk++];
        return TBNot;
    }
    // символ - ошибка
    else {
        l += t[uk++];
        printError("Lexical", errorSymbol, l);
        return TErr;
    }

    _double:    // продолжение константы после точки
    while ((t[uk] <= '9') && (t[uk] >= '0'))
        if (l.size() < MAX_LEX)
            l += t[uk++];
        else {
            uk++;
            isCut = true;
        }

        if ((t[uk] == 'e') || (t[uk] == 'E')) {
            if (l.size() < MAX_LEX)
                l += t[uk++];
            else {
                uk++;
                isCut = true;
            }
            goto _exp;
        }
        if (isCut) {
            printError("Lexical", errorTruncated, l);
            return TErr;
        }
        return TConstDouble;
    
    _exp:       // продолжение константы после е
    if ((t[uk] == '+') || (t[uk] == '-')) {
        if (l.size() < MAX_LEX)
            l += t[uk++];
        else {
                uk++;
                isCut = true;
        }
        if ((t[uk] <= '9') && (t[uk] >= '0')) {
            if (l.size() < MAX_LEX)
                l += t[uk++];
            else {
                uk++;
                isCut = true;
            }
            goto continueExpConst;
        }

        else {
            l += t[uk++];
            printError("Lexical", errorConst, l);
            return TErr;
        }
    }

    continueExpConst:   // продолжение порядка числовой константы
    while ((t[uk] <= '9') && (t[uk] >= '0')) {
        if (l.size() < MAX_LEX)
            l += t[uk++];
        else {
                uk++;
                isCut = true;
        }
    }
    if (isCut) {
            printError("Lexical", errorTruncated, l);
            return TErr;
    }
    return TConstExp;
}
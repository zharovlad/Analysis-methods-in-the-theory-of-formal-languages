#ifndef __DIAGRAM
#define __DIAGRAM
#include "defs.hpp"
#include "scaner.hpp"

class TDiagram {
private:
	TScaner* scan;
	void GlobalVar();				// глобальная переменная
	void Identifiers();				// идентификаторы
	void Data();					// данные
	void Var();						// переменная
	void BetwiseOR();				// побитовое или
	void BetwiseXOR();				// побитовое исключающее или
	void BetwiseAND();				// побитовое и
	void Equal();					// равно или не равно
	void Compare();					// сравнение
	void Shift();					// сдвиг
	void Addition();				// сложение
	void Multiplication();			// умножение
	void BetwiseNOT();				// побитовое не
	void Elementary();				// элементарное выражение
	void CompositeStatement();		// составной оператор
	void Statement();				// оператор
	void ActionsAndConditions();	// действия и условия для цикла for
	void PrintError(int, TypeLex, TypeLex);			// печать ошибок
	void Assignment();

public:
	TDiagram(const string);
	~TDiagram();
	void Prog(); // аксиома
};

#endif
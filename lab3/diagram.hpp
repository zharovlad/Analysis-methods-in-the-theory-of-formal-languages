#ifndef __DIAGRAM
#define __DIAGRAM
#include "defs.hpp"
#include "semant.hpp"
#include "scaner.hpp"

class TDiagram {
private:
	TScaner* scan;
	Tree* root;

	void GlobalVar();				// глобальная переменная
	void Identifiers();				// идентификаторы
	void Data();					// данные
	void Var(int, Tree*);					// переменная
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

	void PrintSemantTree();
	TDiagram(const string);
	~TDiagram();
	void Prog(); // аксиома
};

#endif
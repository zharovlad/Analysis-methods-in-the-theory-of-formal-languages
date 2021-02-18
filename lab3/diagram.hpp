#ifndef __DIAGRAM
#define __DIAGRAM
#include "defs.hpp"
#include "semant.hpp"
#include "scaner.hpp"
#include <utility>
#include <sstream>

class TDiagram {
private:
	TScaner* scan;
	Tree* root;

	void GlobalVar();				// глобальная переменная
	vector<string> Identifiers();				// идентификаторы
	void Data(bool needAllocation=true);					// данные
	void Var(int, Tree*, bool);					// переменная
	pair<DataType, DataValue> BetwiseOR();				// побитовое или
	pair<DataType, DataValue> BetwiseXOR();				// побитовое исключающее или
	pair<DataType, DataValue> BetwiseAND();				// побитовое и
	pair<DataType, DataValue> Equal();					// равно или не равно
	pair<DataType, DataValue> Compare();					// сравнение
	pair<DataType, DataValue> Shift();					// сдвиг
	pair<DataType, DataValue> Addition();				// сложение
	pair<DataType, DataValue> Multiplication();			// умножение
	pair<DataType, DataValue> BetwiseNOT();				// побитовое не
	pair<DataType, DataValue> Elementary();				// элементарное выражение
	void CompositeStatement();		// составной оператор
	void Statement();				// оператор
	void ActionsAndConditions();	// действия и условия для цикла for
	pair<DataType, DataValue> Assignment();

public:

	void PrintSemantTree(string);
	TDiagram(const string);
	~TDiagram();
	void Prog(); // аксиома
};

#endif
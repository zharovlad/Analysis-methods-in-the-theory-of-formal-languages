#include "defs.hpp"
#include "scaner.hpp"
#include "diagram.hpp"


TDiagram::TDiagram(const string fileName) {
	scan = new TScaner(fileName);
}


void TDiagram::PrintError(int stringNumber) {
	cout << endl << "String #" << stringNumber << endl;
	cout << "Error : unexpected lexem." << endl;
	exit(2);	// при ошибке - сразу выходим, т.к. наш транслятор работает до первой ошибки
}


void TDiagram::Prog() {					// аксиома
	int uk = scan->getUK();				// проверка условия цикла или if требует дополнительного сканирования
	int str = scan->getStringNumber();	// 2 переменные, чтобы можно было вернуть указатель и номер строки до сканирования
	TypeLex lex;						// локальная лексема
	int lexType = scan->scaner(lex);	// тип лексемы

	if (lexType == TEnd) // пустая программа
		return;
	
	//
	// иначе сначала глобальные переменные, потом главная функция
	//

	scan->putUK(uk);
	scan->putString(str);

	GlobalVar();

	lexType = scan->scaner(lex);
	if (lexType != Tint) {
		PrintError(scan->getStringNumber());
	}

	lexType = scan->scaner(lex);
	if (lexType != Tmain) {
		PrintError(scan->getStringNumber());
	}

	lexType = scan->scaner(lex);
	if (lexType != TLB) {	// (
		PrintError(scan->getStringNumber());
	}

	lexType = scan->scaner(lex);
	if (lexType != TRB) {	// )
		PrintError(scan->getStringNumber());
	}

	CompositeStatement();
}


void TDiagram::GlobalVar() {	// СД глобальных перемен 
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);
	while (lexType >= Tint || lexType <= Tstruct) {

		if (lexType == Tstruct) {	// ветвь описания структуры

			lexType = scan->scaner(lex);
			if (lexType != Tident) {
				PrintError(scan->getStringNumber());
			}
			
			lexType = scan->scaner(lex);
			if (lexType != TFLB) {	// {
				PrintError(scan->getStringNumber());
			}

			uk = scan->getUK();
			str = scan->getStringNumber();
			lexType = scan->scaner(lex);
			while (lexType == Tident) {
				scan->putUK(uk);
				scan->putString(str);
				Data();
				uk = scan->getUK();
				str = scan->getStringNumber();
				lexType = scan->scaner(lex);
			}
			scan->putUK(uk);
			scan->putString(str);

			lexType = scan->scaner(lex);
			if (lexType != TFRB) {	// }
				PrintError(scan->getStringNumber());
			}

			lexType = scan->scaner(lex);
			if (lexType != TDC) {	// ;
				PrintError(scan->getStringNumber());
			}
			uk = scan->getUK();
			str = scan->getStringNumber();
			lexType = scan->scaner(lex);
		}
		else {
			if (lexType == Tint) {
				lexType = scan->scaner(lex);
				// если int main - возвращаем указатель на позицию перед int и выходим из цикла и GlobalVar, т.к. мы уже в главной функции 
				// иначе возвращаем указатель на позицию перед int и вызываем Data - описывается глобальная переменная типа int
				scan->putUK(uk);
				scan->putString(str);
				if (lexType == Tmain) {
					break;
				}
			}
			Data();
			uk = scan->getUK();
			str = scan->getStringNumber();
			lexType = scan->scaner(lex);
		}
	}
	scan->putUK(uk);
	scan->putString(str);
}


void TDiagram::Identifiers() {	// СД идентификаторов
	int uk;
	int str;
	TypeLex lex;
	int lexType;
	do {
		lexType = scan->scaner(lex);
		if (lexType != Tident) {
			PrintError(scan->getStringNumber());
		}
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	} while (lexType == TDot);	// случай a.b.c.d.e...
	scan->putUK(uk);
	scan->putString(str);
}


void TDiagram::Data() {	// СД данных
	int uk;
	int str;
	TypeLex lex;
	int lexType = scan->scaner(lex);

	if (lexType < Tint || lexType > Tdouble)
		if (lexType != Tident) {
			PrintError(scan->getStringNumber());
		}

	do {
		Var();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	} while (lexType == TCom);	// ,
	scan->putUK(uk);
	scan->putString(str);
	lexType = scan->scaner(lex);

	if (lexType != TDC) {	//	;
		PrintError(scan->getStringNumber());
	}
}


void TDiagram::Var() {	// СД переменной
	int uk;
	int str;
	TypeLex lex;
	int lexType = scan->scaner(lex);

	if (lexType != Tident) {
		PrintError(scan->getStringNumber());
	}

	uk = scan->getUK();
	str = scan->getStringNumber();
	lexType = scan->scaner(lex);
	if (lexType != TSave) { // если после идентификатора не "=", значит просто выходим - переменная без инициализации
		scan->putUK(uk);
		scan->putString(str);
		return;
	}

	uk = scan->getUK();
	str = scan->getStringNumber();
	lexType = scan->scaner(lex);
	if (lexType != TFLB) {	// инициализация переменных
		scan->putUK(uk);
		scan->putString(str);
		BetwiseOR();
		return;
	}

	// инициализация структур
	do {
		BetwiseOR();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	} while (lexType == TCom); // .
	scan->putUK(uk);
	scan->putString(str);

	if (lexType != TFRB) {	// }
		PrintError(scan->getStringNumber());
	}
}


void TDiagram::BetwiseOR() { // СД побитового ИЛИ
	BetwiseXOR();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TBOR) {	// |
		BetwiseXOR();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}


void TDiagram::BetwiseXOR() {	// СД побитового исключающего ИЛИ
	BetwiseAND();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TBExOr) {	// ^
		BetwiseAND();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}


void TDiagram::BetwiseAND() {	// СД побитового И
	Equal();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TBAnd) {	// &
		Equal();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}


void TDiagram::Equal() {	// СД сравнения "=="/"!="
	Compare();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);
	
	while (lexType == TEQ || lexType == TNEQ) { // "=="/"!="
		Compare();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}


void TDiagram::Compare() {	// СД сравнения "<" / "<=" / ">" / ">="
	Shift();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType >= TLT || lexType <= TGE) { // "<" / "<=" / ">" / ">="
		Shift();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}


void TDiagram::Shift() {	// СД сдвигов
	Addition();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TBLS || lexType == TBRS) {	// ">>" / "<<"
		Addition();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}


void TDiagram::Addition() {	// СД сложения (вычитания) 
	Multiplication();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TPlus || lexType == TMinus) {	// "+" / "-"
		Multiplication();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}


void TDiagram::Multiplication() {	// СД умножения (деления)
	BetwiseNOT();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TMult || lexType == TDiv) {	// "*" / "/"
		BetwiseNOT();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}


void TDiagram::BetwiseNOT() {	// СД побитового НЕ
	Elementary();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TBNot) {	//	~
		Elementary();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}

void TDiagram::Elementary() {	// СД элементарного выражения
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);
	
	// константа
	if (lexType == TConstDouble || lexType == TConstInt || lexType == TConstExp) {
		return;
	}

	// выражение в скобках
	if (lexType == TLB) {
		BetwiseOR();
		lexType = scan->scaner(lex);
		if (lexType != TRB) {
			PrintError(scan->getStringNumber());
		}
		return;
	}
	
	// идентификатор
	scan->putUK(uk);
	scan->putString(str);
	Identifiers();
}


void TDiagram::CompositeStatement() {	// СД составной оператор
	int uk;
	int str;
	TypeLex lex;
	int lexType = scan->scaner(lex);
	if (lexType != TFLB) {
		PrintError(scan->getStringNumber());
	}

	uk = scan->getUK();
	str = scan->getStringNumber();
	int lexType = scan->scaner(lex);
	while (lexType != TFRB) {
		// данные
		if (lexType == Tident) {
			lexType = scan->scaner(lex);
			scan->putUK(uk);
			scan->putString(str);
			if (lexType != TSave) {
				Data();
				uk = scan->getUK();
				str = scan->getStringNumber();
				lexType = scan->scaner(lex);
				continue;
			}
		}
		// опреатор
		scan->putUK(uk);
		scan->putString(str);
		lexType = scan->scaner(lex);
		do {
			Statement();
			uk = scan->getUK();
			str = scan->getStringNumber();
			lexType = scan->scaner(lex);
		} while (lexType == Tident || lexType == TFLB || lexType == Tfor || lexType == TDC);
		scan->putUK(uk);
		scan->putString(str);
	}
}

void TDiagram::Statement() {	// СД оператор
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	if (lexType == Tident) {	// присваивание 
		scan->putUK(uk);
		scan->putString(str);
		Identifiers();
		lexType = scan->scaner(lex);

		if (lexType != TSave) {	// =
			PrintError(scan->getStringNumber());
		}

		BetwiseOR();
		lexType = scan->scaner(lex);

		if (lexType != TDC) {	// ;
			PrintError(scan->getStringNumber());
		}

	}

	else if (lexType == TFLB) {	// "{" составной оператор
		scan->putUK(uk);
		scan->putString(str);
		CompositeStatement();
	}

	else if (lexType == Tfor) { // оператор for

		if (lexType == TLB) {	// (
			PrintError(scan->getStringNumber());
		}

		ActionsAndConditions();

		if (lexType == TRB) {	// )
			PrintError(scan->getStringNumber());
		}

		Statement();
	}

	else {	// пустой оператор
		if (lexType != TDC) {	// ;
			PrintError(scan->getStringNumber());
		}
	}
}

void TDiagram::ActionsAndConditions() {	// операторы цикла for
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);
	
	// действие перед циклом
	if (lexType != TDC) {	// ;
		scan->putUK(uk);
		scan->putString(str);
		BetwiseOR();
	}

	if (lexType != TDC) {	// ;
		PrintError(scan->getStringNumber());
	}

	// ограничение
	if (lexType != TDC) {	// ;
		scan->putUK(uk);
		scan->putString(str);
		BetwiseOR();
	}

	if (lexType != TDC) {	// ;
		PrintError(scan->getStringNumber());
	}

	// действие после каждого цикла
	if (lexType == TConstDouble || lexType == TConstInt || lexType == TConstExp || lexType == Tident || lexType == TLB) {
		BetwiseOR();
	}
}
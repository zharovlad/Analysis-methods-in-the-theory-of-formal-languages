#include "defs.hpp"
#include "scaner.hpp"
#include "diagram.hpp"


TDiagram::TDiagram(const string fileName) {
	scan = new TScaner(fileName);
}


void TDiagram::PrintError(int stringNumber, TypeLex lex, TypeLex exp) {
	cout << endl << "String #" << stringNumber << endl;
	cout << "Error unexpected lexem - " << lex << endl;
	cout << "Expexted lexem - " << exp << endl;
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
		PrintError(scan->getStringNumber(), lex, "int");
	}

	lexType = scan->scaner(lex);
	if (lexType != Tmain) {
		PrintError(scan->getStringNumber(), lex, "main");
	}

	lexType = scan->scaner(lex);
	if (lexType != TLB) {	// (
		PrintError(scan->getStringNumber(), lex, "(");
	}

	lexType = scan->scaner(lex);
	if (lexType != TRB) {	// )
		PrintError(scan->getStringNumber(), lex, ")");
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
				PrintError(scan->getStringNumber(), lex, "identifier");
			}
			
			lexType = scan->scaner(lex);
			if (lexType != TFLB) {	// {
				PrintError(scan->getStringNumber(), lex, "{");
			}

			uk = scan->getUK();
			str = scan->getStringNumber();
			lexType = scan->scaner(lex);
			while (lexType != TFRB) {
				scan->putUK(uk);
				scan->putString(str);
				Data();
				uk = scan->getUK();
				str = scan->getStringNumber();
				lexType = scan->scaner(lex);
			}

			lexType = scan->scaner(lex);
			if (lexType != TDC) {	// ;
				PrintError(scan->getStringNumber(), lex, ";");
			}
		}
		else {
			if (lexType == Tint) {
				lexType = scan->scaner(lex);
				// если int main - возвращаем указатель на позицию перед int и выходим из цикла и GlobalVar, т.к. мы уже в главной функции 
				// иначе возвращаем указатель на позицию перед int и вызываем Data - описывается глобальная переменная типа int
				if (lexType == Tmain) {
					break;
				}
			}
			scan->putUK(uk);
			scan->putString(str);
			Data();
		}

		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
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
			PrintError(scan->getStringNumber(), lex, "identifier");
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
			PrintError(scan->getStringNumber(), lex, "identifier");
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
		PrintError(scan->getStringNumber(), lex, ";");
	}
}


void TDiagram::Var() {	// СД переменной
	Identifiers();

	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

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
		Assignment();
		return;
	}

	// инициализация структур
	do {
		Assignment();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	} while (lexType == TCom); // .
	scan->putUK(uk);
	scan->putString(str);

	if (lexType != TFRB) {	// }
		PrintError(scan->getStringNumber(), lex, "}");
	}
}

void TDiagram::Assignment() {	// операция присваивания
	BetwiseOR();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);
	while (lexType == TSave) {	//	=
		BetwiseOR();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
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

	while (lexType >= TLT && lexType <= TGE) { // "<" / "<=" / ">" / ">="
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
		Assignment();
		lexType = scan->scaner(lex);
		if (lexType != TRB) {
			PrintError(scan->getStringNumber(), lex, ")");
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
		PrintError(scan->getStringNumber(), lex, "{");
	}

	uk = scan->getUK();
	str = scan->getStringNumber();
	lexType = scan->scaner(lex);
	while (lexType != TFRB) {
		// данные
		if (lexType >= Tint && lexType <= Tdouble) {
			scan->putUK(uk);
			scan->putString(str);
			Data();
			uk = scan->getUK();
			str = scan->getStringNumber();
			lexType = scan->scaner(lex);
			continue;
		}
		if (lexType == Tident) {
			lexType = scan->scaner(lex);
			scan->putUK(uk);
			scan->putString(str);
			if (lexType != TDot) {
				Data();
				uk = scan->getUK();
				str = scan->getStringNumber();
				lexType = scan->scaner(lex);
				continue;
			}
		}

		// опреатор
		do {
			scan->putUK(uk);
			scan->putString(str);
			Statement();
			uk = scan->getUK();
			str = scan->getStringNumber();
			lexType = scan->scaner(lex);
			if (lexType == Tident) {
				int _uk = scan->getUK();
				int _str = scan->getStringNumber();
				int _lexType = scan->scaner(lex);
				if (_lexType == Tident) {
					break;
				}
			}
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
			PrintError(scan->getStringNumber(), lex, "=");
		}

		Assignment();
		lexType = scan->scaner(lex);

		if (lexType != TDC) {	// ;
			PrintError(scan->getStringNumber(), lex, ";");
		}

	}

	else if (lexType == TFLB) {	// "{" составной оператор
		scan->putUK(uk);
		scan->putString(str);
		CompositeStatement();
	}

	else if (lexType == Tfor) { // оператор for

		lexType = scan->scaner(lex);
		if (lexType != TLB) {	// (
			PrintError(scan->getStringNumber(), lex, "(");
		}

		ActionsAndConditions();

		lexType = scan->scaner(lex);
		if (lexType != TRB) {	// )
			PrintError(scan->getStringNumber(), lex, ")");
		}
		
		Statement();
	}

	else {	// пустой оператор
		if (lexType != TDC) {	// ;
			PrintError(scan->getStringNumber(), lex, ";");
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
		if (lexType >= Tint && lexType <= Tstruct || lexType == Tident) {
			Data();
		}
		else {
			Assignment();
			uk = scan->getUK();
			str = scan->getStringNumber();
			lexType = scan->scaner(lex);
			if (lexType != TDC) {	// ;
				PrintError(scan->getStringNumber(), lex, ";");
			}
			scan->putUK(uk);
			scan->putString(str);
		}
	}
	uk = scan->getUK();
	str = scan->getStringNumber();
	lexType = scan->scaner(lex);
	// ограничение
	if (lexType != TDC) {	// ;
		scan->putUK(uk);
		scan->putString(str);
		Assignment();
	}
	lexType = scan->scaner(lex);
	if (lexType != TDC) {	// ;
		PrintError(scan->getStringNumber(), lex, ";");
	}

	uk = scan->getUK();
	str = scan->getStringNumber();
	lexType = scan->scaner(lex);
	scan->putUK(uk);
	scan->putString(str);
	// действие после каждого цикла
	if (lexType != TRB) {
		Assignment();
	}
	
}

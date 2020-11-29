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

	if (lexType == TEnd)
		return;
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
	if (lexType != TLB) {
		PrintError(scan->getStringNumber());
	}

	lexType = scan->scaner(lex);
	if (lexType != TRB) {
		PrintError(scan->getStringNumber());
	}

	CompositeStatement();
}


void TDiagram::GlobalVar() {			// НЕ ДОДЕЛАНО!!!!!!!
	
	TypeLex lex;
	int lexType = scan->scaner(lex);
	// не понятно до куда проверять
	// возможно нужно 2 раза проводить сканирование и сравнивать с int main
}


void TDiagram::Identifiers() {
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
	} while (lexType == TDot);
	scan->putUK(uk);
	scan->putString(str);
}


void TDiagram::Data() {
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
	} while (lexType == TCom);
	scan->putUK(uk);
	scan->putString(str);
	lexType = scan->scaner(lex);

	if (lexType != TDC) {
		PrintError(scan->getStringNumber());
	}
}


void TDiagram::Var() {
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
	if (lexType != TSave) {
		scan->putUK(uk);
		scan->putString(str);
		return;
	}

	uk = scan->getUK();
	str = scan->getStringNumber();
	lexType = scan->scaner(lex);
	if (lexType != TFLB) {
		scan->putUK(uk);
		scan->putString(str);
		BetwiseOR();
		return;
	}

	do {
		BetwiseOR();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	} while (lexType == TCom);
	scan->putUK(uk);
	scan->putString(str);

	if (lexType != TFRB) {
		PrintError(scan->getStringNumber());
	}
}


void TDiagram::BetwiseOR() {
	BetwiseXOR();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TBOR) {
		BetwiseXOR();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}

void TDiagram::BetwiseXOR() {
	BetwiseAND();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TBExOr) {
		BetwiseAND();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}

void TDiagram::BetwiseAND() {
	Equal();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TBAnd) {
		Equal();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}

void TDiagram::Equal() {
	Compare();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);
	
	while (lexType == TEQ || lexType == TNEQ) {
		Compare();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}

void TDiagram::Compare() {
	Shift();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType >= TLT || lexType <= TGE) {
		Shift();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}

void TDiagram::Shift() {
	Addition();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TBLS || lexType == TBRS) {
		Addition();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}

void TDiagram::Addition() {
	Multiplication();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TPlus || lexType == TMinus) {
		Multiplication();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}

void TDiagram::Multiplication() {
	BetwiseNOT();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TMult || lexType == TDiv) {
		BetwiseNOT();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}

void TDiagram::BetwiseNOT() {
	Elementary();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TBNot) {
		Elementary();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
}

void TDiagram::Elementary() {
	TypeLex lex;
	int lexType = scan->scaner(lex);
	
	if (lexType == TConstDouble || lexType == TConstInt || lexType == TConstExp) {
		return;
	}

	if (lexType == TLB) {
		BetwiseOR();
		lexType = scan->scaner(lex);
		if (lexType != TRB) {
			PrintError(scan->getStringNumber());
		}
		return;
	}

	Identifiers();
}

void TDiagram::CompositeStatement() {	// не доделано!!!!!!
	int uk;
	int str;
	TypeLex lex;
	int lexType = scan->scaner(lex);
	if (lexType != TFLB) {
		PrintError(scan->getStringNumber());
	}

	//while (lexType != TFRB) {
	//	;
	//}
}

void TDiagram::Statement() {
	;
}

void TDiagram::ActionsAndConditions() {
	;
}
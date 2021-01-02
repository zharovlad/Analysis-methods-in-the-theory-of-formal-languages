#include "defs.hpp"
#include "diagram.hpp"


TDiagram::TDiagram(const string fileName) {
	this->scan = new TScaner(fileName);
	this->root = new Tree();
	Tree::SetScaner(scan);
	root->SetPos();
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
		scan->printError("Syntax", lex, "int");
	}

	lexType = scan->scaner(lex);
	if (lexType != Tmain) {
		scan->printError("Syntax", lex, "main");
	}

	Tree::AddAreaofVisibility(lex);

	lexType = scan->scaner(lex);
	if (lexType != TLB) {	// (
		scan->printError("Syntax", lex, "(");
	}

	lexType = scan->scaner(lex);
	if (lexType != TRB) {	// )
		scan->printError("Syntax", lex, ")");
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
				scan->printError("Syntax", lex, "identifier");
			}
			
			auto tree = Tree::GetPos();
			tree->AddTypeStruct(lex);

			lexType = scan->scaner(lex);
			if (lexType != TFLB) {	// {
				scan->printError("Syntax", lex, "{");
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
				scan->printError("Syntax", lex, ";");
			}

			tree->SetPos();
			Tree::CorrectPos();
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
			scan->printError("Syntax", lex, "identifier");
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
	int type = lexType;

	Tree* struc = nullptr;
	if (lexType < Tint || lexType > Tdouble) {
		if (lexType != Tident) {
			scan->printError("Syntax", lex, "identifier");
		}
		struc = Tree::FindUp(Tree::GetPos(), lex);
		if (struc == nullptr) {
			scan->printError("Semant", notDefine, lex);
		}
	}

	do {
		Var(type, struc);
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	} while (lexType == TCom);	// ,

	scan->putUK(uk);
	scan->putString(str);
	lexType = scan->scaner(lex);

	if (lexType != TDC) {	//	;
		scan->printError("Syntax", lex, ";");
	}
}


void TDiagram::Var(int type, Tree *struc) {	// СД переменной

	TypeLex lex;
	int lexType = scan->scaner(lex);

	if (lexType != Tident) {
		scan->printError("Syntax", lex, "identifier");
	}
	
	// добавляем переменную в семантическое дерево
	Tree::AddID(lex, type, struc);
	
	int uk = scan->getUK();
	int str = scan->getStringNumber();
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
		scan->printError("Syntax", lex, "}");
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
			scan->printError("Syntax", lex, ")");
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
		scan->printError("Syntax", lex, "{");
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
			scan->printError("Syntax", lex, "=");
		}

		Assignment();
		lexType = scan->scaner(lex);

		if (lexType != TDC) {	// ;
			scan->printError("Syntax", lex, ";");
		}

	}

	else if (lexType == TFLB) {	// "{" составной оператор
		scan->putUK(uk);
		scan->putString(str);
		Tree* k = Tree::GetPos();
		Tree::AddAreaofVisibility("");
		CompositeStatement();
		k->SetPos();
		Tree::CorrectPos();
	}

	else if (lexType == Tfor) { // оператор for
		Tree* k = Tree::GetPos();
		Tree::AddAreaofVisibility(lex);
		lexType = scan->scaner(lex);
		if (lexType != TLB) {	// (
			scan->printError("Syntax", lex, "(");
		}

		ActionsAndConditions();

		lexType = scan->scaner(lex);
		if (lexType != TRB) {	// )
			scan->printError("Syntax", lex, ")");
		}
		
		Statement();
		k->SetPos();
		Tree::CorrectPos();
	}

	else {	// пустой оператор
		if (lexType != TDC) {	// ;
			scan->printError("Syntax", lex, ";");
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
				scan->printError("Syntax", lex, ";");
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
		scan->printError("Syntax", lex, ";");
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

void TDiagram::PrintSemantTree() {
	//cout << "TYPE_INT = 1" << endl;
	//cout << "TYPE_SHORT = 2" << endl;
	//cout << "TYPE_LONG = 3" << endl;
	//cout << "TYPE_STRUCT = 4" << endl;
	//cout << "TYPE_NAME_STRUCT = 5" << endl;
	//cout << "TYPE_EMPTY = 6" << endl;
	//cout << "TYPE_MAIN = 7" << endl << endl;
	
	root->Print();
}
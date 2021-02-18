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
				Data(false);
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

	PrintSemantTree("after global variables");
}


vector<string> TDiagram::Identifiers() {	// СД идентификаторов
	int uk;
	int str;
	TypeLex lex;
	int lexType;

	vector<string> result;

	do {
		lexType = scan->scaner(lex);
		if (lexType != Tident) {
			scan->printError("Syntax", lex, "identifier");
		}
		result.push_back(lex);
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	} while (lexType == TDot);	// случай a.b.c.d.e...
	scan->putUK(uk);
	scan->putString(str);

	return result;
}


void TDiagram::Data(bool needAllocation) {	// СД данных
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
		Var(type, struc, needAllocation);
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


void TDiagram::Var(int type, Tree *struc, bool needAllocation) {	// СД переменной

	TypeLex lex;
	int lexType = scan->scaner(lex);

	if (lexType != Tident) {
		scan->printError("Syntax", lex, "identifier");
	}
	vector<string> ident = { lex };

	DataType dataType = DTYPE_UNKNOWN;

	switch (type) {

	case Tint:
		type = TYPE_INT;
		dataType = DTYPE_INT;
		break;
	case Tshort:
		type = TYPE_SHORT;
		break;
	case Tlong:
		type = TYPE_LONG;
		break;
	case Tdouble:
		type = TYPE_DOUBLE;
		dataType = DTYPE_DOUBLE;
		break;
	case Tident:
		type = TYPE_NAME_STRUCT;
		break;
	}
	// добавляем переменную в семантическое дерево
	Tree::AddID(lex, type, dataType, struc, needAllocation);
	
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
		pair<DataType, DataValue> dataVar = Assignment();
		Tree::SetData(ident, dataVar.first, dataVar.second);
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


pair<DataType, DataValue> TDiagram::Assignment() {	// операция присваивания
	pair<DataType, DataValue> res = BetwiseOR();
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
	return res;
}


pair<DataType, DataValue> TDiagram::BetwiseOR() { // СД побитового ИЛИ
	pair<DataType, DataValue> res = BetwiseXOR();
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
	return res;
}


pair<DataType, DataValue> TDiagram::BetwiseXOR() {	// СД побитового исключающего ИЛИ
	pair<DataType, DataValue> res = BetwiseAND();
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
	return res;
}


pair<DataType, DataValue> TDiagram::BetwiseAND() {	// СД побитового И
	pair<DataType, DataValue> res = Equal();
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
	return res;
}


pair<DataType, DataValue> TDiagram::Equal() {	// СД сравнения "=="/"!="
	pair<DataType, DataValue> res = Compare();
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
	return res;
}


pair<DataType, DataValue> TDiagram::Compare() {	// СД сравнения "<" / "<=" / ">" / ">="
	pair<DataType, DataValue> res = Shift();
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
	return res;
}


pair<DataType, DataValue> TDiagram::Shift() {	// СД сдвигов
	pair<DataType, DataValue> res = Addition();
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
	return res;
}


pair<DataType, DataValue> TDiagram::Addition() {	// СД сложения (вычитания) 
	pair<DataType, DataValue> res = Multiplication();
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
	return res;
}


pair<DataType, DataValue> TDiagram::Multiplication() {	// СД умножения (деления)
	pair<DataType, DataValue> res = BetwiseNOT();
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
	return res;
}


pair<DataType, DataValue> TDiagram::BetwiseNOT() {	// СД побитового НЕ
	pair<DataType, DataValue> res = Elementary();
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
	return res;
}


pair<DataType, DataValue> TDiagram::Elementary() {	// СД элементарного выражения
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);
	
	DataType dataType;
	DataValue dataValue;
	istringstream iss(lex, istringstream::in);
	// константа
	switch (lexType)
	{
	case TConstDouble:
		dataType = DTYPE_DOUBLE;
		iss >> dataValue.dataAsDouble;
		return make_pair(dataType, dataValue);
	case TConstInt:
		dataType = DTYPE_INT;
		iss >> dataValue.dataAsInt;
		return make_pair(dataType, dataValue);
	case TConstExp:
		dataType = DTYPE_DOUBLE;
		iss >> dataValue.dataAsDouble;
		return make_pair(dataType, dataValue);
	default:
		break;
	}
		

	// выражение в скобках
	if (lexType == TLB) {
		Assignment();
		lexType = scan->scaner(lex);
		if (lexType != TRB) {
			scan->printError("Syntax", lex, ")");
		}
		return make_pair(dataType, dataValue);	/////////////////////////////////////// заглушка
	}
	
	// идентификатор
	scan->putUK(uk);
	scan->putString(str);
	vector<string> ident = Identifiers();
	
	return Tree::GetData(ident);
}


void TDiagram::CompositeStatement() {	// СД составной оператор
	PrintSemantTree("before composite statement");
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
			if (lexType == TSave) {
				;
			}
			else {
				if (lexType != TDot) {
					Data();
					uk = scan->getUK();
					str = scan->getStringNumber();
					lexType = scan->scaner(lex);
					continue;
				}
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
	PrintSemantTree("in composite statement");
	Tree::FreeMemory();
	PrintSemantTree("after composite statement");
}


void TDiagram::Statement() {	// СД оператор
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	if (lexType == Tident) {	// присваивание 
		scan->putUK(uk);
		scan->putString(str);
		vector<string> ident = Identifiers();
		lexType = scan->scaner(lex);

		if (lexType != TSave) {	// =
			scan->printError("Syntax", lex, "=");
		}

		pair<DataType, DataValue> dataVar = Assignment();
		Tree::SetData(ident, dataVar.first, dataVar.second);
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
		//Tree::CorrectPos();
	}

	else if (lexType == Tfor) { // оператор for
		PrintSemantTree("before for");
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

		uk = scan->getUK();
		lexType = scan->scaner(lex);
		str = scan->getStringNumber();

		if (lexType >= Tint && lexType <= Tstruct) {
			scan->putUK(uk);
			scan->putString(str);
			Data();
		}
		else {
			scan->putUK(uk);
			scan->putString(str);
			Statement();
		}

		PrintSemantTree("in for");
		Tree::FreeMemory();
		PrintSemantTree("after for");
		
		k->SetPos();
		//Tree::CorrectPos();
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
		lexType = scan->scaner(lex);
		if (lexType != TDC) {	// ;
			scan->printError("Syntax", lex, ";");
		}
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

void TDiagram::PrintSemantTree(string info) {
	static int count = 1;
	cout << count++ << " position " << info << endl;
	root->Print();
	cout << endl << endl;
}
#include "defs.hpp"
#include "diagram.hpp"


TDiagram::TDiagram(const string fileName) {
	this->scan = new TScaner(fileName);
	this->root = new Tree();
	Tree::SetScaner(scan);
	root->SetPos();
}


void TDiagram::Prog() {					// �������
	int uk = scan->getUK();				// �������� ������� ����� ��� if ������� ��������������� ������������
	int str = scan->getStringNumber();	// 2 ����������, ����� ����� ���� ������� ��������� � ����� ������ �� ������������
	TypeLex lex;						// ��������� �������
	int lexType = scan->scaner(lex);	// ��� �������

	if (lexType == TEnd) // ������ ���������
		return;
	
	//
	// ����� ������� ���������� ����������, ����� ������� �������
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


void TDiagram::GlobalVar() {	// �� ���������� ������� 
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);
	while (lexType >= Tint || lexType <= Tstruct) {

		if (lexType == Tstruct) {	// ����� �������� ���������

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
				// ���� int main - ���������� ��������� �� ������� ����� int � ������� �� ����� � GlobalVar, �.�. �� ��� � ������� ������� 
				// ����� ���������� ��������� �� ������� ����� int � �������� Data - ����������� ���������� ���������� ���� int
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


vector<string> TDiagram::Identifiers() {	// �� ���������������
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
	} while (lexType == TDot);	// ������ a.b.c.d.e...
	scan->putUK(uk);
	scan->putString(str);

	return result;
}


void TDiagram::Data(bool needAllocation) {	// �� ������
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


void TDiagram::Var(int type, Tree *struc, bool needAllocation) {	// �� ����������

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
	// ��������� ���������� � ������������� ������
	Tree::AddID(lex, type, dataType, struc, needAllocation);
	
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	lexType = scan->scaner(lex);

	if (lexType != TSave) { // ���� ����� �������������� �� "=", ������ ������ ������� - ���������� ��� �������������
		scan->putUK(uk);
		scan->putString(str);
		return;
	}

	uk = scan->getUK();
	str = scan->getStringNumber();
	lexType = scan->scaner(lex);
	if (lexType != TFLB) {	// ������������� ����������
		scan->putUK(uk);
		scan->putString(str);
		pair<DataType, DataValue> dataVar = Assignment();
		Tree::SetData(ident, dataVar.first, dataVar.second);
		return;
	}

	// ������������� ��������

	struc = Tree::FindID(ident[0]);
	struc = struc->GetStructFirstVar();
	do {
		pair<DataType, DataValue> dataVar = Assignment();
		struc = Tree::NextInit(struc, dataVar);
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	} while (lexType == TCom); // ,

	if (lexType != TFRB) {	// }
		scan->printError("Syntax", lex, "}");
	}
}


pair<DataType, DataValue> TDiagram::Assignment() {	// �������� ������������
	pair<DataType, DataValue> res = BetwiseOR();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);
	while (lexType == TSave) {	//	=
		res = BetwiseOR();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
	return res;
}


pair<DataType, DataValue> TDiagram::BetwiseOR() { // �� ���������� ���
	pair<DataType, DataValue> res = BetwiseXOR();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TBOR) {	// |
		pair<DataType, DataValue> res2 = BetwiseXOR();
		if (res.first == DTYPE_DOUBLE || res2.first == DTYPE_DOUBLE) {
			scan->printError("Wrong type");
		}
		res.second.dataAsInt = res.second.dataAsInt | res2.second.dataAsInt;
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
	return res;
}


pair<DataType, DataValue> TDiagram::BetwiseXOR() {	// �� ���������� ������������ ���
	pair<DataType, DataValue> res = BetwiseAND();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TBExOr) {	// ^
		pair<DataType, DataValue> res2 = BetwiseAND();
		if (res.first == DTYPE_DOUBLE || res2.first == DTYPE_DOUBLE) {
			scan->printError("Wrong type");
		}
		res.second.dataAsInt = res.second.dataAsInt ^ res2.second.dataAsInt;
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
	return res;
}


pair<DataType, DataValue> TDiagram::BetwiseAND() {	// �� ���������� �
	pair<DataType, DataValue> res = Equal();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TBAnd) {	// &
		pair<DataType, DataValue> res2 = Equal();
		if (res.first == DTYPE_DOUBLE || res2.first == DTYPE_DOUBLE) {
			scan->printError("Wrong type");
		}
		res.second.dataAsInt = res.second.dataAsInt & res2.second.dataAsInt;
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
	return res;
}


pair<DataType, DataValue> TDiagram::Equal() {	// �� ��������� "=="/"!="
	pair<DataType, DataValue> res = Compare();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);
	
	while (lexType == TEQ || lexType == TNEQ) { // "=="/"!="
		pair<DataType, DataValue> res2 = Compare();
		switch (res.first) {
			
		case DTYPE_INT:
			switch (res2.first) {
			case DTYPE_INT:         
				if (lexType == TEQ) {	// int == int
					res.second.dataAsInt = res.second.dataAsInt == res2.second.dataAsInt ? 1 : 0;
				}
				else {	// int != int
					res.second.dataAsInt = res.second.dataAsInt != res2.second.dataAsInt ? 1 : 0;
				}
				break;
			case DTYPE_DOUBLE:      
				if (lexType == TEQ) {	// int == double
					res.second.dataAsInt = res.second.dataAsInt == res2.second.dataAsDouble ? 1 : 0;
				}
				else {	// int != double
					res.second.dataAsInt = res.second.dataAsInt != res2.second.dataAsDouble ? 1 : 0;
				}
				break;
			default:
				break;
			}
			break;

		case DTYPE_DOUBLE:
			switch (res2.first) {
			case DTYPE_INT:
				if (lexType == TEQ) {	// double == int
					res.second.dataAsInt = res.second.dataAsDouble == res2.second.dataAsInt ? 1 : 0;
				}
				else {	// double != int
					res.second.dataAsInt = res.second.dataAsDouble != res2.second.dataAsInt ? 1 : 0;
				}
				break;
			case DTYPE_DOUBLE:
				if (lexType == TEQ) {	// double == double
					res.second.dataAsInt = res.second.dataAsDouble == res2.second.dataAsDouble ? 1 : 0;
				}
				else {	// double != double
					res.second.dataAsInt = res.second.dataAsDouble != res2.second.dataAsDouble ? 1 : 0;
				}
				break;
			default:
				break;
			}
			break;

		default:
			break;
		}

		res.first = DTYPE_INT;
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
	return res;
}


pair<DataType, DataValue> TDiagram::Compare() {	// �� ��������� "<" / "<=" / ">" / ">="
	pair<DataType, DataValue> res = Shift();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType >= TLT && lexType <= TGE) { // "<" / "<=" / ">" / ">="
		pair<DataType, DataValue> res2 = Shift();
		switch (res.first) {

		case DTYPE_INT:
			switch (res2.first) {
			case DTYPE_INT:	// int  int
				if (lexType == TLT) {	// <
					res.second.dataAsInt = res.second.dataAsInt < res2.second.dataAsInt ? 1 : 0;
				}
				else if (lexType == TLE) {	// <=
					res.second.dataAsInt = res.second.dataAsInt <= res2.second.dataAsInt ? 1 : 0;
				}
				else if (lexType == TGT) {	// >
					res.second.dataAsInt = res.second.dataAsInt > res2.second.dataAsInt ? 1 : 0;
				}
				else {	// >=
					res.second.dataAsInt = res.second.dataAsInt >= res2.second.dataAsInt ? 1 : 0;
				}
				break;
			case DTYPE_DOUBLE:	// int double
				if (lexType == TLT) {	// <
					res.second.dataAsInt = res.second.dataAsInt < res2.second.dataAsDouble ? 1 : 0;
				}
				else if (lexType == TLE) {	// <=
					res.second.dataAsInt = res.second.dataAsInt <= res2.second.dataAsDouble ? 1 : 0;
				}
				else if (lexType == TGT) {	// >
					res.second.dataAsInt = res.second.dataAsInt > res2.second.dataAsDouble ? 1 : 0;
				}
				else {	// >=
					res.second.dataAsInt = res.second.dataAsInt >= res2.second.dataAsDouble ? 1 : 0;
				}
				break;
			default:
				break;
			}
			break;

		case DTYPE_DOUBLE:
			switch (res2.first) {
			case DTYPE_INT:	// double  int
				if (lexType == TLT) {	// <
					res.second.dataAsInt = res.second.dataAsDouble < res2.second.dataAsInt ? 1 : 0;
				}
				else if (lexType == TLE) {	// <=
					res.second.dataAsInt = res.second.dataAsDouble <= res2.second.dataAsInt ? 1 : 0;
				}
				else if (lexType == TGT) {	// >
					res.second.dataAsInt = res.second.dataAsDouble > res2.second.dataAsInt ? 1 : 0;
				}
				else {	// >=
					res.second.dataAsInt = res.second.dataAsDouble >= res2.second.dataAsInt ? 1 : 0;
				}
				break;
			case DTYPE_DOUBLE:	// int double
				if (lexType == TLT) {	// <
					res.second.dataAsInt = res.second.dataAsDouble < res2.second.dataAsDouble ? 1 : 0;
				}
				else if (lexType == TLE) {	// <=
					res.second.dataAsInt = res.second.dataAsDouble <= res2.second.dataAsDouble ? 1 : 0;
				}
				else if (lexType == TGT) {	// >
					res.second.dataAsInt = res.second.dataAsDouble > res2.second.dataAsDouble ? 1 : 0;
				}
				else {	// >=
					res.second.dataAsInt = res.second.dataAsDouble >= res2.second.dataAsDouble ? 1 : 0;
				}
				break;
			default:
				break;
			}
			break;
		}

		res.first = DTYPE_INT;
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
	return res;
}


pair<DataType, DataValue> TDiagram::Shift() {	// �� �������
	pair<DataType, DataValue> res = Addition();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TBLS || lexType == TBRS) {	// TBLS = "<<", TBRS = ">>"
		pair<DataType, DataValue> res2 = Addition();
		if (res.first == DTYPE_DOUBLE || res2.first == DTYPE_DOUBLE) {
			scan->printError("Wrong type");
		}
		if (lexType == TBLS)
			res.second.dataAsInt = res.second.dataAsInt << res2.second.dataAsInt;
		else
			res.second.dataAsInt = res.second.dataAsInt >> res2.second.dataAsInt;
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
	return res;
}


pair<DataType, DataValue> TDiagram::Addition() {	// �� �������� (���������) 
	pair<DataType, DataValue> res = Multiplication();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TPlus || lexType == TMinus) {	// "+" / "-"
		pair<DataType, DataValue> res2 = Multiplication();

		if (res.first == DTYPE_DOUBLE || res2.first == DTYPE_DOUBLE) {
			res = Tree::IntToDouble(res);
			res2 = Tree::IntToDouble(res2);
			if (lexType == TPlus)
				res.second.dataAsDouble = res.second.dataAsDouble + res2.second.dataAsDouble;
			else
				res.second.dataAsDouble = res.second.dataAsDouble - res2.second.dataAsDouble;
		}
		else {
			if (lexType == TPlus)
				res.second.dataAsInt = res.second.dataAsInt + res2.second.dataAsInt;
			else
				res.second.dataAsInt = res.second.dataAsInt - res2.second.dataAsInt;
		}
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
	return res;
}


pair<DataType, DataValue> TDiagram::Multiplication() {	// �� ��������� (�������)
	pair<DataType, DataValue> res = BetwiseNOT();
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	while (lexType == TMult || lexType == TDiv) {	// "*" / "/"
		pair<DataType, DataValue> res2 = BetwiseNOT();
		if (res.first == DTYPE_DOUBLE || res2.first == DTYPE_DOUBLE) {
			res = Tree::IntToDouble(res);
			res2 = Tree::IntToDouble(res2);
			if (lexType == TMult)
				res.second.dataAsDouble = res.second.dataAsDouble * res2.second.dataAsDouble;
			else
				res.second.dataAsDouble = res.second.dataAsDouble / res2.second.dataAsDouble;
		}
		else {
			if (lexType == TMult)
				res.second.dataAsInt = res.second.dataAsInt * res2.second.dataAsInt;
			else
				res.second.dataAsInt = res.second.dataAsInt / res2.second.dataAsInt;
		}
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	}
	scan->putUK(uk);
	scan->putString(str);
	return res;
}

pair<DataType, DataValue> TDiagram::BetwiseNOT() {	// �� ���������� ��
	
	pair<DataType, DataValue> res;
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	if (lexType == TBNot) {
		res = Elementary();
		if (res.first == DTYPE_DOUBLE) {
			scan->printError("Wrong type");
		}
		res.second.dataAsInt = ~res.second.dataAsInt;
	}
	else {
		scan->putUK(uk);
		scan->putString(str);
		res = Elementary();
	}
	return res;
}

pair<DataType, DataValue> TDiagram::Elementary() {	// �� ������������� ���������
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);
	
	DataType dataType;
	DataValue dataValue;
	istringstream iss(lex, istringstream::in);
	// ���������
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
		

	// ��������� � �������
	if (lexType == TLB) {
		pair<DataType, DataValue> res = Assignment();
		lexType = scan->scaner(lex);
		if (lexType != TRB) {
			scan->printError("Syntax", lex, ")");
		}
		return res;
	}
	
	// �������������
	scan->putUK(uk);
	scan->putString(str);
	vector<string> ident = Identifiers();
	
	return Tree::GetData(ident);
}


void TDiagram::CompositeStatement() {	// �� ��������� ��������
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
		// ������
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

		// ��������
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


void TDiagram::Statement() {	// �� ��������
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	if (lexType == Tident) {	// ������������ 
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

	else if (lexType == TFLB) {	// "{" ��������� ��������
		scan->putUK(uk);
		scan->putString(str);
		Tree* k = Tree::GetPos();
		Tree::AddAreaofVisibility("");
		CompositeStatement();
		k->SetPos();
		//Tree::CorrectPos();
	}

	else if (lexType == Tfor) { // �������� for
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

	else {	// ������ ��������
		if (lexType != TDC) {	// ;
			scan->printError("Syntax", lex, ";");
		}
	}
}


void TDiagram::ActionsAndConditions() {	// ��������� ����� for
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);
	
	// �������� ����� ������
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
	// �����������
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
	// �������� ����� ������� �����
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
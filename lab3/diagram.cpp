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
	exit(2);	// ��� ������ - ����� �������, �.�. ��� ���������� �������� �� ������ ������
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


void TDiagram::GlobalVar() {	// �� ���������� ������� 
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);
	while (lexType >= Tint || lexType <= Tstruct) {

		if (lexType == Tstruct) {	// ����� �������� ���������

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
}


void TDiagram::Identifiers() {	// �� ���������������
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
	} while (lexType == TDot);	// ������ a.b.c.d.e...
	scan->putUK(uk);
	scan->putString(str);
}


void TDiagram::Data() {	// �� ������
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


void TDiagram::Var() {	// �� ����������
	Identifiers();

	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

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
		Assignment();
		return;
	}

	// ������������� ��������
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

void TDiagram::Assignment() {	// �������� ������������
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

void TDiagram::BetwiseOR() { // �� ���������� ���
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


void TDiagram::BetwiseXOR() {	// �� ���������� ������������ ���
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


void TDiagram::BetwiseAND() {	// �� ���������� �
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


void TDiagram::Equal() {	// �� ��������� "=="/"!="
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


void TDiagram::Compare() {	// �� ��������� "<" / "<=" / ">" / ">="
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


void TDiagram::Shift() {	// �� �������
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


void TDiagram::Addition() {	// �� �������� (���������) 
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


void TDiagram::Multiplication() {	// �� ��������� (�������)
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


void TDiagram::BetwiseNOT() {	// �� ���������� ��
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


void TDiagram::Elementary() {	// �� ������������� ���������
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);
	
	// ���������
	if (lexType == TConstDouble || lexType == TConstInt || lexType == TConstExp) {
		return;
	}

	// ��������� � �������
	if (lexType == TLB) {
		Assignment();
		lexType = scan->scaner(lex);
		if (lexType != TRB) {
			PrintError(scan->getStringNumber(), lex, ")");
		}
		return;
	}
	
	// �������������
	scan->putUK(uk);
	scan->putString(str);
	Identifiers();
}


void TDiagram::CompositeStatement() {	// �� ��������� ��������
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
			if (lexType != TDot) {
				Data();
				uk = scan->getUK();
				str = scan->getStringNumber();
				lexType = scan->scaner(lex);
				continue;
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
}

void TDiagram::Statement() {	// �� ��������
	int uk = scan->getUK();
	int str = scan->getStringNumber();
	TypeLex lex;
	int lexType = scan->scaner(lex);

	if (lexType == Tident) {	// ������������ 
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

	else if (lexType == TFLB) {	// "{" ��������� ��������
		scan->putUK(uk);
		scan->putString(str);
		CompositeStatement();
	}

	else if (lexType == Tfor) { // �������� for

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

	else {	// ������ ��������
		if (lexType != TDC) {	// ;
			PrintError(scan->getStringNumber(), lex, ";");
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
				PrintError(scan->getStringNumber(), lex, ";");
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
	// �������� ����� ������� �����
	if (lexType != TRB) {
		Assignment();
	}
	
}

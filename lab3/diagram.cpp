#include "defs.hpp"
#include "scaner.hpp"
#include "diagram.hpp"


TDiagram::TDiagram(const string fileName) {
	scan = new TScaner(fileName);
}


void TDiagram::PrintError(int stringNumber) {
	cout << endl << "String #" << stringNumber << endl;
	cout << "Error : unexpected lexem." << endl;
	exit(2);	// ��� ������ - ����� �������, �.�. ��� ���������� �������� �� ������ ������
}


void TDiagram::Prog() {					// �������
	int uk = scan->getUK();				// �������� ������� ����� ��� if ������� ��������������� ������������
	int str = scan->getStringNumber();	// 2 ����������, ����� ����� ���� ������� ��������� � ����� ������ �� ������������
	TypeLex lex;						// ��������� �������
	int lexType = scan->scaner(lex);	// ��� �������

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


void TDiagram::GlobalVar() {			// �� ��������!!!!!!!
	
	TypeLex lex;
	int lexType = scan->scaner(lex);
	// �� ������� �� ���� ���������
	// �������� ����� 2 ���� ��������� ������������ � ���������� � int main
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
	int uk = scan->getUK();				// �������� ������� ����� ��� if ������� ��������������� ������������
	int str = scan->getStringNumber();	// 2 ����������, ����� ����� ���� ������� ��������� � ����� ������ �� ������������
	TypeLex lex;						// �������
	int lexType = scan->scaner(lex);	// ��� �������

	do {
		BetwiseXOR();
		uk = scan->getUK();
		str = scan->getStringNumber();
		lexType = scan->scaner(lex);
	} while (lexType == TBOR);
	scan->putUK(uk);
	scan->putString(str);
}
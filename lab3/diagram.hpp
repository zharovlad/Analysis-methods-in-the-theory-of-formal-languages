#ifndef __DIAGRAM
#define __DIAGRAM
#include "defs.hpp"
#include "scaner.hpp"

class TDiagram {
private:
	TScaner* scan;
	void GlobalVar();				// ���������� ����������
	void Identifiers();				// ��������������
	void Data();					// ������
	void Var();						// ����������
	void BetwiseOR();				// ��������� ���
	void BetwiseXOR();				// ��������� ����������� ���
	void BetwiseAND();				// ��������� �
	void Equal();					// ����� ��� �� �����
	void Compare();					// ���������
	void Shift();					// �����
	void Addition();				// ��������
	void Multiplication();			// ���������
	void BetwiseNOT();				// ��������� ��
	void Elementary();				// ������������ ���������
	void CompositeStatement();		// ��������� ��������
	void Statement();				// ��������
	void ActionsAndConditions();	// �������� � ������� ��� ����� for
	void PrintError(int, TypeLex, TypeLex);			// ������ ������
	void Assignment();

public:
	TDiagram(const string);
	~TDiagram();
	void Prog(); // �������
};

#endif
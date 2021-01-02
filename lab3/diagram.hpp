#ifndef __DIAGRAM
#define __DIAGRAM
#include "defs.hpp"
#include "semant.hpp"
#include "scaner.hpp"

class TDiagram {
private:
	TScaner* scan;
	Tree* root;

	void GlobalVar();				// ���������� ����������
	void Identifiers();				// ��������������
	void Data();					// ������
	void Var(int, Tree*);					// ����������
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

	void PrintSemantTree();
	TDiagram(const string);
	~TDiagram();
	void Prog(); // �������
};

#endif
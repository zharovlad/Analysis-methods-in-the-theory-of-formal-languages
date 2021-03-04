#ifndef __DIAGRAM
#define __DIAGRAM
#include "defs.hpp"
#include "semant.hpp"
#include "scaner.hpp"
#include <utility>
#include <sstream>

class TDiagram {
private:
	TScaner* scan;
	Tree* root;

	void GlobalVar();				// ���������� ����������
	vector<string> Identifiers();				// ��������������
	void Data(bool needAllocation=true);					// ������
	void Var(int, Tree*, bool);					// ����������
	pair<DataType, DataValue> BetwiseOR();				// ��������� ���
	pair<DataType, DataValue> BetwiseXOR();				// ��������� ����������� ���
	pair<DataType, DataValue> BetwiseAND();				// ��������� �
	pair<DataType, DataValue> Equal();					// ����� ��� �� �����
	pair<DataType, DataValue> Compare();					// ���������
	pair<DataType, DataValue> Shift();					// �����
	pair<DataType, DataValue> Addition();				// ��������
	pair<DataType, DataValue> Multiplication();			// ���������
	pair<DataType, DataValue> Unary();					// ������� ��������
	pair<DataType, DataValue> Elementary();				// ������������ ���������
	void CompositeStatement(bool needAllocation = true, bool flagInterpretFor = true);		// ��������� ��������
	void Statement(bool needAllocation=true, bool flagInterpretFor=true);				// ��������
	void For(bool flagInterpretFor);
	void ActionsAndConditions();	// �������� � ������� ��� ����� for
	pair<DataType, DataValue> Assignment();

public:

	void PrintSemantTree(string);
	TDiagram(const string);
	~TDiagram();
	void Prog(); // �������
};

#endif
#include "diagram.hpp"

const string inputFileName = "input6.txt";

int main() {
	TDiagram* diagram = new TDiagram(inputFileName);
	diagram->Prog();
	cout << "Errors not found" << endl;
	return 0;
}
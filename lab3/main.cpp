#include "diagram.hpp"

const string inputFileName = "input2.txt";

int main() {
	TDiagram* diagram = new TDiagram(inputFileName);
	diagram->Prog();
	return 0;
}
#include "diagram.hpp"

const string inputFileName = "input4.txt";

int main() {
	TDiagram* diagram = new TDiagram(inputFileName);
	diagram->Prog();
	return 0;
}
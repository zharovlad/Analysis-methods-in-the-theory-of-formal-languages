#include "diagram.hpp"

const string inputFileName = "input.txt";

int main() {
	TDiagram* diagram = new TDiagram(inputFileName);
	diagram->Prog();
	return 0;
}
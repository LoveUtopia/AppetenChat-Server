#include <iostream>
#include "CKernel.h"
using namespace std;
int main() {

	CKernel kernel;
	if (!kernel.startSever()) {
		cout << "kernel start error" << endl;
	}
	while (1) {
		Sleep(50);
	}

	return 0;
}
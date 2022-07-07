/*
main.cpp
2022-06-09
Collin Abraham

-Main function to drive the main loop and catch any exceptions that may occur 

*/

#include "commandloop.h"

using namespace std;
using namespace pqxx;


int main() {
	cout << "++++++++++++++++++++++++++++++" << endl;
	cout << "+++ Tree Management System +++" << endl;
	cout << "++++++++++++++++++++++++++++++" << endl;

	cout << "\nAttempting connection to database..." << endl;
	cout << "Enter \"help\" for options" << endl;

	try {
		// launch the user command loop
		command_loop();
	}
	catch (std::exception& e) {
		cerr << e.what() << endl;
	}

	return 0;
}
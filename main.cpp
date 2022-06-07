//#include <iostream>
//#include <pqxx/pqxx>
#include "commandloop.h"

using namespace std;
using namespace pqxx;


int main() {
	cout << "++++++++++++++++++++++++++++++" << endl;
	cout << "+++ Tree Management System +++" << endl;
	cout << "++++++++++++++++++++++++++++++" << endl;

	cout << "\nAttempting connection to database..." << endl;

	try {
		//connection connObj(connString()); // build a connection object to db
		//work worker(connObj); // worker obj to interact with db connection 
		cout << "Connection to database established..." << endl;
		cout << "Enter \"help\" for options" << endl;

		commandLoop();
		//worker.commit();
	}
	catch (std::exception& e) {
		cerr << e.what() << endl;
	}

	return 0;
}
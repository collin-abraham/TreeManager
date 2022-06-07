#include "commandloop.h"

using namespace std;


// helper func to clear out the buffer
void cleanBuffer() {
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cin.clear();
}

int commandLoop() {

	string userInput = "";

	for (;;) {

		// build connection object to db and prompt user for input
		pqxx::connection connObj(connString()); 
		cout << "\nEnter command: " << endl;
		getline(cin, userInput);
		

		/*************************************
		READ
		*************************************/

		// user wants to read records 
		if (userInput == "read") {	// no flags given 
			printResults(buildRead(connObj));
			cout << "\nPress enter to continue...";
			cleanBuffer();
			continue;
		}

		if (userInput.substr(0, 4) == "read" && userInput.length() > 4) { // flags were given with read

			if (userInput.substr(5, 7) == "-n") 
				printResults(buildReadName(connObj));

			else if (userInput.substr(5, 7) == "-N") 
				printResults(buildReadNameReversed(connObj));

			else if (userInput.substr(5, 7) == "-c") 
				printResults(buildReadCost(connObj));

			else if (userInput.substr(5, 7) == "-C") 
				printResults(buildReadCostReversed(connObj));

			else if (userInput.substr(5, 7) == "-q") 
				printResults(buildReadQuantity(connObj));

			else if (userInput.substr(5, 7) == "-Q") 
				printResults(buildReadQuantityReversed(connObj));

			else 
				cerr << "ERROR: Flag of " << userInput << " is not an option, enter \"help\" as a command for options" << endl; 

			cout << "\nPress enter to continue...";
			cleanBuffer();
			continue;
		} // end reads


		/*************************************
		CREATE
		*************************************/

		// user wants to create (insert) a new record
		else if (userInput.substr(0, 6) == "create") {

			// prompt user for some values
			string tree_name = "";
			double tree_cost = 0.0;
			int quantity = 0;
			std::string machineStr = "";
			const int colwidth = 15;

			cout << "\n +++ CREATE A NEW RECORD +++\n";

			for (;;) {
				cout << endl << left << setw(colwidth) << "Name: "; getline(cin, tree_name);

				// check that the contents are all alphanumeric 
				bool alphaFlag = true;
				for (const auto c : tree_name) {
					if (!isalnum(c)) {
						alphaFlag = false;
						break;
					}
				}

				if (!alphaFlag) {
					cerr << endl << "ERROR: Input failed Name must be alphanumeric (letters/numbers)!\n";
					continue;
				}

				if (tree_name == "") {
					cerr << endl << "ERROR: Tree name cannot be empty!";
					continue;
				}

				else if (tree_name.length() > 99) {
					cerr << endl << "ERROR: Tree name cannot be more than 100 characters!";
					continue;
				}

				else
					break; // error checking successful, exit loop
			}

			for (;;) {
				cout << endl << left << setw(colwidth) << "Cost: "; cin >> tree_cost;

				// check that the contents are all digits 
				bool digitFlag = true;
				for (const auto c : tree_name) {
					if (!isdigit(c) && c != '.') {		// allow for periods for decimal costs
						digitFlag = false;
						break;
					}
				}

				if (!digitFlag) {
					cerr << "ERROR: Cost must be a a valid numerical amount!\n";
					continue;
				}

				if (tree_cost <= 0.0 || tree_cost > 100000) {
					cerr << endl << "ERROR: Tree cost must be between 0 and 100000!";
					continue;
				}
				else
					break; // error checking successful, exit loop
			}

			for (;;) {
				cout << endl << left << setw(colwidth) << "Quantity: "; cin >> quantity;

				// check that the contents are all digits 
				bool digitFlag = true;
				for (const auto c : tree_name) {
					if (!isdigit(c)) {
						digitFlag = false;
						break;
					}
				}

				if (!digitFlag) {
					cerr << "ERROR: Cost must be a a valid numerical amount!\n";
					continue;
				}

				if (quantity <= 0 || quantity > 10000) {
					cerr << endl << "ERROR: Tree quantity must be between 0 and 10000!";
					continue;
				}
				else
					break; // error checking successful, exit loop
			}

			for (;;) {
				
				cout << endl << left << setw(colwidth) << "Machine (yes/no): "; cin >> machineStr;
				transform(machineStr.begin(), machineStr.end(), machineStr.begin(), ::tolower); // push to lower for simplicity

				if (machineStr == "") {
					cerr << endl << "ERROR: Machine field cannot be left blank!";
					continue;
				}

				else if (machineStr != "yes" && machineStr != "no") {
					cerr << endl << "ERROR: Machine status must be 'yes' or 'no' cannot be: \"" << machineStr << "\"!";
					continue;
				}
				else
					break;
			}

			// confirm details to user
			cout << endl << "------------------------------";
			cout << endl << "!!! CONFIRM RECORD DETAILS !!!";
			cout << endl << left << setw(colwidth) << "Name: " << left << setw(colwidth) << tree_name;
			cout << endl << left << setw(colwidth) << "Cost: " << left << setw(colwidth) << tree_cost;
			cout << endl << left << setw(colwidth) << "Quantity: " << left << setw(colwidth) << quantity;
			cout << endl << left << setw(colwidth) << "Machine: " << left << setw(colwidth) << machineStr;
			cout << endl << "------------------------------";
			
			string answer = "";
			cout << endl << "Correct (yes/no)? "; cin >> answer;
			transform(answer.begin(), answer.end(), answer.begin(), ::tolower); // push answer str to lowercase for ease of use
			
			for (;;) {
				if (answer != "yes" && answer != "no") {
					cerr << endl << "ERROR: Confirmation must be 'yes' or 'no' cannot be: \"" << answer << "\"!";
				continue;
				}
				else {
					if (answer == "yes") {	// user agreed to commit 
						
						const tuple<string, int, double, bool> insertDetails { tree_name, tree_cost, quantity, (machineStr == "yes" ? true : false)};
						buildCreate(connObj, insertDetails);
						break;
					}
					else {
						cerr << "\nCREATE RECORD TERMINATED\n";
						break;
					}
				}
			} // end user confirmation
			
			cleanBuffer();
			continue;
		} // end create


		/*************************************
		UPDATE
		*************************************/

		// user wants to update a record
		else if (userInput.substr(0,6) == "update" && userInput.length() >= 8) {

			// extract tree id from user input string 
			userInput.erase(userInput.begin(), userInput.begin() + 7); // trim update chars off

			for (const auto c : userInput) {
				if (!isdigit(c)) {	// see if we encounter something that isn't a digit
					cerr << "Input failed, usage is update tree_id. Use Read command to obtain tree_id" << endl;
					break;
				}
			}

			// display selection to user
			
			pqxx::result response = buildReadId(connObj, userInput);
			if (response.empty()) {
				cerr << "ERROR: tree_id of " << userInput << " could not be queried from database (does it exist?)\n";
				continue;
			}
			else 
				cout << "\n +++ UPDATE FOLLOWING RECORD +++\n";
			printResults(response);

			// obtain and validate user input before updating 
			string tree_name = "";
			double tree_cost = 0.0;
			int quantity = 0;
			std::string machineStr = "";
			const int colwidth = 15;

			for (;;) {
				cout << endl << left << setw(colwidth) << "Name: "; getline(cin, tree_name);

				// check that the contents are all alphanumeric 
				bool alphaFlag = true;
				for (const auto c : tree_name) {
					if (!isalnum(c)) {	
						alphaFlag = false;
						break;
					}
				}

				if (!alphaFlag) {
					cerr << endl << "ERROR: Input failed Name must be alphanumeric (letters/numbers)!\n";
					continue; 
				}

				if (tree_name == "") {
					cerr << endl << "ERROR: Tree name cannot be empty!\n";
					continue;
				}

				else if (tree_name.length() > 99) {
					cerr << endl << "ERROR: Tree name cannot be more than 100 characters!\n";
					continue;
				}

				else
					break; // error checking successful, exit loop
			}

			for (;;) {
				cout << endl << left << setw(colwidth) << "Cost: "; cin >> tree_cost;

				// check that the contents are all digits 
				bool digitFlag = true;
				for (const auto c : tree_name) {
					if (!isdigit(c) && c != '.') {		// allow for periods for decimal costs
						digitFlag = false;
						break;
					}
				}

				if (!digitFlag) {
					cerr << "ERROR: Cost must be a a valid numerical amount!\n";
					continue;
				}

				if (tree_cost <= 0.0 || tree_cost > 100000) {
					cerr << endl << "ERROR: Tree cost must be between 0 and 100000!\n";
					continue;
				}
				else
					break; // error checking successful, exit loop
			}

			for (;;) {
				cout << endl << left << setw(colwidth) << "Quantity: "; cin >> quantity;

				// check that the contents are all digits 
				bool digitFlag = true;
				for (const auto c : tree_name) {
					if (!isdigit(c)) {		
						digitFlag = false;
						break;
					}
				}

				if (!digitFlag) {
					cerr << "ERROR: Cost must be a a valid numerical amount!\n";
					continue;
				}

				if (quantity <= 0 || quantity > 10000) {
					cerr << endl << "ERROR: Tree quantity must be between 0 and 10000!\n";
					continue;
				}
				else
					break; // error checking successful, exit loop
			}

			for (;;) {

				cout << endl << left << setw(colwidth) << "Machine (yes/no): "; cin >> machineStr;
				transform(machineStr.begin(), machineStr.end(), machineStr.begin(), ::tolower); // push to lower for simplicity

				if (machineStr == "") {
					cerr << endl << "ERROR: Machine field cannot be left blank!\n";
					continue;
				}

				else if (machineStr != "yes" && machineStr != "no") {
					cerr << endl << "ERROR: Machine status must be 'yes' or 'no' cannot be: \"" << machineStr << "\"!\n";
					continue;
				}
				else
					break;
			}

			cout << endl << "------------------------------";
			cout << endl << "!!! CONFIRM UPDATE DETAILS !!!";
			cout << endl << left << setw(colwidth) << "Name: " << left << setw(colwidth) << tree_name;
			cout << endl << left << setw(colwidth) << "Cost: " << left << setw(colwidth) << tree_cost;
			cout << endl << left << setw(colwidth) << "Quantity: " << left << setw(colwidth) << quantity;
			cout << endl << left << setw(colwidth) << "Machine: " << left << setw(colwidth) << machineStr;
			cout << endl << "------------------------------";

			string answer = "";
			cout << endl << "Correct (yes/no)? "; cin >> answer;
			transform(answer.begin(), answer.end(), answer.begin(), ::tolower); // push answer str to lowercase for ease of use

			for (;;) {
				if (answer != "yes" && answer != "no") {
					cerr << endl << "ERROR: Confirmation must be 'yes' or 'no' cannot be: \"" << answer << "\"!";
					continue;
				}
				else {
					if (answer == "yes") {	// user agreed to commit 

						const tuple<int, string, int, double, bool> insertDetails{ stoi(userInput), tree_name, tree_cost, quantity, (machineStr == "yes" ? true : false) };
						buildUpdate(connObj, insertDetails);
						break;
					}
					else {
						cerr << "\n+++ UPDATE RECORD TERMINATED +++\n";
						break;
					}
				}
			} // end user confirmation
		
			cleanBuffer();
			continue;
		} // end update


		/*************************************
		DELETE
		*************************************/

		// user wants to delete a record
		else if (userInput.substr(0, 6) == "delete" && userInput.length() >= 8) {

			// extract tree id from user input string 
			userInput.erase(userInput.begin(), userInput.begin() + 7); // trim delete chars off

			for (const auto& c : userInput) {
				if (!isdigit(c)) {	// see if we encounter something that isn't a digit
					cerr << "Input failed, usage is delete tree_id. Use Read command to obtain tree_id" << endl;
					break;
				}
			}

			// display selection to user
			cout << "\n +++ DELETE FOLLOWING RECORD +++\n";
			printResults(buildReadId(connObj,userInput));

			string answer = "";
			cout << endl << "Are you sure you want to delete (yes/no)? "; cin >> answer;
			transform(answer.begin(), answer.end(), answer.begin(), ::tolower); // push answer str to lowercase for ease of use

			for (;;) {
				if (answer != "yes" && answer != "no") {
					cerr << endl << "ERROR: Confirmation must be 'yes' or 'no' cannot be: \"" << answer << "\"!";
					continue;
				}
				else {
					if (answer == "yes") {	// user agreed to commit 

						buildDelete(connObj, userInput);
						break;
					}
					else {
						cerr << "\n+++ DELETE RECORD TERMINATED +++\n";
						break;
					}
				}
			} // end user confirmation

			cleanBuffer();
			continue;
		}

		/*************************************
		REPORT
		*************************************/

		else if (userInput.substr(0,6) == "report") {

			if (userInput.length() > 6) { // user wants to print to a text file 

				// extract file name from user input string 
				userInput.erase(userInput.begin(), userInput.begin() + 7); // trim report chars off
				ofstream fileOut(userInput);

				// if we can't open the file the user provided
				if (!fileOut.is_open()) {
					cerr << "ERROR: File \"" << userInput << "\" could not be opened to write to! (permissions?)";
					continue;
				}
				else // build the report into the file 
					buildReport(connObj, fileOut, userInput);
			}
			else
				buildReport(connObj);
		}
		
		// user enters exit cmd
		else if (userInput == "exit")
			return EXIT_SUCCESS;

		// final else statement 
		else {
			cerr << "ERROR: Input of \"" << userInput << "\" invalid. Enter help for options. Enter exit to close\n";
			continue;			
		}
		
		cleanBuffer();
		connObj.close(); // close the connection 
	} // main cmd loop

	return 0;
}
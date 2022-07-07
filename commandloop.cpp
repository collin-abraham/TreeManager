/*
commandLoop.cpp
2022-06-09
Collin Abraham

-Main driving functionality of the program
-Takes the user's input, parses the string and allows the CRUD functionality within DB
-Definitions of declarations within commandLoop.h
*/

#include "commandloop.h"
#include "validationFunctions.h"

using namespace std;


/* Helper function that ensures the buffer is clear between obtaining user input 
This was needed to ensure the infinite loop within commandLoop() wouldn't catch a faulty input and hang forever */
void clean_buffer() {
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cin.clear();
}

/* Main driving function of the program, contained in an infinite for(;;) loop. 
The loop takes in user input as a string and does string analysis on it. Based on the 
contents, various functions are then executed. Each CRUD area is labeled within 
a 3 line comment block */
int command_loop() {

	string userInput = "";

	for (;;) {

		// build connection object to db and prompt user for input
		pqxx::connection connObj(conn_string()); 
		cout << "\nConnection established, enter command: " << endl;
		getline(cin, userInput);
		

		/*************************************
		READ
		*************************************/

		// user wants to read records 
		if (userInput == "read") {	// no flags given 
			print_results(build_read(connObj));
			//cout << "\nPress enter to continue...";
			//cleanBuffer();
			continue;
		}

		else if (userInput.substr(0, 4) == "read" && userInput.length() > 4) { // flags were given with read

			if (userInput.substr(5, 7) == "-n") 
				print_results(build_read_name(connObj));

			else if (userInput.substr(5, 7) == "-N") 
				print_results(build_read_name_reversed(connObj));

			else if (userInput.substr(5, 7) == "-p") 
				print_results(build_read_cost(connObj));

			else if (userInput.substr(5, 7) == "-P") 
				print_results(build_read_cost_reversed(connObj));

			else if (userInput.substr(5, 7) == "-q") 
				print_results(build_read_quantity(connObj));

			else if (userInput.substr(5, 7) == "-Q") 
				print_results(build_read_quantity_reversed(connObj));

			else 
				cerr << "ERROR: Flag of " << userInput << " is not an option, enter \"help\" as a command for options" << endl; 

			continue;

		} // end reads


		/*************************************
		CREATE
		*************************************/

		// user wants to create (insert) a new record
		else if (userInput.substr(0, 6) == "create") {

			// prompt user for some values
			string tree_name = "";
			string tree_cost = "";
			string quantity = "";
			string machineStr = "";

			const double cost_low = 0.0; // cost low end
			const double cost_high = 100000.0; // cost high end 
			const int quantity_low = 0; // quantity low end
			const int quantity_high = 10000; // quantity high end 
			const int colwidth = 15;

			cout << "\n +++ CREATE A NEW RECORD +++\n";

			for (;;) {
				cout << endl << left << setw(colwidth) << "Name: "; getline(cin, tree_name);
				
				// name must be alpha numeric, not empty and cannot be more than 100 characters 
				if (!valid_alpha_num(tree_name))
					continue;

				else if (!valid_empty(tree_name))
					continue;

				else if (!valid_name_length(tree_name, colwidth*2))
					continue;

				else
					break; // error checking successful, exit loop
			}

			for (;;) {
				cout << endl << left << setw(colwidth) << "Cost: "; getline(cin,tree_cost);

				if (!valid_digits_cost(tree_cost))
					continue; 

				if (!valid_limits(stod(tree_cost), cost_low, cost_high))
					continue; 

				else
					break; // error checking successful, exit loop
			}

			for (;;) {
				cout << endl << left << setw(colwidth) << "Quantity: "; getline(cin,quantity);

				// check that the contents are all digits 
				if (!valid_digits(quantity))
					continue;

				if (!valid_limits(stoi(quantity), quantity_low, quantity_high))
					continue;

				else
					break; // error checking successful, exit loop
			}

			for (;;) {
				
				cout << endl << left << setw(colwidth) << "Machine (yes/no): "; getline(cin, machineStr);
				transform(machineStr.begin(), machineStr.end(), machineStr.begin(), ::tolower); // push to lower for simplicity

				if (!valid_empty(machineStr))
					continue;

				if (!valid_yes_no(machineStr))
					continue;
				
				else
					break;
			}

			// confirm details to user
			cout << endl << "------------------------------";
			cout << endl << "!!! CONFIRM CREATE DETAILS !!!";
			cout << endl << left << setw(colwidth) << "Name: " << left << setw(colwidth) << tree_name;
			cout << endl << left << setw(colwidth) << "Cost: " << left << setw(colwidth) << tree_cost;
			cout << endl << left << setw(colwidth) << "Quantity: " << left << setw(colwidth) << quantity;
			cout << endl << left << setw(colwidth) << "Machine: " << left << setw(colwidth) << machineStr;
			cout << endl << "------------------------------";
			
			
			
			
			for (;;) {
				string answer = "";
				cout << endl << "Correct (yes/no)? "; cin >> answer;
				transform(answer.begin(), answer.end(), answer.begin(), ::tolower); // push answer str to lowercase for ease of use

				if (!valid_yes_no(answer))
					continue;
				else {
					if (answer == "yes") {	// user agreed to commit 
						
						const tuple<string, int, double, bool> insertDetails { 
							tree_name, 
							stoi(tree_cost), 
							stod(quantity), 
							(machineStr == "yes" ? true : false)
						};

						build_create(connObj, insertDetails);
						break;
					}
					else {
						cerr << "\n+++ CREATE RECORD TERMINATED +++\n";
						break;
					}
				}
			} // end user confirmation
			
			clean_buffer();
			continue;
		} // end create


		/*************************************
		UPDATE
		*************************************/

		// user wants to update a record
		else if (userInput.substr(0,6) == "update" && userInput.length() >= 8) {

			// extract tree id from user input string 
			userInput.erase(userInput.begin(), userInput.begin() + 7); // trim update chars off

			if (!valid_digits(userInput))
				continue;

			// display selection to user
			
			pqxx::result response = build_read_id(connObj, userInput);
			if (response.empty()) {
				cerr << "ERROR: tree_id of " << userInput << " could not be queried from database (does it exist?)\n";
				continue;
			}
			else 
				cout << "\n +++ UPDATE FOLLOWING RECORD +++\n";
			print_results(response);

			// obtain and validate user input before updating 
			string tree_name = "";
			string tree_cost = "";
			string quantity = "";
			std::string machineStr = "";

			const double cost_low = 0.0; // cost low end
			const double cost_high = 100000.0; // cost high end 
			const int quantity_low = 0; // quantity low end
			const int quantity_high = 10000; // quantity high end 
			const int colwidth = 15;

			for (;;) {
				cout << endl << left << setw(colwidth) << "Name: "; getline(cin, tree_name);

				// name must be alpha numeric, not empty and cannot be more than 100 characters 
				if (!valid_alpha_num(tree_name))
					continue; 

				if (!valid_empty(tree_name)) 
					continue;
				
				if (!valid_name_length(tree_name, colwidth*2))
					continue;

				else
					break; // error checking successful, exit loop
			}

			for (;;) {
				cout << endl << left << setw(colwidth) << "Cost: "; getline(cin,tree_cost);

				// cost must be a decimal digit num only 
				if (!valid_digits_cost((tree_cost)))
					continue;

				if (!valid_limits(stod(tree_cost), cost_low, cost_high))
					continue;

				else 
					break; // error checking successful, exit loop
				

			}

			for (;;) {
				cout << endl << left << setw(colwidth) << "Quantity: "; getline(cin,quantity);

				// check quantity to see that it is all digits
				if (!valid_digits(quantity))
					continue; 

				// limit checks, declared earlier in variable list 
				if (!valid_limits(stoi(quantity), quantity_low, quantity_high))
					continue; 

				else
					break; // error checking successful, exit loop
			}

			for (;;) {

				cout << endl << left << setw(colwidth) << "Machine (yes/no): "; getline(cin,machineStr);

				transform(machineStr.begin(), machineStr.end(), machineStr.begin(), ::tolower); // push to lower for simplicity

				if (!valid_empty(machineStr))
					continue;

				if (!valid_yes_no(machineStr))
					continue; 

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


			for (;;) {
				string answer = "";
				cout << endl << "Correct (yes/no)? "; cin >> answer;
				transform(answer.begin(), answer.end(), answer.begin(), ::tolower); // push answer str to lowercase for ease of use

				// check that input was actually yes or no
				if (!valid_yes_no(answer))
					continue; 
				else {
					if (answer == "yes") {	// user agreed to commit 

						const tuple<int, string, int, double, bool> insertDetails { 
							stoi(userInput), 
							tree_name, 
							stoi(tree_cost), 
							stod(quantity), 
							(machineStr == "yes" ? true : false) 
						};

						build_update(connObj, insertDetails);
						break;
					}
					else {
						cerr << "\n+++ UPDATE RECORD TERMINATED +++\n";
						break;
					}
				}
			} // end user confirmation
		
			clean_buffer();
			continue;
		} // end update


		/*************************************
		DELETE
		*************************************/

		// user wants to delete a record
		else if (userInput.substr(0, 6) == "delete" && userInput.length() >= 8) {

			// extract tree id from user input string 
			userInput.erase(userInput.begin(), userInput.begin() + 7); // trim delete chars off

			// check that the input was actually a digit 
			if (!valid_digits(userInput))
				continue; 

			// display selection to user
			cout << "\n +++ DELETE FOLLOWING RECORD +++\n";
			print_results(build_read_id(connObj,userInput));


			for (;;) {

				string answer = "";
				cout << endl << "Are you sure you want to delete (yes/no)? "; cin >> answer;
				transform(answer.begin(), answer.end(), answer.begin(), ::tolower); // push answer str to lowercase for ease of use

				if (!valid_yes_no(answer))
					continue; 
				else {
					if (answer == "yes") {	// user agreed to commit 
						build_delete(connObj, userInput);
						break;
					}
					else {
						cerr << "\n+++ DELETE RECORD TERMINATED +++\n";
						break;
					}
				}
			} // end user confirmation

			clean_buffer();
			continue;
		} // end delete

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
					cerr << "ERROR: File \"" << userInput << "\" could not be opened to write to! (permissions?) \n";
					continue;
				}
				else // build the report into the file 
					build_report(connObj, fileOut, userInput);
			}
			else
				build_report(connObj);

			clean_buffer();
			continue;
		} // end report
		
		/*************************************
		HELP
		*************************************/

		else if (userInput == "help") {
			const int colwidthR = 40;
			const int colwidthL = 25;

			
			cout << "\n+++ HELP MENU +++";
			print_hyphens(colwidthR + colwidthL);

			cout << endl << setw(colwidthL) << left << "create" << setw(colwidthR) << "create a new record for database";
			cout << endl << setw(colwidthL) << left << "delete tree_id" << setw(colwidthR) << "deletes records based on tree_id";
			cout << endl << setw(colwidthL) << left << "exit" << setw(colwidthR) << "exits the program";
			cout << endl << setw(colwidthL) << left << "read" << setw(colwidthR) << "displays all records ordered by id asc";
			cout << endl << setw(colwidthL) << left << "read -p" << setw(colwidthR) << "records ordered by cost asc";
			cout << endl << setw(colwidthL) << left << "read -P" << setw(colwidthR) << "records ordered by cost desc";
			cout << endl << setw(colwidthL) << left << "read -n" << setw(colwidthR) << "records ordered by name asc";
			cout << endl << setw(colwidthL) << left << "read -N" << setw(colwidthR) << "records ordered by name desc";
			cout << endl << setw(colwidthL) << left << "read -q" << setw(colwidthR) << "records ordered by quantity asc";
			cout << endl << setw(colwidthL) << left << "read -Q" << setw(colwidthR) << "records ordered by quantity desc";
			cout << endl << setw(colwidthL) << left << "report" << setw(colwidthR) << "generates a report of business questions";
			cout << endl << setw(colwidthL) << left << "report filename.txt" << setw(colwidthR) << "writes the report to given filename";
			cout << endl << setw(colwidthL) << left << "update tree_id" << setw(colwidthR) << "updates records based on tree_id";
			
			print_hyphens(colwidthR + colwidthL);

			cout << endl << "\nPress enter to continue.\n";

		}

		/*************************************
		EXIT PROGRAM
		*************************************/

		else if (userInput == "exit")
			return EXIT_SUCCESS;

		/*************************************
		INVALID INPUT
		*************************************/
		else {
			cerr << "ERROR: Input of \"" << userInput << "\" invalid. Enter help for options. Enter exit to close\n";
			continue;			
		}
		
		clean_buffer();
		connObj.close(); // close the connection 

	} // main cmd loop

	return 0;
}
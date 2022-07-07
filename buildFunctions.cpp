/*
buildFunctions.cpp
2022-06-09
Collin Abraham

-Contains function declarations from definitions in buildFunctions.h.
-these functions construct SQL statements and output the results to the user 

*/

#include "buildFunctions.h"

/* func used to connect to the database 
obviously in a production environment you wouldn't want the username and password in open text like this
but for learning/practice uses it was left like this. Modify to connect to your own postgres db */
const std::string conn_string() {
	return "host=localhost port=5432 dbname=TreeManagement user=postgres password=password";
}


/* helper func to print hypens to the screen, used with printResults() */
void print_hyphens(int width) {
	// print a row of hypens 
	std::cout << "\n";
	for (int i = 0; i < width; ++i)
		std::cout << "-";
}

/* Constructs a select statement to retrieve all rows orderd by the primary key id */
pqxx::result build_read(pqxx::connection& connObj) {
	pqxx::nontransaction workerN(connObj);
	return workerN.exec("SELECT * FROM trees ORDER BY tree_id");
}

/* Constructs a select statement to retrieve a single row based on id given */
pqxx::result build_read_id(pqxx::connection& connObj, const std::string& id) {
	pqxx::nontransaction workerN(connObj);
	return workerN.exec("SELECT * FROM trees WHERE tree_id = " + id);
}

/* Constructs a select statement to retrieve all rows ordred by name in ascending order */
pqxx::result build_read_name(pqxx::connection& connObj) {
	pqxx::nontransaction workerN(connObj);
	return workerN.exec("SELECT * FROM trees ORDER BY tree_name ASC");
}

/* Constructs a select statement to retrieve all rows ordred by name in descending order */
pqxx::result build_read_name_reversed(pqxx::connection& connObj) {
	pqxx::nontransaction workerN(connObj);
	return workerN.exec("SELECT * FROM trees ORDER BY tree_name DESC");
}

/* Constructs a select statement to retrieve all rows ordred by their cost in ascending order */
pqxx::result build_read_cost(pqxx::connection& connObj) {
	pqxx::nontransaction workerN(connObj);
	return workerN.exec("SELECT * FROM trees ORDER BY tree_cost ASC");
}

/* Constructs a select statement to retrieve all rows ordred by their cost in descending order */
pqxx::result build_read_cost_reversed(pqxx::connection& connObj) {
	pqxx::nontransaction workerN(connObj);
	return workerN.exec("SELECT * FROM trees ORDER BY tree_cost DESC ");
}

/* Constructs a select statement to retrieve all rows ordred by their quantity in ascending order */
pqxx::result build_read_quantity(pqxx::connection& connObj) {
	pqxx::nontransaction workerN(connObj);
	return workerN.exec("SELECT * FROM trees ORDER BY quantity ASC");
}

/* Constructs a select statement to retrieve all rows ordred by their quantity in descending order */
pqxx::result build_read_quantity_reversed(pqxx::connection& connObj) {
	pqxx::nontransaction workerN(connObj);
	return workerN.exec("SELECT * FROM trees ORDER BY quantity DESC");
}


//template<class ...T>
//void build_create(pqxx::connection& connObj, const std::tuple<T...>& insertDetails) {
//	
//	// query and store the most recent ID added so that this insert statement can auto increment 
//	pqxx::nontransaction workerN(connObj);
//	pqxx::result highestIdQuery = (workerN.exec("SELECT MAX(tree_id) FROM trees")); 
//	const int newId = std::stoi(highestIdQuery[0][0].c_str()) + 1; // conv the str to an int and increment by 1 for new ID
//	connObj.close();
//
//	
//	// build transactional worker
//	pqxx::connection tempConnObj(conn_string());
//	pqxx::work worker(tempConnObj);
//
//	// build the insert statement with a stringstream and the tuple argument 
//	std::stringstream ss; 
//	ss << "INSERT INTO trees (tree_id,tree_name,tree_cost,quantity,machine)";
//	ss << "VALUES (\'";
//	ss << newId;
//	ss << "\',\'";
//	ss << get<0>(insertDetails);
//	ss << "\',\'";
//	ss << get<1>(insertDetails);
//	ss << "\',\'";
//	ss << get<2>(insertDetails);
//	ss << "\',\'";
//	ss << (get<3>(insertDetails) == 1 ? "TRUE" : "FALSE");
//	ss << "\')"; // end of SQL statement
//
//	pqxx::result response = worker.exec(ss.str());
//	worker.commit();
//	std::cout << std::endl << "*** New Record successfully written to Database! ***" << std::endl;
//	
//}


//void build_update(pqxx::connection& connObj, const std::tuple<int, std::string, int, double, bool>& insertDetails) {
//
//	pqxx::work worker(connObj);
//
//	// build the update statement with a stringstream and the tuple argument 
//	std::stringstream ss;
//	ss << "UPDATE trees SET (tree_name,tree_cost,quantity,machine) = ";
//	ss << "(\'";
//	ss << get<1>(insertDetails);
//	ss << "\',\'";
//	ss << get<2>(insertDetails);
//	ss << "\',\'";
//	ss << get<3>(insertDetails);
//	ss << "\',\'";
//	ss << (get<4>(insertDetails) == 1 ? "TRUE" : "FALSE");
//	ss << "\')"; 
//	ss << "WHERE tree_id = ";
//	ss << get<0>(insertDetails); // end of SQL statement
//
//	pqxx::result response = worker.exec(ss.str());
//	worker.commit();
//	std::cout << std::endl << "*** New Record successfully updated to Database! ***" << std::endl;
//}

/* Cosntructs a delete statement to remove records based on primary key id given by user */
void build_delete(pqxx::connection& connObj, const std::string& id) {

	pqxx::work worker(connObj);

	// build the delete statement with a stringstream and the id
	std::stringstream ss;
	ss << "DELETE FROM trees WHERE tree_id = ";
	ss << id;

	pqxx::result response = worker.exec(ss.str());
	worker.commit();
	std::cout << std::endl << "*** Record successfully deleted from Database! ***" << std::endl;
}

/* Sends several queries to the database to answer a few hypothetically created business questions */
void build_report(pqxx::connection& connObj) {
	pqxx::nontransaction workerN(connObj);

	std::cout << "\n+++ REPORT STATUS +++\n";

	pqxx::result response;

	// Average cost 
	response = workerN.exec("SELECT round(AVG(tree_cost)::numeric, 2) FROM trees");
	std::cout << "\nAverage cost: " << response[0][0].as<std::string>() << std::endl;
	response.clear();

	// Total quantity of inventory
	response = workerN.exec("SELECT SUM(quantity) FROM trees");
	std::cout << "\nTotal quantity of inventory: " << response[0][0].as<std::string>() << std::endl;
	response.clear();

	// Total value of inventory
	response = workerN.exec("SELECT SUM(tree_cost*quantity) FROM trees");
	std::cout << "\nTotal value of inventory: " << response[0][0].as<std::string>() << std::endl;
	response.clear();

	// Most Expensive Product 
	response = workerN.exec("SELECT tree_id, tree_name, tree_cost FROM trees ORDER BY tree_cost DESC LIMIT 1 ");
	std::cout << "\nMost expensive product: \n"
		<< "\tID: " << response[0][0].as<std::string>() << std::endl
		<< "\tName: " << response[0][1].as<std::string>() << std::endl
		<< "\tCost: " << response[0][2].as<std::string>() << std::endl;
	response.clear();

	// Percentage of inventory that needs machines 
	response = workerN.exec("SELECT COUNT(machine) FROM trees WHERE machine = 'TRUE' ");
	const float machineYes = response[0][0].as<float>();
	response.clear();

	response = workerN.exec("SELECT COUNT(machine) FROM trees WHERE machine = 'FALSE'");
	const float machineNo = response[0][0].as<float>();
	response.clear();

	std::cout << "\nPercentage of trees that require machines: "
		<< std::setprecision(4) << (machineYes / (machineYes + machineNo)) * 100
		<< "%"
		<< std::endl;

	std::cout << "\nPress Enter to continue..." << std::endl;

}

/* Sends several queries to the database to answer a few hypothetically created business questions 
and outputs the results to a given filename.txt */
void build_report(pqxx::connection& connObj, std::ofstream& fileOut, const std::string& userInput) {
	pqxx::nontransaction workerN(connObj);

	fileOut << "\n+++ REPORT STATUS +++\n";
	pqxx::result response;

	// write date/time from db
	response = workerN.exec("SELECT CURRENT_DATE, CURRENT_TIME");
	fileOut << "\nGenerated at: " << response[0][0].as<std::string>() << " " << response[0][1].as<std::string>();
	response.clear();

	// Average cost 
	response = workerN.exec("SELECT round(AVG(tree_cost)::numeric, 2) FROM trees");
	fileOut << "\nAverage cost: " << std::setprecision(4) << response[0][0].as<std::string>() << std::endl;
	response.clear();

	// Total quantity of inventory
	response = workerN.exec("SELECT COUNT(quantity) FROM trees");
	fileOut << "\nTotal quantity of inventory: " << response[0][0].as<std::string>() << std::endl;
	response.clear();

	// Total value of inventory
	response = workerN.exec("SELECT SUM(tree_cost*quantity) FROM trees");
	fileOut << "\nTotal value of inventory: " << response[0][0].as<std::string>() << std::endl;
	response.clear();

	// Most Expensive Product 
	response = workerN.exec("SELECT tree_id, tree_name, tree_cost FROM trees ORDER BY tree_cost DESC LIMIT 1 ");
	fileOut << "\nMost expensive product: \n"
		<< "\tID: " << response[0][0].as<std::string>() << std::endl
		<< "\tName: " << response[0][1].as<std::string>() << std::endl
		<< "\tCost: " << response[0][2].as<std::string>() << std::endl;
	response.clear();

	// Percentage of inventory that needs machines 
	response = workerN.exec("SELECT COUNT(machine) FROM trees WHERE machine = 'TRUE' ");
	const float machineYes = response[0][0].as<float>();
	response.clear();

	response = workerN.exec("SELECT COUNT(machine) FROM trees WHERE machine = 'FALSE'");
	const float machineNo = response[0][0].as<float>();
	response.clear();

	fileOut << "\nPercentage of trees that require machines: "
		<< std::setprecision(4) << (machineYes / (machineYes + machineNo)) * 100
		<< "%"
		<< std::endl;

	if (fileOut.good())
		std::cout << "\nSuccessfully wrote report to " << userInput << "!\n";
	else
		std::cerr << "\nERROR: file writing to " << userInput << "was not successful!\n";

	// check to see if the file was created but is empty
	std::ifstream fileIn(userInput);
	std::stringstream ss;
	if (fileIn.is_open()) 
		while (!fileIn.eof())
			ss << fileIn.get();
	fileIn.close();
	
	// if nothing was extracted from the file, delete it from the filesystem so that no folder turd is left 
	if ((ss.str()).empty()) { 
		try {
			std::filesystem::remove(userInput);
		}
		catch (const std::filesystem::filesystem_error& e) {
			std::cout << "filesystem error: " << e.what() << '\n';
		}
	}	

	fileOut.close();
	std::cout << "\nPress enter to continue.\n";
}

/* Displays to the user the contents of a pqxx::result object created after executing 
one of the query statement funcs within this file */
void print_results(const pqxx::result& theResult) {
	
	const std::string separator{ "  |  " }; // cleaner output
	const int colWidth = 12; 
	const int rowWidth = 110;

	// give a row of hyphens for readability
	print_hyphens(rowWidth);
	std::cout << "\n";

	// print a row of col names
	std::cout << separator << std::right << std::setw(colWidth) << "Tree ID"
		<< separator << std::right << std::setw(colWidth * 2) << "Name"
		<< separator << std::right << std::setw(colWidth) << "Price"
		<< separator << std::right << std::setw(colWidth) << "Quantity"
		<< separator << std::right << std::setw(colWidth) << "Machine" << separator;

	// give a row of hyphens for readability
	print_hyphens(rowWidth);
	std::cout << "\n";

	// print a vertical row 
	for (size_t i = 0; i < theResult.size(); ++i) {

		std::cout << separator
			<< std::setw(colWidth) << std::right << theResult[i][0].c_str() << separator
			<< std::setw(colWidth * 2) << std::right << theResult[i][1].c_str() << separator
			<< std::setw(colWidth) << std::right << theResult[i][2].c_str() << separator
			<< std::setw(colWidth) << std::right << theResult[i][3].c_str() << separator
			<< std::setw(colWidth) << std::right << (theResult[i][4].as<std::string>() == "f" ? "no" : "yes") << separator
			<< std::endl;
	}
}
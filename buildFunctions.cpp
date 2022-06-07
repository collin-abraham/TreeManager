#include "buildFunctions.h"


const std::string connString() {
	return "host=localhost port=5432 dbname=TreeManagement user=postgres password=password";
}


// helper function to print hypens to the screen, used with the print func
void printHyphens(const int& width) {
	// print a row of hypens 
	for (int i = 0; i < width; ++i)
		std::cout << "-";
	std::cout << "\n";
}

pqxx::result buildRead(pqxx::connection& connObj) {
	pqxx::nontransaction workerN(connObj);
	return workerN.exec("SELECT * FROM trees ORDER BY tree_id");
}

pqxx::result buildReadId(pqxx::connection& connObj, const std::string& id) {
	pqxx::nontransaction workerN(connObj);
	return workerN.exec("SELECT * FROM trees WHERE tree_id = " + id);
}

pqxx::result buildReadName(pqxx::connection& connObj) {
	pqxx::nontransaction workerN(connObj);
	return workerN.exec("SELECT * FROM trees ORDER BY tree_name ASC");
}

pqxx::result buildReadNameReversed(pqxx::connection& connObj) {
	pqxx::nontransaction workerN(connObj);
	return workerN.exec("SELECT * FROM trees ORDER BY tree_name DESC");
}

pqxx::result buildReadCost(pqxx::connection& connObj) {
	pqxx::nontransaction workerN(connObj);
	return workerN.exec("SELECT * FROM trees ORDER BY tree_cost ASC");
}

pqxx::result buildReadCostReversed(pqxx::connection& connObj) {
	pqxx::nontransaction workerN(connObj);
	return workerN.exec("SELECT * FROM trees ORDER BY tree_cost DESC ");
}

pqxx::result buildReadQuantity(pqxx::connection& connObj) {
	pqxx::nontransaction workerN(connObj);
	return workerN.exec("SELECT * FROM trees ORDER BY quantity ASC");
}

pqxx::result buildReadQuantityReversed(pqxx::connection& connObj) {
	pqxx::nontransaction workerN(connObj);
	return workerN.exec("SELECT * FROM trees ORDER BY quantity DESC");
}

void buildCreate(pqxx::connection& connObj, const std::tuple<std::string, int, double, bool>& insertDetails) {
	
	// query and store the most recent ID added so that this insert statement can auto increment 
	pqxx::nontransaction workerN(connObj);
	pqxx::result highestIdQuery = (workerN.exec("SELECT MAX(tree_id) FROM trees")); 
	int newId = std::stoi(highestIdQuery[0][0].c_str()) + 1; // conv the str to an int and increment by 1 for new ID

	// build transactional worker
	pqxx::work worker(connObj);

	// build the insert statement with a stringstream and the tuple argument 
	std::stringstream ss; 
	ss << "INSERT INTO trees (tree_id,tree_name,tree_cost,quantity,machine)";
	ss << "VALUES (\'";
	ss << newId;
	ss << "\',\'";
	ss << get<0>(insertDetails);
	ss << "\',\'";
	ss << get<1>(insertDetails);
	ss << "\',\'";
	ss << get<2>(insertDetails);
	ss << "\',\'";
	ss << (get<3>(insertDetails) == 1 ? "TRUE" : "FALSE");
	ss << "\')"; // end of SQL statement

	pqxx::result response = worker.exec(ss.str());
	worker.commit();
	std::cout << std::endl << "*** New Record successfully written to Database! ***" << std::endl;
	
}


void buildUpdate(pqxx::connection& connObj, const std::tuple<int, std::string, int, double, bool>& insertDetails) {

	pqxx::work worker(connObj);

	// build the update statement with a stringstream and the tuple argument 
	std::stringstream ss;
	ss << "UPDATE trees SET (tree_name,tree_cost,quantity,machine) = ";
	ss << "(\'";
	ss << get<1>(insertDetails);
	ss << "\',\'";
	ss << get<2>(insertDetails);
	ss << "\',\'";
	ss << get<3>(insertDetails);
	ss << "\',\'";
	ss << (get<4>(insertDetails) == 1 ? "TRUE" : "FALSE");
	ss << "\')"; 
	ss << "WHERE tree_id = ";
	ss << get<0>(insertDetails); // end of SQL statement

	pqxx::result response = worker.exec(ss.str());
	worker.commit();
	std::cout << std::endl << "*** New Record successfully updated to Database! ***" << std::endl;
}


void buildDelete(pqxx::connection& connObj, const std::string& id) {

	pqxx::work worker(connObj);

	// build the delete statement with a stringstream and the id
	std::stringstream ss;
	ss << "DELETE FROM trees WHERE tree_id = ";
	ss << id;

	pqxx::result response = worker.exec(ss.str());
	worker.commit();
	std::cout << std::endl << "*** Record successfully deleted from Database! ***" << std::endl;
}


void buildReport(pqxx::connection& connObj) {
	pqxx::nontransaction workerN(connObj);

	std::cout << "\n+++ REPORT STATUS +++\n";

	pqxx::result response;

	// Average cost 
	response = workerN.exec("SELECT AVG(tree_cost) FROM trees");
	std::cout << "\nAverage cost: " << std::setprecision(4) << response[0][0].as<float>() << std::endl;
	response.clear();

	// Total quantity of inventory
	response = workerN.exec("SELECT COUNT(quantity) FROM trees");
	std::cout << "\nTotal quantity of inventory: " << response[0][0].as<std::string>() << std::endl;
	response.clear();

	// Total value of inventory
	response = workerN.exec("SELECT SUM(tree_cost) FROM trees");
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
		<< std::setprecision(4) << (machineYes / machineNo) * 100
		<< "%"
		<< std::endl;

	std::cout << "\nPress Enter to continue..." << std::endl;
}

void buildReport(pqxx::connection& connObj, std::ofstream& fileOut, const std::string& userInput) {
	pqxx::nontransaction workerN(connObj);

	fileOut << "\n+++ REPORT STATUS +++\n";
	pqxx::result response;

	// write date/time from db
	response = workerN.exec("SELECT CURRENT_DATE, CURRENT_TIME");
	fileOut << "\nGenerated at: " << response[0][0].as<std::string>() << " " << response[0][1].as<std::string>();
	response.clear();

	// Average cost 
	response = workerN.exec("SELECT AVG(tree_cost) FROM trees");
	fileOut << "\nAverage cost: " << std::setprecision(4) << response[0][0].as<float>() << std::endl;
	response.clear();

	// Total quantity of inventory
	response = workerN.exec("SELECT COUNT(quantity) FROM trees");
	fileOut << "\nTotal quantity of inventory: " << response[0][0].as<std::string>() << std::endl;
	response.clear();

	// Total value of inventory
	response = workerN.exec("SELECT SUM(tree_cost) FROM trees");
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
		<< std::setprecision(4) << (machineYes / machineNo) * 100
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
			if (std::filesystem::remove(userInput))
				std::cout << "file " << userInput << " deleted.\n";
			else
				std::cout << "file " << userInput << " not found.\n";
		}
		catch (const std::filesystem::filesystem_error& err) {
			std::cout << "filesystem error: " << err.what() << '\n';
		}
	}	

	fileOut.close();
	std::cout << "\nPress enter to continue.\n";
}

void printResults(const pqxx::result& theResult) {
	
	const std::string seperator{ "  |  " }; // cleaner output
	const int colWidth = 12; 
	const int rowWidth = 110;

	// give a row of hyphens for readability
	printHyphens(rowWidth);

	// print a row of col names
	std::cout << seperator << std::setw(colWidth) << "Tree ID"
		<< seperator << std::setw(colWidth * 2) << "Name"
		<< seperator << std::setw(colWidth) << "Price"
		<< seperator << std::setw(colWidth) << "Quantity"
		<< seperator << std::setw(colWidth) << "Machine" << seperator 
		<< std::endl; 

	// give a row of hyphens for readability
	printHyphens(rowWidth);

	// print a vertical row 
	for (size_t i = 0; i < theResult.size(); ++i) {

		std::cout << seperator
			<< std::setw(colWidth) << theResult[i][0].c_str() << seperator
			<< std::setw(colWidth * 2) << theResult[i][1].c_str() << seperator
			<< std::setw(colWidth) << theResult[i][2].c_str() << seperator
			<< std::setw(colWidth) << theResult[i][3].c_str() << seperator
			<< std::setw(colWidth) << (theResult[i][4].as<std::string>() == "f" ? "no" : "yes") << seperator
			<< std::endl;
	}
}
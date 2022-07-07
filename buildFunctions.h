/*
buildFunctions.h
2022-06-09
Collin Abraham

- Function definitions used to construct + send SQL statements to db 
- templated funcs have definition and declaration within this file (wanted to experiment with variadic functions) 
  for build_create() and build_update()

*/

#pragma once

#include "commandloop.h" // all necessary libraries found within commandloop.h

const std::string conn_string();

pqxx::result build_read(pqxx::connection& connObj);

pqxx::result build_read_id(pqxx::connection& connObj, const std::string& id);

pqxx::result build_read_name(pqxx::connection& connObj);

pqxx::result build_read_name_reversed(pqxx::connection& connObj);

pqxx::result build_read_cost(pqxx::connection& connObj);

pqxx::result build_read_cost_reversed(pqxx::connection& connObj);

pqxx::result build_read_quantity(pqxx::connection& connObj);

pqxx::result build_read_quantity_reversed(pqxx::connection& connObj);

/* Constructs an insert statement to write a new record into the db */
template<class ...TUPLE_TYPES>
void build_create(pqxx::connection& connObj, const std::tuple<TUPLE_TYPES...>& insertDetails) {

	// query and store the most recent ID added so that this insert statement can auto increment 
	pqxx::nontransaction workerN(connObj);
	pqxx::result highestIdQuery = (workerN.exec("SELECT MAX(tree_id) FROM trees"));
	const int newId = std::stoi(highestIdQuery[0][0].c_str()) + 1; // conv the str to an int and increment by 1 for new ID
	connObj.close();


	// build transactional worker
	pqxx::connection tempConnObj(conn_string());
	pqxx::work worker(tempConnObj);

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

/* Constructs an update statement to change a record in the database based on user inputs */
template<class ...TUPLE_TYPES>
void build_update(pqxx::connection& connObj, const std::tuple<TUPLE_TYPES...>& insertDetails) {
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

void build_delete(pqxx::connection& connObj, const std::string& id);

void build_report(pqxx::connection& connObj);

void build_report(pqxx::connection& connObj, std::ofstream& fileOut, const std::string& userInput);

void print_results(const pqxx::result& theResult);

void print_hyphens(int width); 




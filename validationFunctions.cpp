/*
validationFunctions.cpp
2022-06-09
Collin Abraham

-Function definitions of validationFunctions.h
-These functions are used to validate user input and are generally self explanitory within their naming 
*/

#include "validationFunctions.h"

using namespace std;

/* checks that the string contains only digits, breaks the loop if not */
bool valid_digits(const string& str) {

	for (const auto& c : str) {
		if (!isdigit(c)) {	// see if we encounter something that isn't a digit
			std::cerr << "ERROR: Input failed, must be digits only!\n";
			return false;
		}
	}

	return true;
}

/* checks that the string contains only digits, breaks the loop if not */

bool valid_digits_cost(const string& cost) {

	//string temp = to_string(cost); // conv to string to check contents 
	for (const auto& c : cost) {
		if (!isdigit(c) && c != '.') {	// digits and decimal point allowed 
			std::cerr << "ERROR: Input failed, must be digits only!\n";
			return false;
		}
	}

	return true;
}

/* checks that the string contains only alphabetical and digit chars, breaks the loop if not*/
bool valid_alpha_num(const string& str) {

	for (const auto& c : str) {
		if (!isalnum(c) && c != '\'' && c != '-' && c != ' ') {	// allow for apostrophies, hyphens and spaces
			cerr << endl << "ERROR: Input failed, must be alphanumeric (letters/numbers)!\n";
			return false;
		}
	}
	
	return true;
}

/* string must not be empty */
bool valid_empty(const string& str) {

	if (str == "") {
		cerr << "ERROR: Input cannot be empty!\n";
		return false; 
	}
	
	return true;
}

/* length of the string must fall within the specific value */
bool valid_name_length(const string& str, int len) {

	if (str.length() > len) {
		cerr << "ERROR: Input cannot be more than " << len << " characters\n";
		return false;
	}

	return true;
}

/* string must explicitly equal "yes" or "no" */
bool valid_yes_no(const string& str) {
	if (str != "yes" && str != "no") {
		cerr << endl << "ERROR: Input must be 'yes' or 'no' cannot be: \"" << str << "\"!\n";
		return false;
	}

	return true; 
}

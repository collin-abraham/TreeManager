/*
validationFunctions.h
2022-06-09
Collin Abraham

-Function declarations for validation procedures

*/

#pragma once

#include "commandloop.h"

bool valid_digits(const std::string& str);

bool valid_digits_cost(const std::string& cost);

bool valid_alpha_num(const std::string& str);

bool valid_empty(const std::string& str);

bool valid_name_length(const std::string& str, int len);

bool valid_yes_no(const std::string& str);

/* While compiling with the latest version of C++ thru Visual Studio.. was unable to detach declaration from definition on a 
templated funcion. Had to run latest version to guarantee that libpqxx would be functional, as well. Not 100% sure why it creates 
a linker error having them seperate */
template<class NUMTYPE> 
bool valid_limits(NUMTYPE num, NUMTYPE low, NUMTYPE high) {
	if (num <= low || num > high) {
		std::cerr << std::endl << "ERROR: Input must be between " << low << " and " << high << "!\n";
		return false;
	}

	return true;
}

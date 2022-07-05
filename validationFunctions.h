/*
validationFunctions.h
2022-06-09
Collin Abraham

-Function declarations for validation procedures

*/

#pragma once

#include "commandloop.h"

bool validDigits(const std::string& str);
bool validDigitsCost(double cost);
bool validAlphaNum(const std::string& str);
bool validEmpty(const std::string& str);
bool validNameLength(const std::string& str, int len);
bool validYesNo(const std::string& str);

/* Due to being a generic function, both decl and def had to be in the same file to avoid a LNK2001 error within visual studio.
This function takes in a value, upper limit and lower limit all of type <T>. It ensures that the value falls within that limit */
template<class NUMTYPE>
bool validLimits(NUMTYPE num, NUMTYPE low, NUMTYPE high) {
	if (num <= low || num > high) {
		std::cerr << std::endl << "ERROR: Input must be between " << low << " and " << high << "!\n";
		return false;
	}

	return true;
}

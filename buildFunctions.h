/*
buildFunctions.h
2022-06-09
Collin Abraham

- Function definitions used to construct + send SQL statements to db 

*/

#pragma once

#include "commandloop.h" // all necessary libraries found within commandloop.h

pqxx::result buildRead(pqxx::connection& connObj);
pqxx::result buildReadId(pqxx::connection& connObj, const std::string& id);
pqxx::result buildReadName(pqxx::connection& connObj);
pqxx::result buildReadNameReversed(pqxx::connection& connObj);
pqxx::result buildReadCost(pqxx::connection& connObj);
pqxx::result buildReadCostReversed(pqxx::connection& connObj);
pqxx::result buildReadQuantity(pqxx::connection& connObj);
pqxx::result buildReadQuantityReversed(pqxx::connection& connObj);

void buildCreate(pqxx::connection& connObj, const std::tuple<std::string, int, double, bool>& insertDetails);

void buildUpdate(pqxx::connection& connObj, const std::tuple<int, std::string, int, double, bool>& insertDetails);

void buildDelete(pqxx::connection& connObj, const std::string& id);

void buildReport(pqxx::connection& connObj);
void buildReport(pqxx::connection& connObj, std::ofstream& fileOut, const std::string& userInput);

void printResults(const pqxx::result& theResult);
void printHyphens(int width); 

const std::string connString();


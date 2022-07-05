/*
commandLoop.h
2022-06-09
Collin Abraham

-Library inclusions necessary for functions
-Function declarations 

*/

#pragma once

#include <iostream>
#include <string>
#include <tuple>
#include <pqxx/pqxx>
#include <algorithm> // transform ... to_lower
#include <iomanip>
#include <limits>
#include <fstream>
#include <filesystem>

#include "buildFunctions.h"


void cleanBuffer();

int commandLoop();

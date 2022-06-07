#pragma once
#include <iostream>
#include <string>
#include <tuple>
#include <pqxx/pqxx>
#include <algorithm> // transform ... to_lower
#include <iomanip>
#include <limits>
#include <fstream>

#include "buildFunctions.h"

void cleanBuffer();
int commandLoop();

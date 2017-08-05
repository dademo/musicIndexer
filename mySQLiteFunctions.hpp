#ifndef MYSQLITEFUNCTIONS
#define MYSQLITEFUNCTIONS

#include "sqlite3.h"
#include <string>

int sqliteReturnVal(int returnVal, const char* err);

int idResearch(sqlite3* db, std::string dbName, std::string paramName, std::string paramValue);

#endif

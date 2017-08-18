#ifndef MYSQLITEFUNCTIONS
#define MYSQLITEFUNCTIONS

#include "sqlite3.h"
#include <string>

//int sqliteReturnVal(int returnVal, const char* err);
int sqliteReturnVal(int returnVal);

int idResearch(sqlite3* db, std::string dbName, std::string paramName, std::string paramValue);

int getnResults(sqlite3* db, std::string request);	// Count the number of results in the request
int delElementFromDb(sqlite3* db, std::string request);	// To execute delete request

#endif

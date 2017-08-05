#include <iostream>
#include <unistd.h>

#include "sqlite3.h"
#include "mySQLiteFunctions.hpp"

int sqliteReturnVal(int returnVal, const char* err)
{
	switch(returnVal)
	{
	case SQLITE_OK:
		return SQLITE_OK;
	case SQLITE_ROW:
		return SQLITE_ROW;
	case SQLITE_DONE:
		return SQLITE_DONE;
		/* Execution error */
	case SQLITE_ERROR:
		std::cerr << "Error reported" << ((err != 0)? std::string(" (") + err + ")": "") << "... Aborting" << std::endl;
		return SQLITE_ERROR;
	case SQLITE_ABORT:
		std::cerr << "General error... Operation aborted" << std::endl;
		return SQLITE_ABORT;
	case SQLITE_LOCKED:
		std::cerr << "Database locked (can't write, conflict with the same connexion)..." << std::endl;
		return SQLITE_LOCKED;	
	case SQLITE_BUSY:
		std::cerr << "Database locked (can't write, conflict with another connexion)..." << std::endl;
		return SQLITE_BUSY;
	case SQLITE_PERM:
		std::cerr << "Can't get permission for the operation... Aborting" << std::endl;
		return SQLITE_PERM;
	case SQLITE_TOOBIG:
		std::cerr << "Requested operation is too big... Aborting" << std::endl;
		return SQLITE_TOOBIG;
	case SQLITE_MISUSE:
		std::cerr << "SQLite interface undefined or unsupported (ex: statement used after finalized)... Aborting" << std::endl;
		return SQLITE_MISUSE;
		/* SQLITE database file error */
	case SQLITE_NOTADB:
		std::cerr << "Opened file is not a database... Aborting" << std::endl;
		return SQLITE_NOTADB;
	case SQLITE_CORRUPT:
		std::cerr << "Corrupted database... Aborting" << std::endl;
		return SQLITE_CORRUPT;
	case SQLITE_CANTOPEN:
		std::cerr << "Can't open database... Aborting" << std::endl;
		return SQLITE_CANTOPEN;
	case SQLITE_FULL:
		std::cerr << "Disk full, can't write... Aborting" << std::endl;
		return SQLITE_FULL;
	case SQLITE_IOERR:
		std::cerr << "I/O error... Aborting" << std::endl;
		return SQLITE_IOERR;
	case SQLITE_READONLY:
		std::cerr << "Database is read only... Aborting" << std::endl;
		return SQLITE_READONLY;
		/* Memory errors */
	case SQLITE_NOMEM:
		std::cerr << "SQLite3 can't allocate memory... Aborting" << std::endl;
		return SQLITE_NOMEM;
        /* Request errors */
	case SQLITE_RANGE:
		std::cerr << "sqlite3_bind out of range OR sqlite3_column out of range... Aborting" << std::endl;
		return SQLITE_RANGE;
	default:
		std::cerr << "Untreated value (" + std::to_string(returnVal) + ")" << std::endl;
		return returnVal;
	}
}


int idResearch(sqlite3* db, std::string dbName, std::string paramName, std::string paramValue)
{
	sqlite3_stmt* requestStatement;
	int returnVal = 0;
	int id = 0;
	std::string request = "\
	SELECT " + dbName + ".id \
	FROM " + dbName + " \
	WHERE " + dbName + "." + paramName + "=?";

	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, request.c_str(), -1, &requestStatement, 0), 0);
 
	if(returnVal != SQLITE_OK) { return -1; }
	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, paramValue.c_str(), -1, SQLITE_STATIC), 0) != SQLITE_OK) { return -1; };

	do{
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement), 0);
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_DONE && returnVal != SQLITE_BUSY) { return -1; }
		if(returnVal == SQLITE_ROW)
		{
			id = sqlite3_column_int(requestStatement, 0);
		}
		if(returnVal == SQLITE_BUSY) { sleep(1); }

	} while (returnVal != SQLITE_DONE);

	sqliteReturnVal(sqlite3_finalize(requestStatement), 0);

	return id;
}

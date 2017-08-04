#include <iostream>

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
		std::cerr << "Error reported (" << err << ")... Aborting" << std::endl;
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
	default:
		std::cerr << "Untreated value" << std::endl;
		return returnVal;
	}
}

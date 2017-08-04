#include <iostream>
#include <string>

#include "mySQLiteFunctions.hpp"
#include "music_sqlTables.hpp"

int createTables(sqlite3* db)
{
	sqlite3_stmt* requestStatement;
	int returnVal = 0;

	const char* err;	// Uncompiled part

	std::string tablesRequest[5] = {"\
CREATE TABLE IF NOT EXISTS \"albums\" (\n\
	`id`	INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,\n\
	`artist`	INTEGER,\n\
	`name`	TEXT,\n\
	`ntrack`	INTEGER,\n\
	`cover`	BLOB,\n\
	`year`	INTEGER\n\
);\n",
"CREATE TABLE IF NOT EXISTS \"genres\" (\n\
	`id`	INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,\n\
	`name`	TEXT\n\
);\n",
"CREATE TABLE IF NOT EXISTS \"artists\" (\n\
	`id`	INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,\n\
	`name`	TEXT\n\
);\n",
"CREATE TABLE IF NOT EXISTS \"directories\" (\n\
	`id`	INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,\n\
	`path`	TEXT\n\
);\n",
"CREATE TABLE IF NOT EXISTS \"songs\" (\n\
	`id`	INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,\n\
	`id_album`	INTEGER	/*Foreign key for table album->id*/,\n\
	`id_genre`	INTEGER	/*Foreign key for table genre->id*/,\n\
	`id_artist`	INTEGER	/*Foreign key for table artist->id*/,\n\
	`id_dirName`	INTEGER	/*Foreign key for table dirname->id*/,\n\
	`path`	TEXT		/*Name of the file*/,\n\
	`name`	TEXT		/*Name of the track*/,\n\
	`tracknbr`	INTEGER	/*Number of tracks*/\n\
);" };

	for(int i = 0; i < 5; i++)
	{
		// Creating the request
		returnVal = sqlite3_prepare_v2(db, tablesRequest[i].c_str(), -1, &requestStatement, &err);
		if(sqliteReturnVal(returnVal, err) != SQLITE_OK) { return returnVal; }

		//std::cout << err << std::endl;

		returnVal = 0;

		while (returnVal != SQLITE_DONE)
		{
			returnVal = sqliteReturnVal(returnVal = sqlite3_step(requestStatement), 0);
			if( returnVal != SQLITE_DONE && returnVal != SQLITE_BUSY && returnVal != SQLITE_ROW )
			{ return returnVal; }
		}
		std::cout << "[" << i << "] Request OK" << std::endl;

		// Finalization of requiestStatement
		returnVal = sqliteReturnVal(sqlite3_finalize(requestStatement), 0);
		if(returnVal != SQLITE_OK) { return returnVal; }
	}
	return returnVal;
	//return SQLITE_OK;
}


// Checking the db tables //
struct colInfo {
	std::string colName;	// Name of the column
	int dataType;		// From SQLite formats
	bool primKey;		// Is a primary key ?
};

int checkTablecolsNames(sqlite3* db, std::string tableName, int nCol, struct colInfo allCols[]);
int strToSQLiteDataType(std::string dataType);
void resetTable(sqlite3* db, std::string tableName);

void checkTables(sqlite3* db)
{
	int colResult = 0;
	struct colInfo allCols[][8] = {
	{	// Albums table
		{"id", SQLITE_INTEGER, true},
		{"artist", SQLITE_INTEGER, false},
		{"name", SQLITE_TEXT, false},
		{"ntrack", SQLITE_INTEGER, false},
		{"cover", SQLITE_BLOB, false},
		{"year", SQLITE_INTEGER, false}
	},{	// Artists table
		{"id", SQLITE_INTEGER, true},
		{"name", SQLITE_TEXT, false}
	},{	// Directories table
		{"id", SQLITE_INTEGER, true},
		{"path", SQLITE_TEXT, false}
	},{	// Genres table
		{"id", SQLITE_INTEGER, true},
		{"name", SQLITE_TEXT, false}
	},{	// Songs table
		{"id", SQLITE_INTEGER, true},
		{"id_album", SQLITE_INTEGER, false},
		{"id_genre", SQLITE_INTEGER, false},
		{"id_artist", SQLITE_INTEGER, false},
		{"id_dirName", SQLITE_INTEGER, false},
		{"path", SQLITE_TEXT,false},
		{"name", SQLITE_TEXT, false},
		{"tracknbr", SQLITE_INTEGER, false}
	}};

	std::string allTablesNames[] = {
	"albums",
	"artists",
	"directories",
	"genres",
	"songs"
	};
	int elemCount[] = {6,2,2,2,8};

	for(int i = 0; i < 5; i++)
	{
		colResult = checkTablecolsNames(db, allTablesNames[i], elemCount[i], allCols[i]);
		if(colResult > 0) { std::cout << "Error n°" << colResult << std::endl; resetTable(db, allTablesNames[i]); }
		else if(colResult < 0)
		{
			//std::cout << "ColResult : " << colResult << std::endl;
			if(colResult <=(-100))
			{ std::cout << "Table [" << allTablesNames[i] << "]\t Problem at loop n°" << -(colResult + 100) << std::endl; resetTable(db, allTablesNames[i]); }
			else
			{ std::cout << "Table [" << allTablesNames[i] << "]\t Problem with column n°" << -(colResult) << std::endl; resetTable(db, allTablesNames[i]); }
		}
		else { std::cout << "Table [" << allTablesNames[i] << "]\t OK !" << std::endl; }
	}
}


int checkTablecolsNames(sqlite3* db, std::string tableName, int nCol, struct colInfo allCols[])
{
	sqlite3_stmt* requestStatement;
	std::string request = "PRAGMA table_info(" + tableName + ");"; // Reply : ID | colName | colType | (Can be nul or not) | default_val | (index of the column in the primary key column) //
	int returnVal = 0;	// Value returned for each sqlite3 functions
	int i = 0;		// For the result loop

	returnVal = sqlite3_prepare_v2(db, request.c_str(), -1, &requestStatement, 0);
	if(sqliteReturnVal(returnVal, 0) != SQLITE_OK) { return returnVal; }

	while(returnVal != SQLITE_DONE)
	{
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement),0);
		if(returnVal == SQLITE_DONE && i == 0) { return -1; }	// If there's no result, so there's no table
		//if(returnVal != SQLITE_BUSY && returnVal != SQLITE_ROW && returnVal != SQLITE_DONE)
		if(returnVal != SQLITE_BUSY && returnVal != SQLITE_ROW && returnVal != SQLITE_DONE)
		{ return returnVal; }

		if(returnVal == SQLITE_ROW)	// There's data !
		{
			// Note : if there's an error, we call "reset" to free the database access
			if(i > nCol) { sqliteReturnVal(sqlite3_reset(requestStatement), 0); return -i-100; }
			if(std::string((char*)sqlite3_column_text(requestStatement, 1)) != allCols[i].colName) { sqliteReturnVal(sqlite3_reset(requestStatement), 0); return -1; }	// Problem with the name column
			if(strToSQLiteDataType(std::string( (const char*) sqlite3_column_text(requestStatement, 2))) != allCols[i].dataType) { sqliteReturnVal(sqlite3_reset(requestStatement), 0); return -2; }	// Problem with the data type
			if((sqlite3_column_int(requestStatement, 5) != 0) != allCols[i].primKey) { sqliteReturnVal(sqlite3_reset(requestStatement), 0); return -5; }	// Problem with the primary key info
			i++;
		}
	}

	sqliteReturnVal(sqlite3_finalize(requestStatement), 0);

	return 0;
}

int strToSQLiteDataType(std::string dataType)
{
	if(dataType == "TEXT")		{ return SQLITE_TEXT; }
	if(dataType == "INTEGER")	{ return SQLITE_INTEGER; }
	if(dataType == "REAL")		{ return SQLITE_FLOAT; }
	if(dataType == "NUMERIC")	{ return SQLITE_FLOAT; }	// Can be integer or real
	if(dataType == "BLOB")		{ return SQLITE_BLOB; }

	return 0;
}

void resetTable(sqlite3* db, std::string tableName)
{
	char* errmsg = 0;
	std::cout << "Resetting the table [" << tableName << "] ..." << std::endl;
	// Deletting the table
	sqliteReturnVal(sqlite3_exec(db, ("DROP TABLE IF EXISTS " + tableName + ";").c_str(), 0, 0, &errmsg), errmsg);

	std::cout << "OK !" << std::endl;

	createTables(db);
} 

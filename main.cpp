#include <iostream>
#include <vector>

#include "sqlite3.h"
#include "music_sqlTables.hpp"
#include "fileManip.hpp"

#define DBNAME "music.db"

int main (int argc, char* argv[])
{
	sqlite3 *db;
	int rc;
	//char* err;

	rc = sqlite3_open(DBNAME, &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	else { std::cout << "rc = " << rc << std::endl; }

	// Creating the database if not exist

	/*char request[] = "CREATE TABLE IF NOT EXISTS testTable (id INTEGER PRIMARY KEY AUTOINCREMENT, data TEXT)";
	sqlite3_stmt* requestStatement;
	
	int returnVal = sqlite3_prepare_v2(db,(const char*) request, -1, &requestStatement, (const char**)  &err);
	if(returnVal == SQLITE_OK)
	{
		returnVal = sqlite3_step(requestStatement);
		if(returnVal == SQLITE_DONE)		{ std::cout << " Request OK (SQLITE_DONE) !" << std::endl; }
		else if (returnVal == SQLITE_ROW)	{ std::cout << " Request OK (SQLITE_ROW) !" << std::endl; }
		else { std::cout << "Error(2) : " << returnVal << std::endl; }
	}
	else { std::cout << "Error(1) : " << returnVal << std::endl << "Error : " << err << std::endl;; }
	if( sqlite3_finalize(requestStatement) == SQLITE_OK) { std::cout << "Finalize : OK !" << std::endl; }*/

	//std::cout << "Hello world !" << std::endl;

	//createTables(db);

	checkTables(db);

	std::vector<std::string> fileList = getFileList("/home/dademo/Musique", false);

	for(std::vector<std::string>::iterator it = fileList.begin(); it != fileList.end(); it++)
	{
		std::cout << *it << std::endl;
	}


	sqlite3_close(db);

	return 0;
}

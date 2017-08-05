#include <iostream>
#include <vector>

#include "sqlite3.h"
#include "music_sqlTables.hpp"
#include "fileManip.hpp"
#include "tagfcts.hpp"

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
		return 1;
	}
	else
	{
	
		checkTables(db);

		std::vector<std::string> fileList = getFileList("/home/dademo/Musique", false);

		//TagInfos maMusique("/home/dademo/Musique/Flac/Daft Punk/Musique Vol. 1 (1993-2005)/1 - Daft Punk - Musique.flac");
		TagInfos maMusique("/home/dademo/Musique/Flac/Daft Punk/Random Access Memories/1 - Daft Punk - Give Life Back to Music.flac");
	/*
		for(std::vector<std::string>::iterator it = fileList.begin(); it != fileList.end(); it++)
		{
			std::cout << *it << std::endl;
		}
	*/

		std::cout << maMusique.toString() << std::endl;


		maMusique.sync(db);

		sqlite3_close(db);

		return 0;
	}
}

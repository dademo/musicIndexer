#include <iostream>
#include <map>
#include <vector>
#include <stdlib.h>	// atoi()

// TEMP //
#include <unistd.h>

#include "sqlite3.h"
#include "music_sqlTables.hpp"
#include "fileManip.hpp"
#include "fct_utiles.hpp"
#include "tagfcts.hpp"
#include "class_tagInfos.hpp"
#include "class_asyncTagInfos.hpp"

#define DBNAME "/tmp/music.db"


int callbackFct(void* db, int ncols, char** stringResults, char** colsNames);

int main (int argc, char* argv[])
{
	std::string origPath = "/home/dademo/Musique";
	//std::string origPath = "/home/dademo/Musique/Daft.Punk.Discographie.FLAC-NoTag";
	sqlite3 *db;
	int rc;
	//char* err;

	// Configuring the  sqlite3 library for serialized mode -> multithreading with the same db connexion : https://sqlite.org/threadsafe.html
	rc = sqlite3_config(SQLITE_CONFIG_SERIALIZED);
	if( rc ){
		fprintf(stderr, "Can't configure SQLITE3 library: %s\n", sqlite3_errstr(rc));
		return 1;
	}

	rc = sqlite3_open(DBNAME, &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}
	else
	{
	
		//TagInfos maMusique("/home/dademo/Musique/Playlist Wave.m3u");

		checkTables(db);


		//std::vector<TagInfos> allMyTags = fullGetAllTags(origPath);

		std::vector<TagInfos> allMyTags = fastGetAllTags(origPath);
		
		for(std::vector<TagInfos>::iterator it = allMyTags.begin(); it != allMyTags.end(); it++)
		{
			//std::cout << it->toString() << std::endl;
			//std::cout << "Sycing : "  << " [" << it->getData().album.name << "]\t" << it->getData().name<< std::endl;
			it->sync(db);
		}
/*
		std::vector<ASyncTagInfos> allMyTags = fastGetAllTags_ASync(origPath);
		for(std::vector<ASyncTagInfos>::iterator it = allMyTags.begin(); it != allMyTags.end(); it++)
		{
			it->sync(db);
		}
*/

/*		struct songInfos dataInfos = genVoidStructSongInfos();

		dataInfos.name="Contact";
		dataInfos.artist="Daft Punk";
		dataInfos.tracknbr=13;
		dataInfos.album.name="Random%";

		//struct audioProperties dataProperties = {1,2,3,4};
		struct audioProperties dataProperties = genVoidStructAudioProperties();

		std::vector<TagInfos> allResults = TagInfos::searchTagInfos(db, dataInfos, dataProperties);

		for(std::vector<TagInfos>::iterator it = allResults.begin(); it != allResults.end(); it++)
		{
			std::cout << it->toString() << std::endl;
		}

		std::vector<TagInfos> allResults2 = TagInfos::searchTagInfos(db, allResults[0].getData(), dataProperties);
		for(std::vector<TagInfos>::iterator it = allResults2.begin(); it != allResults2.end(); it++)
		{
			std::cout << it->toString() << std::endl;
		}

		struct songInfos toSearch = genVoidStructSongInfos();
		toSearch.name="C%";
		std::vector<TagInfos> allResults3 = TagInfos::searchTagInfos(db, toSearch, genVoidStructAudioProperties());
		for(std::vector<TagInfos>::iterator it = allResults3.begin(); it != allResults3.end(); it++)
		{
			std::cout << it->getData().name << std::endl;
		}*/

		//TagInfos(db, 2).delDataFromDb(db);
		/*struct songInfos data;

		TagInfos::getSongInfosById(db, 1, &data);*/


		//maMusique.sync(db);

		//ASyncTagInfos myTest(allMyTags[0]);
		/*
		ASyncTagInfos myTest("/home/dademo/Musique/Albums/AC-DC/Back In Black/01 Hells Bells.mp3");

		std::cout << myTest.sync(db) << std::endl;
		std::cout << "Waiting for myTest.sync()" << std::endl;
		myTest.join();
*/
		sqlite3_close(db);

		return 0;
	}
}


int callbackFct(void* listIds, int ncols, char** stringResults, char** colsNames)
{
	//int id = atoi(stringResults[0]);
	((std::vector<int>*) listIds)->push_back(atoi(stringResults[0]));//= atoi(stringResults[0]);

	/*for(int i(0); i <= 50; i++)
	{
		std::cout << ((i%2)? "-":"_");
	}
	std::cout << std::endl;*/

	//std::cout << colsNames[0] << " : " << colNum  << " (" << ncols << " columns)" << std::endl;

	return 0;
}


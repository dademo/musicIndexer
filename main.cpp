#include <iostream>
#include <map>
#include <vector>
#include <stdlib.h>	// atoi()

// TEMP //
#include <unistd.h>

// AUBIO FCTS //
#include "aubiofcts.hpp"

#include "sqlite3.h"
#include "music_sqlTables.hpp"
#include "fileManip.hpp"
#include "fct_utiles.hpp"
#include "tagfcts.hpp"
#include "class_tagInfos.hpp"

#define DBNAME "/tmp/music.db"


int callbackFct(void* db, int ncols, char** stringResults, char** colsNames);

int main (int argc, char* argv[])
{
	//std::string origPath = "/home/dademo/Musique";
	std::string origPath = "/home/dademo/Music";
	//std::string origPath = "/home/dademo/Musique/Daft.Punk.Discographie.FLAC-NoTag";
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
	
		//TagInfos maMusique("/home/dademo/Musique/Playlist Wave.m3u");

		checkTables(db);

		std::vector<std::string> allMyFiles = getFileList(origPath, true);


		//std::vector<TagInfos> allMyTags = fullGetAllTags(origPath);
		std::vector<TagInfos> allMyTags = fastGetAllTags(origPath);
		
		for(std::vector<TagInfos>::iterator it = allMyTags.begin(); it != allMyTags.end(); it++)
		{
			//std::cout << it->toString() << std::endl;
			std::cout << "Sycing : "  << " [" << it->getData().album.name << "]\t" << it->getData().name<< std::endl;
			it->sync(db);
		}

		struct songInfos dataInfos = genVoidStructSongInfos();

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
		}


		//TagInfos(db, 2).delDataFromDb(db);
		/*struct songInfos data;

		TagInfos::getSongInfosById(db, 1, &data);*/


		//maMusique.sync(db);
		//
		std::cout << "BPM of [ /home/dademo/Musique/Fichiers wave/Comancero-I don't want let you down-1988-120.wav ] : " << getBPM("/home/dademo/Musique/Fichiers wave/Comancero-I don't want let you down-1988-120.wav") << std::endl;

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


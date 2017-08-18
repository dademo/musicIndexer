#include <iostream>
#include <vector>
#include <stdlib.h>	// atoi()

#include "sqlite3.h"
#include "music_sqlTables.hpp"
#include "fileManip.hpp"
#include "tagfcts.hpp"

#define DBNAME "/tmp/music.db"


int callbackFct(void* db, int ncols, char** stringResults, char** colsNames);

int main (int argc, char* argv[])
{
	std::string origPath = "/home/dademo/Musique";
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
	
		std::string supportedExtensions[18] = {"mp3", "ogg", "spx", "mpc", "ape", "flac", "wv", "tta", "wma", "m4a", "m4b", "m4p", "mp4", "3g2", "wav", "aif", "aiff", "opus"};

		//TagInfos maMusique("/home/dademo/Musique/Playlist Wave.m3u");

		checkTables(db);

		std::vector<std::string> fileList = getFileList(origPath, false);
		std::vector<TagInfos> allMyTags;

		for(std::vector<std::string>::iterator it = fileList.begin(); it != fileList.end(); it++)
		{
			std::string extension = getFileLastExtension(*it);
			bool ok = false;
			for(int i = 0; i < 18; i++)
			{
				if(extension == supportedExtensions[i]) { ok = true; break; }
				/*if(extension == supportedExtensions[i]) {
					if((origPath + "/" + *it).find("Daft") != std::string::npos)
					{ ok = true; break; } }*/
			}
			if(ok)
			{
				std::string temp = *it;
				std::cout << origPath + "/" + temp << std::endl;
				allMyTags.push_back(TagInfos(origPath + "/" + temp));
			}
		}
		
		for(std::vector<TagInfos>::iterator it = allMyTags.begin(); it != allMyTags.end(); it++)
		{
			//std::cout << it->toString() << std::endl;
			std::cout << "Sycing : " << it->getData().name << " [" << it->getData().album.name << "]" << std::endl;
			it->sync(db);
		}

		std::vector<int> allIds;

		//sqlite3_exec(db, "SELECT id FROM songs", callbackFct, db, 0);
		sqlite3_exec(db, "SELECT id FROM songs", callbackFct, &allIds, 0);

		//std::cout << TagInfos(db, 10).toString() << std::endl;

		for(std::vector<int>::iterator it = allIds.begin(); it != allIds.end(); it++)
		{
			//std::cout << TagInfos(db, *it).toString() << std::endl;
			//TagInfos(db, *it).sync(db);
			/*for(int i(0); i <= 50; i++)
			{
				std::cout << ((i%2)? "-":"_");
			}*/
			std::cout << std::endl;
		}

		//TagInfos(db, 2).delDataFromDb(db);
		/*struct songInfos data;

		TagInfos::getSongInfosById(db, 1, &data);*/

		//TagInfos maMusique("/home/dademo/Musique/Flac/Daft Punk/Musique Vol. 1 (1993-2005)/1 - Daft Punk - Musique.flac");
		//TagInfos maMusique("/home/dademo/Musique/Flac/Daft Punk/Random Access Memories/1 - Daft Punk - Give Life Back to Music.flac");
	/*
		for(std::vector<std::string>::iterator it = fileList.begin(); it != fileList.end(); it++)
		{
			std::cout << *it << std::endl;
		}
	*/

		//std::cout << maMusique.toString() << std::endl;


		//maMusique.sync(db);

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


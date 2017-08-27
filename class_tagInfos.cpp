#include <unistd.h>	// sleep()
#include "sqlite3.h"
#include <taglib/taglib.h>
#include <taglib/fileref.h>
#include <taglib/tpropertymap.h>
#include <taglib/oggflacfile.h>

#include "fileManip.hpp"
#include "mySQLiteFunctions.hpp"
#include "class_tagInfos.hpp"


TagInfos::TagInfos(TagLib::FileRef targetFile)
{
	if(targetFile.file()->isValid())
	{
		std::string targetName(targetFile.file()->name());
		m_songs_path = targetName.substr(targetName.rfind("/")+1);
		m_directories_path = targetName.substr(0, targetName.rfind("/"));
		//std::cout << "File name : " << targetName.substr(targetName.rfind("/")+1) << std::endl;
		//std::cout << "Directory : " << targetName.substr(0, targetName.rfind("/")) << std::endl;

		TagLib::PropertyMap allProperties = targetFile.file()->properties();
		//std::cout << allProperties.toString() << std::endl;

		for(TagLib::PropertyMap::Iterator it = allProperties.begin(); it != allProperties.end(); it++)
		{
			// it->first : key
			// if->second : data
				// albums
			if( it->first == "ALBUM" ) { m_albums_name = it->second.toString().to8Bit(true); }
			if( it->first == "ALBUMARTIST" ) { m_albums_artists_name = it->second.toString().to8Bit(true); }
			if( it->first == "TRACKTOTAL" ) { m_albums_ntracks = std::stoi(it->second.toString().to8Bit(true)); }
			if( it->first == "DATE" ) { m_albums_year = it->second.toString().to8Bit(true); }
				// genres
			if( it->first == "GENRE" ) { m_genres_name = it->second.toString().to8Bit(true); }
				// songs
			if( it->first == "TITLE" ) { m_songs_name = it->second.toString().to8Bit(true); }
			if( it->first == "ARTIST" ) { m_songs_artists_name = it->second.toString().to8Bit(true); }
			if( it->first == "TRACKNUMBER" ) { m_songs_tracknbr = std::stoi(it->second.toString().to8Bit(true)); }
			if( it->first == "COMMENT" ) { m_songs_comment = it->second.toString().to8Bit(true); }
			//std::cout << it->first << " <> " << it->second << std::endl;
		}

		TagLib::AudioProperties* audioProperties = targetFile.audioProperties();
		m_audioProperties_length = audioProperties->lengthInSeconds();
		m_audioProperties_bitrate = audioProperties->bitrate();
		m_audioProperties_samplerate = audioProperties->sampleRate();
		m_audioProperties_channels = audioProperties->channels();
	}
}

TagInfos::TagInfos(sqlite3* db, int songDbId) : 
	m_albums_name(""),
	m_albums_artists_name(""),
	m_albums_ntracks(0),
	m_albums_year(""),
	m_directories_path(""),
	m_genres_name(""),
	m_songs_name(""),
	m_songs_artists_name(""),
	m_songs_tracknbr(0),
	m_songs_comment(""),
	m_songs_path(""),
	m_audioProperties_length(0),
	m_audioProperties_bitrate(0),
	m_audioProperties_samplerate(0),
	m_audioProperties_channels(0)
{
	struct songInfos data;
	struct audioProperties dataAudioProperties;
	if(getSongInfosById(db, songDbId, &data))
	{
		m_albums_name		= data.album.name;
		m_albums_artists_name	= data.album.albumArtist;
		m_albums_ntracks	= data.album.nTracks;
		m_albums_year		= data.album.year;
		m_genres_name		= data.genre;
		m_songs_name		= data.name;
		m_songs_artists_name	= data.artist;
		m_songs_tracknbr	= data.tracknbr;
		m_songs_comment		= data.comment;
		m_songs_path		= data.fullPath.substr(data.fullPath.rfind("/")+1);
		m_directories_path	= data.fullPath.substr(0, data.fullPath.rfind("/"));

		if(getAudioPropertiesById(db, songDbId, &dataAudioProperties))
		{
			m_audioProperties_length	= dataAudioProperties.length;
			m_audioProperties_bitrate	= dataAudioProperties.bitrate;
			m_audioProperties_samplerate	= dataAudioProperties.samplerate;
			m_audioProperties_channels	= dataAudioProperties.channels;
		}
		else
		{
			std::cerr << "Minor error while creating [TagInfos] object with ID : " << songDbId << " (getAudioPropertiesById(...))" << std::endl;
		}
	}
	else
	{
		std::cerr << "Error while creating [TagInfos] object with ID : " << songDbId << std::endl;
	}
}

TagInfos::TagInfos(struct songInfos songTagInfos, struct audioProperties songAudioProperties):
	m_albums_name			(songTagInfos.album.name),
	m_albums_artists_name		(songTagInfos.album.albumArtist),
	m_albums_ntracks		(songTagInfos.album.nTracks),
	m_albums_year			(songTagInfos.album.year),
	m_directories_path		(songTagInfos.fullPath.substr(0, songTagInfos.fullPath.rfind("/"))),
	m_genres_name			(songTagInfos.genre),
	m_songs_name			(songTagInfos.name),
	m_songs_artists_name		(songTagInfos.artist),
	m_songs_tracknbr		(songTagInfos.tracknbr),
	m_songs_comment			(songTagInfos.comment),
	m_songs_path			(songTagInfos.fullPath.substr(songTagInfos.fullPath.rfind("/")+1)),
	m_audioProperties_length	(songAudioProperties.length),
	m_audioProperties_bitrate	(songAudioProperties.bitrate),
	m_audioProperties_samplerate	(songAudioProperties.samplerate),
	m_audioProperties_channels	(songAudioProperties.channels)
{
	
}


TagInfos::~TagInfos()
{

}


struct songInfos TagInfos::getData()
{
	struct songInfos toReturn;

	toReturn.name = m_songs_name;
	toReturn.album.name = m_albums_name;
	toReturn.album.albumArtist = m_albums_artists_name;
	toReturn.album.nTracks = m_albums_ntracks;
	toReturn.album.year = m_albums_year;
	toReturn.artist = m_songs_artists_name;
	toReturn.tracknbr = m_songs_tracknbr;
	toReturn.genre = m_genres_name;
	toReturn.comment = m_songs_comment;
	toReturn.fullPath = m_directories_path + "/" + m_songs_path;

	return toReturn;
}

struct audioProperties TagInfos::getAudioProperties()
{
	struct audioProperties toReturn;

	toReturn.length = m_audioProperties_length;
	toReturn.bitrate = m_audioProperties_bitrate;
	toReturn.samplerate = m_audioProperties_samplerate;
	toReturn.channels = m_audioProperties_channels;

	return toReturn;
}

std::string TagInfos::toString()
{
	std::string toReturn = "";

	toReturn += "[" + m_songs_name + "]\n";
	toReturn += "\tAlbum : " + m_albums_name + "\n";
	toReturn += "\tArtists : " + m_songs_artists_name + "\n";
	toReturn += "\tAlbum artist : " + m_albums_artists_name + "\n";
	toReturn += "\tGenre : " + m_genres_name + "\n";
	toReturn += "\tTrack n°" + std::to_string(m_songs_tracknbr) + "/" + std::to_string(m_albums_ntracks) + "\n";
	toReturn += "\tDate : " + m_albums_year + "\n";
	toReturn += "\tComment : " + m_songs_comment + "\n";
	toReturn += "\tPath : " +  m_directories_path + "/" + m_songs_path + "\n";
	toReturn += "\t" + std::to_string(m_audioProperties_length) + " s | " + std::to_string(m_audioProperties_bitrate) + " kb/s | " + std::to_string(m_audioProperties_samplerate) + " Hz | " + std::to_string(m_audioProperties_channels) + " channels\n";

	return toReturn;
}

bool TagInfos::sync(sqlite3* db)
{
		//std::cout << "CompareDirPath(db) : " << compareDirPath(db) << std::endl;
		//std::cout << "CompareSongPath(db) : " << compareSongPath(db) << std::endl;
		//if(!compareArtist(db)) { std::cout << m_songs_artists_name << " | " << m_albums_artists_name << std::endl; }
		if(fexists(m_directories_path + "/" + m_songs_path))
		{
			if(!compareSongArtist(db)) { std::cout << "Artist (song) not found, adding..." << std::endl; insertSongArtist(db); }
			if(!compareAlbumArtist(db)) { std::cout << "Artist (album) not found, adding..." << std::endl; insertAlbumArtist(db); }
			if(!compareAlbum(db)) { std::cout << "Album not found, adding..." << std::endl; insertAlbum(db); }
			if(!compareGenre(db)) { std::cout << "Genre not found, adding..." << std::endl; insertGenre(db); }
			if(compareDirPath(db) && compareSongPath(db))
			{
				if(!compareSongData(db))
				{ std::cout << "Song's datas (" + m_songs_name + ") are obsolete, updating the database..." << std::endl; updateSong(db); }
			}
			else
			{ std::cout << "Adding song [" << m_songs_name << "] to the database" << std::endl; insertSong(db); }
			if(!compareAudioProperties(db)) { std::cout << "AudioProperties not found, adding..." << std::endl; insertAudioProperties(db); }
		}
		else
		{
			// Deleting the entry from the database ++ look for every object linked to this entry
			delDataFromDb(db);
		}
	return true;
}


bool TagInfos::insertAlbum(sqlite3* db)
{
	if(compareAlbumArtist(db))
	{
		sqlite3_stmt* requestStatement;
		int returnVal = 0;
		int artistId = getArtistId(db, m_albums_artists_name);

		returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "INSERT OR REPLACE INTO albums(id_artist, name, ntracks, year) VALUES (?,?,?,?)", -1, &requestStatement, 0));

		if(returnVal != SQLITE_OK) { return false; }

		if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 1, artistId)) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 2, m_albums_name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 3, m_albums_ntracks)) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 4, m_albums_year.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };

		do{
			returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
			if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
			if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

		} while (returnVal != SQLITE_DONE);

		sqlite3_finalize(requestStatement);
		return true;
	}
	else { std::cerr << "TagInfos::insertAlbum() : missing artist (compareArtist)" << std::endl; return false; }
}

bool TagInfos::insertSongArtist(sqlite3* db)
{
	sqlite3_stmt* requestStatement;
	int returnVal = 0;

	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "INSERT OR REPLACE INTO artists(name) VALUES (?)", -1, &requestStatement, 0));

	if(returnVal != SQLITE_OK) { return false; }

	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_songs_artists_name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };

	do{
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
		if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

	} while (returnVal != SQLITE_DONE);

	sqlite3_finalize(requestStatement);
	return true;
}
bool TagInfos::insertAlbumArtist(sqlite3* db)
{
	sqlite3_stmt* requestStatement;
	int returnVal = 0;

	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "INSERT OR REPLACE INTO artists(name) VALUES (?)", -1, &requestStatement, 0));

	if(returnVal != SQLITE_OK) { return false; }

	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_albums_artists_name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };

	do{
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
		if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

	} while (returnVal != SQLITE_DONE);

	sqlite3_finalize(requestStatement);
	return true;

}

bool TagInfos::insertAudioProperties(sqlite3* db)
{
	sqlite3_stmt* requestStatement;
	int returnVal = 0;

	//returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "INSERT OR REPLACE INTO audioProperties(id_song,length,bitrate,sampleRate,channels) VALUES (?,?,?,?,?)", -1, &requestStatement, 0), 0);
	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "INSERT INTO audioProperties(id_song,length,bitrate,sampleRate,channels) VALUES (?,?,?,?,?)", -1, &requestStatement, 0));

	if(returnVal != SQLITE_OK) { return false; }

	if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 1, TagInfos::getSongId(db, m_directories_path, m_songs_path))) != SQLITE_OK) { return false; };
	if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 2, m_audioProperties_length)) != SQLITE_OK) { return false; };
	if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 3, m_audioProperties_bitrate)) != SQLITE_OK) { return false; };
	if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 4, m_audioProperties_samplerate)) != SQLITE_OK) { return false; };
	if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 5, m_audioProperties_channels)) != SQLITE_OK) { return false; };

	do{
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
		if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

	} while (returnVal != SQLITE_DONE);

	sqlite3_finalize(requestStatement);
	return true;
}

bool TagInfos::insertDirPath(sqlite3* db)
{
	sqlite3_stmt* requestStatement;
	int returnVal = 0;

	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "INSERT OR REPLACE INTO directories(path) VALUES (?)", -1, &requestStatement, 0));

	if(returnVal != SQLITE_OK) { return false; }

	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_directories_path.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };

	do{
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
		if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

	} while (returnVal != SQLITE_DONE);

	sqlite3_finalize(requestStatement);

	return true;
}

bool TagInfos::insertGenre(sqlite3* db)
{
	sqlite3_stmt* requestStatement;
	int returnVal = 0;

	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "INSERT OR REPLACE INTO genres(name) VALUES (?)", -1, &requestStatement, 0));

	if(returnVal != SQLITE_OK) { return false; }

	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_genres_name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };

	do{
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
		if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

	} while (returnVal != SQLITE_DONE);

	sqlite3_finalize(requestStatement);
	return true;
}

bool TagInfos::insertSong(sqlite3* db)
{
	if(compareAlbum(db) && compareSongArtist(db) && compareGenre(db))
	{
		if(! compareDirPath(db)) { insertDirPath(db); }
		sqlite3_stmt* requestStatement;
		int returnVal = 0;

		returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "INSERT OR REPLACE INTO songs(id_album,id_genre,id_artist,id_dirName,path,name,tracknbr,comment) VALUES (?,?,?,?,?,?,?,?)", -1, &requestStatement, 0));
		//returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "INSERT INTO songs(id_album,id_genre,id_artist,id_dirName,path,name,tracknbr,comment) VALUES (?,?,?,?,?,?,?,?)", -1, &requestStatement, 0), 0);

		if(returnVal != SQLITE_OK) { return false; }

		if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 1, TagInfos::getAlbumId(db, m_albums_name, m_albums_artists_name, m_albums_ntracks, m_albums_year))) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 2, TagInfos::getGenreId(db, m_genres_name))) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 3, TagInfos::getArtistId(db, m_songs_artists_name))) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 4, TagInfos::getDirnameId(db, m_directories_path))) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 5, m_songs_path.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 6, m_songs_name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 7, m_songs_tracknbr)) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 8, m_songs_comment.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };

		do{
			returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
			if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
			if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

		} while (returnVal != SQLITE_DONE);

		sqlite3_finalize(requestStatement);
		return true;
	}
	else { std::cerr << "TagInfo::insertSong() : missing album(compareAlbum) or artist(compareArtist) or genre(compareGenre)" << std::endl; return false; }
}

bool TagInfos::updateSong(sqlite3* db)
{
	if(!compareDirPath(db))
	{ insertSong(db); }
	else
	{
		sqlite3_stmt* requestStatement;
		int returnVal = 0;

		returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
		UPDATE OR REPLACE songs \
		SET id_album=?,id_genre=?,id_artist=?,name=?,tracknbr=?,comment=? \
		WHERE id=? ", -1, &requestStatement, 0));

		if(returnVal != SQLITE_OK) { return false; }

		if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 1, TagInfos::getAlbumId(db, m_albums_name, m_albums_artists_name, m_albums_ntracks, m_albums_year))) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 2, TagInfos::getGenreId(db, m_genres_name))) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 3, TagInfos::getArtistId(db, m_songs_artists_name))) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 4, m_songs_name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 5, m_songs_tracknbr)) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 6, m_songs_comment.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 7, TagInfos::getSongId(db, m_directories_path, m_songs_path))) != SQLITE_OK) { return false; };

		do{
			returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
			if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
			if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

		} while (returnVal != SQLITE_DONE);

		sqlite3_finalize(requestStatement);
	}

	return true;
}


int TagInfos::getAlbumId(sqlite3* db, std::string albumName,std::string artistName, int nTracks, std::string date)
{
	sqlite3_stmt* requestStatement;
	int returnVal = 0;
	int id = 0;

	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
SELECT albums.id \
FROM albums,artists \
WHERE albums.name=? AND artists.name=? AND albums.ntracks=? AND albums.year=? AND artists.id=albums.id_artist", -1, &requestStatement, 0));
//WHERE albums.name=? AND artists.name=? AND albums.ntracks=? AND albums.year=? AND artists.id=albums.id_artists", -1, &requestStatement, 0), 0);

	if(returnVal != SQLITE_OK) { return -1; }

	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, albumName.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return -1; };
	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 2, artistName.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return -1; };
	if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 3, nTracks)) != SQLITE_OK) { return -1; };
	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 4, date.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return -1; };

	do{
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return -1; }
		if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }
		if(returnVal == SQLITE_ROW)
		{
			id = sqlite3_column_int(requestStatement,0);
		}

	} while (returnVal != SQLITE_DONE);

	sqlite3_finalize(requestStatement);

	return id;
}

int TagInfos::getGenreId(sqlite3* db, std::string genreName)
{
	return idResearch(db, "genres", "name", genreName);
}

int TagInfos::getArtistId(sqlite3* db, std::string artistName)
{
	return idResearch(db, "artists", "name", artistName);
}

int TagInfos::getDirnameId(sqlite3* db, std::string dirName)
{
	return idResearch(db, "directories", "path", dirName);
}

int TagInfos::getSongId(sqlite3* db, std::string dirPath, std::string songFileName)
{
	sqlite3_stmt* requestStatement;
	int returnVal = 0;
	int id = 0;

	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
	SELECT songs.id \
	FROM directories,songs \
	WHERE directories.path=? AND songs.path=? AND songs.id_dirName=directories.id", -1, &requestStatement, 0));

	if(returnVal != SQLITE_OK) { return -1; }

	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, dirPath.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return -1; };
	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 2, songFileName.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return -1; };

	do{
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return -1; }
		if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }
		if(returnVal == SQLITE_ROW)
		{
			//std::cout << "Song Id : " << sqlite3_column_int(requestStatement, 0) << std::endl;
			id = sqlite3_column_int(requestStatement,0);
		}

	} while (returnVal != SQLITE_DONE);

	sqlite3_finalize(requestStatement);
	return id;
}

int TagInfos::getAudioPropertiesId(sqlite3* db, int songId)
{
	sqlite3_stmt* requestStatement;
	int returnVal = 0;
	int id = 0;

	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
	SELECT audioProperties.id \
	FROM audioProperties \
	WHERE audioProperties.id_song=?", -1, &requestStatement, 0));

	if(returnVal != SQLITE_OK) { return -1; }

	if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 1, songId)) != SQLITE_OK) { return -1; };

	do{
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return -1; }
		if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }
		if(returnVal == SQLITE_ROW)
		{
			id = sqlite3_column_int(requestStatement,0);
		}

	} while (returnVal != SQLITE_DONE);

	sqlite3_finalize(requestStatement);
	return id;
}


bool TagInfos::compareAlbum(sqlite3* db)	// true -> OK
{
	//if(m_albums_name != "")
	//{
		sqlite3_stmt* requestStatement;
		int returnVal = 0;
		int i = 0;

		returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
	SELECT albums.name,artists.name \
	FROM albums,artists \
	WHERE albums.name=? AND artists.name=? AND albums.year=? AND albums.ntracks=? \
		AND  albums.id_artist=artists.id", -1, &requestStatement, 0));

		if(returnVal != SQLITE_OK) { return false; }

		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_albums_name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 2, m_albums_artists_name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 3, m_albums_year.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 4, m_albums_ntracks)) != SQLITE_OK) { return false; };

		do{
			returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
			if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
			if(returnVal != SQLITE_DONE)
			{
				if(m_albums_name != std::string((char*)sqlite3_column_text(requestStatement, 0)))
				{ return false; }
				if(m_albums_artists_name != std::string((char*)sqlite3_column_text(requestStatement, 1)))
				{ return false; }
				i++;
			}

		} while (returnVal != SQLITE_DONE);

		if(i == 0)	// No result
		{ return false; }

		sqliteReturnVal(sqlite3_finalize(requestStatement));
	//}

	return true;
}

bool TagInfos::compareSongArtist(sqlite3* db)	// true -> OK
{
	// Single artist
	//if(m_songs_artists_name != "")
	//{
		sqlite3_stmt* requestStatement;
		int returnVal = 0;
		int i = 0;

		returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
		SELECT artists.name \
		FROM artists\
		WHERE artists.name=?", -1, &requestStatement, 0));
 
		if(returnVal != SQLITE_OK) { return false; }

		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_songs_artists_name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };

		do{
			returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
			if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
			if(returnVal != SQLITE_DONE)
			{
				if(m_songs_artists_name != std::string((char*)sqlite3_column_text(requestStatement, 0)))
				{ return false; }
				i++;
			}
			if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

		} while (returnVal != SQLITE_DONE);

		if(i == 0)	// No result
		{ return false; }

		sqliteReturnVal(sqlite3_finalize(requestStatement));
	//}

	return true;
}

bool TagInfos::compareAlbumArtist(sqlite3* db)
{
	//if(m_albums_artists_name != "")
	//{
		sqlite3_stmt* requestStatement;
		int returnVal = 0;
		int i = 0;

		returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
		SELECT artists.name \
		FROM artists \
		WHERE artists.name=?", -1, &requestStatement, 0));

		if(returnVal != SQLITE_OK) { return false; }

		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_albums_artists_name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };

		do{
			returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
			if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
			if(returnVal != SQLITE_DONE)
			{
				if(m_albums_artists_name != std::string((char*)sqlite3_column_text(requestStatement, 0)))
				{ std::cout << m_songs_artists_name << " <> " << sqlite3_column_text(requestStatement, 0) << std::endl; return false; }
				i++;
			}
		if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

		} while (returnVal != SQLITE_DONE);

		if(i == 0)	// No result
		{ return false; }

		sqliteReturnVal(sqlite3_finalize(requestStatement));
	//}

	return true;
}

bool TagInfos::compareGenre(sqlite3* db)
{
	//if(m_genres_name != "")
	//{
		sqlite3_stmt* requestStatement;
		int returnVal = 0;
		int i = 0;

		returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
		SELECT genres.name \
		FROM genres \
		WHERE genres.name=?", -1, &requestStatement, 0));

		if(returnVal != SQLITE_OK) { return false; }

		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_genres_name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };

		do{
			returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
			if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
			if(returnVal != SQLITE_DONE)
			{
				if(m_genres_name != std::string((char*)sqlite3_column_text(requestStatement, 0)))
				{ return false; }
				i++;
			}
			if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

		} while (returnVal != SQLITE_DONE);

		if(i == 0)	// No result
		{ return false; }

		sqliteReturnVal(sqlite3_finalize(requestStatement));

	//}

	return true;
}

bool TagInfos::compareDirPath(sqlite3* db)	// Checking if the path exists in the database
{
	sqlite3_stmt* requestStatement;
	int returnVal = 0;
	int i = 0;

	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
	SELECT directories.path \
	FROM directories \
	WHERE directories.path=?", -1, &requestStatement, 0));

	if(returnVal != SQLITE_OK) { return false; }

	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_directories_path.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };

	do{
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
		if(returnVal != SQLITE_DONE)
		{
			if(m_directories_path != std::string((char*)sqlite3_column_text(requestStatement, 0)))
			{ return false; }
			i++;
		}
		if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

	} while (returnVal != SQLITE_DONE);

	if(i == 0)	// No result
	{ return false; }

	sqliteReturnVal(sqlite3_finalize(requestStatement));

	return true;
}

bool TagInfos::compareSongPath(sqlite3* db)	// Checking if the path exists in the database
{
	sqlite3_stmt* requestStatement;
	int returnVal = 0;
	int i = 0;

	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
	SELECT directories.path,songs.path \
	FROM songs,directories \
	WHERE songs.path=? AND directories.path=? AND songs.id_dirName=directories.id", -1, &requestStatement, 0));

	if(returnVal != SQLITE_OK) { return false; }

	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_songs_path.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };
	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 2, m_directories_path.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };

	do{
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
		if(returnVal != SQLITE_DONE)
		{
			if(m_directories_path != std::string((char*)sqlite3_column_text(requestStatement, 0)))	{ return false; }
			if(m_songs_path != std::string((char*)sqlite3_column_text(requestStatement, 1)))	{ return false; }
			i++;
		}
		if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

	} while (returnVal != SQLITE_DONE);

	if(i == 0)	// No result
	{ return false; }

	sqliteReturnVal(sqlite3_finalize(requestStatement));

	return true;
}

bool TagInfos::compareSongData(sqlite3* db)
{
	//if(compareSongPath(db) && compareAlbum(db) && compareSongArtist(db) && compareGenre(db))
	if(compareSongPath(db) && compareSongArtist(db) && compareGenre(db))
	{
		sqlite3_stmt* requestStatement;
		int returnVal = 0;
		int i = 0;

		/*returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
	SELECT songs.id_album,songs.id_genre,songs.id_artist,songs.id_dirName,songs.path,songs.name,songs.tracknbr,songs.comment \
	FROM songs,directories,albums \
	WHERE songs.path=? AND directories.path=? AND songs.id_dirName=directories.id AND songs.id_album=albums.id", -1, &requestStatement, 0), 0);*/
		returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
	SELECT songs.id_album,songs.id_genre,songs.id_artist,songs.id_dirName,songs.path,songs.name,songs.tracknbr,songs.comment \
	FROM songs,directories\
	WHERE songs.path=? AND directories.path=? AND songs.id_dirName=directories.id", -1, &requestStatement, 0));

		if(returnVal != SQLITE_OK) { return false; }

		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_songs_path.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 2, m_directories_path.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };

		do{
			returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
			if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
			if(returnVal != SQLITE_DONE)
			{
				if(TagInfos::getAlbumId(db, m_albums_name, m_albums_artists_name, m_albums_ntracks, m_albums_year) != sqlite3_column_int(requestStatement, 0))	{ std::cout << "Album Id different : " << TagInfos::getAlbumId(db, m_albums_name, m_albums_artists_name, m_albums_ntracks, m_albums_year) << " <> " << sqlite3_column_int(requestStatement, 0) << std::endl; return false; }
				if(TagInfos::getGenreId(db, m_genres_name) != sqlite3_column_int(requestStatement, 1))	{ std::cout << "Genre Id different : " << TagInfos::getGenreId(db, m_genres_name) << " <> " << sqlite3_column_int(requestStatement, 1) << std::endl; return false; }
				
if(TagInfos::getArtistId(db, m_songs_artists_name) != sqlite3_column_int(requestStatement, 2))	{ std::cout << "Artist Id different : " << TagInfos::getGenreId(db, m_songs_artists_name) << " <> " << sqlite3_column_int(requestStatement, 2) << std::endl; return false; }
				if(TagInfos::getDirnameId(db, m_directories_path) != sqlite3_column_int(requestStatement, 3))	{ std::cout << "Directory is different" << std::endl; return false; }
				if(m_songs_path != std::string((char*)sqlite3_column_text(requestStatement, 4)))	{ std::cout << "Song's path is different" << std::endl; return false; }
				if(m_songs_name != std::string((char*)sqlite3_column_text(requestStatement, 5)))	{ std::cout << "Song's name different" << std::endl; return false; }
				if(m_songs_tracknbr != sqlite3_column_int(requestStatement, 6))	{ std::cout << "Tracknbr different" << std::endl; return false; }
				if(m_songs_comment != std::string((char*)sqlite3_column_text(requestStatement, 7)))	{ std::cout << "Comment different" << std::endl; return false; }
				i++;
			}
			if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

		} while (returnVal != SQLITE_DONE);

		if(i == 0)	// No result
		{ std::cout << "No result for file : " << m_directories_path << "/" << m_songs_path << std::endl; return false; }

		sqliteReturnVal(sqlite3_finalize(requestStatement));

		return true;
	}
	else { std::cerr << "TagInfos::compareSongData() : missing song path(compareSongPath) or missing album (compareAlbum) or missing artist(compareArtist) or missing genre (compareGenre)" << std::endl; return false; }
}

bool TagInfos::compareAudioProperties(sqlite3* db)
{
	sqlite3_stmt* requestStatement;
	int returnVal = 0;
	int i = 0;

	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
	SELECT audioProperties.length,audioProperties.bitrate,audioProperties.samplerate,audioProperties.channels \
	FROM audioProperties,songs,directories \
	WHERE songs.path=? AND directories.path=? AND audioProperties.id_song=songs.id AND songs.id_dirName=directories.id", -1, &requestStatement, 0));

	if(returnVal != SQLITE_OK) { return false; }

	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_songs_path.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };
	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 2, m_directories_path.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) { return false; };

	do{
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
		if(returnVal != SQLITE_DONE)
		{
			if(m_audioProperties_length != sqlite3_column_int(requestStatement, 0))	{ return false; }
			if(m_audioProperties_bitrate != sqlite3_column_int(requestStatement, 1))	{ return false; }
			if(m_audioProperties_samplerate != sqlite3_column_int(requestStatement, 2))	{ return false; }
			if(m_audioProperties_channels != sqlite3_column_int(requestStatement, 3))	{ return false; }
			i++;
		}
		if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

	} while (returnVal != SQLITE_DONE);

	if(i == 0)	// No result
	{ return false; }

	sqliteReturnVal(sqlite3_finalize(requestStatement));

	return true;
}



bool TagInfos::getAlbumInfosById(sqlite3* db, int albumId, struct albumInfos* infos)
{
	// Reset info
	infos->name = "";
	infos->albumArtist = "";
	infos->nTracks = 0;
	infos->year = "";

	sqlite3_stmt* requestStatement;
	int returnVal = 0;

	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
	SELECT albums.name, artists.name, albums.ntracks, albums.year \
	FROM albums,artists \
	WHERE albums.id=? AND albums.id_artist=artists.id", -1, &requestStatement, 0));

	if(returnVal != SQLITE_OK) { return false; }

	if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 1, albumId)) != SQLITE_OK) { return false; }

	do {
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
		if(returnVal != SQLITE_DONE)
		{
			infos->name = std::string((char*) sqlite3_column_text(requestStatement, 0));
			infos->albumArtist = std::string((char*) sqlite3_column_text(requestStatement, 1));
			infos->nTracks = sqlite3_column_int(requestStatement, 2);
			infos->year = std::string((char*) sqlite3_column_text(requestStatement, 3));
		}
		if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

	} while (returnVal != SQLITE_DONE);

	return true;
}

bool TagInfos::getSongInfosById(sqlite3* db, int songId, struct songInfos* infos)
{
	// Reset info
	infos->name="";
	infos->artist="";
	infos->tracknbr=0;
	infos->genre="";
	infos->fullPath="";

	sqlite3_stmt* requestStatement;
	int returnVal = 0;
	int albumId = 0;

	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
	SELECT songs.name, artists.name, songs.tracknbr, genres.name, directories.path, songs.path, songs.id_album \
	FROM songs,artists,genres,directories \
	WHERE songs.id=? AND songs.id_artist=artists.id AND songs.id_genre=genres.id AND songs.id_dirName=directories.id", -1, &requestStatement, 0));

	if(returnVal != SQLITE_OK) { return false; }

	if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 1, songId)) != SQLITE_OK) { return false; }

	do {
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
		if(returnVal != SQLITE_DONE)
		{
			infos->name = std::string((char*) sqlite3_column_text(requestStatement, 0));
			infos->artist = std::string((char*) sqlite3_column_text(requestStatement, 1));
			infos->tracknbr = sqlite3_column_int(requestStatement, 2);
			infos->genre = std::string((char*) sqlite3_column_text(requestStatement, 3));
			infos->fullPath = std::string((char*) sqlite3_column_text(requestStatement, 4)) + "/" + std::string((char*) sqlite3_column_text(requestStatement, 5));
			albumId = sqlite3_column_int(requestStatement, 6);
		}
		if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

	} while (returnVal != SQLITE_DONE);

	if(!TagInfos::getAlbumInfosById(db, albumId, &infos->album)) { return false; }

	return true;
}


bool TagInfos::getAudioPropertiesById(sqlite3* db, int songId, struct audioProperties* infos)
{
	// Reset info
	infos->length=0;
	infos->bitrate=0;
	infos->samplerate=0;
	infos->channels=0;

	sqlite3_stmt* requestStatement;
	int returnVal = 0;

	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
	SELECT audioProperties.length, audioProperties.bitrate, audioProperties.sampleRate, audioProperties.channels \
	FROM audioProperties \
	WHERE audioProperties.id_song=?", -1, &requestStatement, 0));

	if(returnVal != SQLITE_OK) { return false; }

	if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 1, songId)) != SQLITE_OK) { return false; }

	do {
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return false; }
		if(returnVal != SQLITE_DONE)
		{
			infos->length = sqlite3_column_int(requestStatement, 0);
			infos->bitrate = sqlite3_column_int(requestStatement, 1);
			infos->samplerate = sqlite3_column_int(requestStatement, 2);
			infos->channels = sqlite3_column_int(requestStatement, 3);
		}
		if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

	} while (returnVal != SQLITE_DONE);

	return true;
}


void TagInfos::delDataFromDb(sqlite3* db)
{
	// AlbumArtist
	if(!(getnResults(db, "SELECT albums.id FROM albums,artists WHERE albums.id_artist=artists.id AND artists.id=" + std::to_string(getArtistId(db, m_albums_artists_name))) > 1)) { delAlbumArtistFromDb(db); }

	// SongArtist
	if(!(getnResults(db, "SELECT songs.id FROM songs,artists WHERE songs.id_artist=artists.id AND artists.id=" + std::to_string(getArtistId(db, m_songs_artists_name))) > 1)) { delSongArtistFromDb(db); }

	// Album
	if(!(getnResults(db, "SELECT songs.id FROM songs,albums WHERE songs.id_album=albums.id AND albums.id=" + std::to_string(getAlbumId(db, m_albums_name, m_albums_artists_name, m_albums_ntracks, m_albums_year))) > 1)) { delAlbumFromDb(db); }

	// AudioProperties
	if(!(getnResults(db, "SELECT audioProperties.id FROM audioProperties WHERE audioProperties.id_song=" + std::to_string(getSongId(db, m_directories_path, m_songs_path))) > 1)) { delAudioPropertiesFromDb(db); }

	// The song itself
	delSongFromDb(db);

	/*std::cout << getnResults(db, "SELECT songs.id FROM songs,albums WHERE songs.id_album=albums.id AND albums.id=" + std::to_string(getAlbumId(db, m_albums_name, m_albums_artists_name, m_albums_ntracks, m_albums_year))) << std::endl;
	std::cout << getnResults(db, "SELECT albums.id FROM albums,artists WHERE albums.id_artist=artists.id AND artists.id=" + std::to_string(getArtistId(db, m_albums_artists_name))) << std::endl;
	std::cout << getnResults(db, "SELECT songs.id FROM songs,artists WHERE songs.id_artist=artists.id AND artists.id=" + std::to_string(getArtistId(db, m_songs_artists_name))) << std::endl;
	std::cout << getnResults(db, "SELECT audioProperties.id FROM audioProperties WHERE audioProperties.id_song=" + std::to_string(getSongId(db, m_directories_path, m_songs_path))) << std::endl;*/

}


void TagInfos::delAlbumArtistFromDb(sqlite3* db)
{
	std::cout << "Deleting album artist of " + m_songs_name + " [" + m_albums_name + "] with id : " + std::to_string(getArtistId(db, m_albums_artists_name)) << std::endl;
	delElementFromDb(db, "DELETE FROM artists WHERE artists.id=" + std::to_string(getArtistId(db, m_albums_artists_name)));
}

void TagInfos::delSongArtistFromDb(sqlite3* db)
{
	std::cout << "Deleting song artist of " + m_songs_name + " [" + m_albums_name + "] with id : " + std::to_string(getArtistId(db, m_songs_artists_name)) << std::endl;
	delElementFromDb(db, "DELETE FROM artists WHERE artists.id=" + std::to_string(getArtistId(db, m_songs_artists_name)));
}

void TagInfos::delAlbumFromDb(sqlite3* db)
{
	std::cout << "Deleting album " + m_songs_name + " [" + m_albums_name + "] with id : " + std::to_string(getAlbumId(db, m_albums_name, m_albums_artists_name, m_albums_ntracks, m_albums_year)) << std::endl;
	delElementFromDb(db, "DELETE FROM albums WHERE albums.id=" + std::to_string(getAlbumId(db, m_albums_name, m_albums_artists_name, m_albums_ntracks, m_albums_year)));
}

void TagInfos::delAudioPropertiesFromDb(sqlite3* db)
{
	std::cout << "Deleting audio properties of " + m_songs_name + " [" + m_albums_name + "]"<< std::endl;
	delElementFromDb(db, "DELETE FROM audioProperties WHERE audioProperties.id=" + std::to_string(getAudioPropertiesId(db, getSongId(db, m_directories_path, m_songs_path) )));
}

void TagInfos::delSongFromDb(sqlite3* db)
{
	std::cout << "Deleting song " + m_songs_name + " [" + m_albums_name + "] with id : " + std::to_string(getSongId(db, m_directories_path, m_songs_path))<< std::endl;
	delElementFromDb(db, "DELETE FROM songs WHERE songs.id=" + std::to_string(getSongId(db, m_directories_path, m_songs_path)));
}



std::vector<TagInfos> TagInfos::searchTagInfos(sqlite3* db, struct songInfos songSearch, struct audioProperties songProperties)
{
	// Definitions //
	enum dataType { dataType_int, dataType_text };
	struct dataBind {
		dataType type;
		std::string data;
	};

	// Variables //
	std::vector<TagInfos> searchResults;
	std::vector<std::string> searchDB, finalSearchDB;
	std::map<std::string, struct dataBind> albums;
	std::map<std::string, struct dataBind> songs;
	std::map<std::string, struct dataBind> songAudioProperties;
	std::string directories="";
	std::string genre="";

	std::string request="";
	std::string albumRequest="";
	bool bAlbumRequest=false;

	if(songSearch.album.name != "") { albums["name"]={ dataType_text, songSearch.album.name }; searchDB.push_back("albums"); }
	if(songSearch.album.albumArtist != "") { albums["artist"]={ dataType_text, songSearch.album.albumArtist }; searchDB.push_back("albums"); searchDB.push_back("artists"); }
	if(songSearch.album.nTracks != 0) { albums["ntracks"]={ dataType_int, std::to_string(songSearch.album.nTracks) }; searchDB.push_back("albums"); }
	if(songSearch.album.year != "") { albums["year"]={ dataType_text, songSearch.album.year }; searchDB.push_back("albums"); }

	if(songSearch.genre != "") { genre=songSearch.genre ; searchDB.push_back("genres"); }

	if(songSearch.artist != "") { songs["artist"]={ dataType_text, songSearch.artist }; searchDB.push_back("artists"); }
	if(songSearch.name != "") { songs["name"]={ dataType_text, songSearch.name }; }
	if(songSearch.tracknbr != 0) { songs["tracknbr"]={ dataType_int, std::to_string(songSearch.tracknbr) }; }
	if(songSearch.comment != "") { songs["comment"]={ dataType_int, songSearch.comment }; }

	if(songSearch.fullPath != "")
	{
		directories=songSearch.fullPath.substr(0, songSearch.fullPath.rfind("/"));
		songs["path"]={ dataType_text, songSearch.fullPath.substr(songSearch.fullPath.rfind("/")+1)};
		searchDB.push_back("directories");
		searchDB.push_back("songs");
	}

	if(songProperties.length != 0) { songAudioProperties["length"]={ dataType_int, std::to_string(songProperties.length) }; searchDB.push_back("audioProperties"); };
	if(songProperties.bitrate != 0) { songAudioProperties["bitrate"]={ dataType_int, std::to_string(songProperties.bitrate) };  searchDB.push_back("audioProperties"); };
	if(songProperties.samplerate != 0) { songAudioProperties["samplerate"]={dataType_int, std::to_string(songProperties.samplerate) }; searchDB.push_back("audioProperties"); };
	if(songProperties.channels != 0) { songAudioProperties["channels"]={ dataType_int, std::to_string(songProperties.channels) }; searchDB.push_back("audioProperties"); };

	// Unique values
	bool multipleDB[5] = {false, false, false, false, false};	// albums, artists, genres, audioProperties, directories
	for(std::vector<std::string>::iterator it=searchDB.begin(); it!=searchDB.end(); it++)
	{
		if(*it == "albums")
		{ if(multipleDB[0] == false) { finalSearchDB.push_back(*it); multipleDB[0] = true; bAlbumRequest = true; } }
		if(*it == "artists")
		{ if(multipleDB[1] == false) { finalSearchDB.push_back(*it); multipleDB[1] = true; } }
		if(*it == "genres")
		{ if(multipleDB[2] == false) { finalSearchDB.push_back(*it); multipleDB[2] = true; } }
		if(*it == "audioProperties")
		{ if(multipleDB[3] == false) { finalSearchDB.push_back(*it); multipleDB[3] = true; } }
		if(*it == "directories")
		{ if(multipleDB[4] == false) { finalSearchDB.push_back(*it); multipleDB[4] = true; } }
	}

	request += "SELECT songs.id FROM songs";

	for(std::vector<std::string>::iterator it=finalSearchDB.begin(); it != finalSearchDB.end(); it++)
	{
		request += "," + *it;
	}

	request += " WHERE ";

	int nElems = 0;
	for(std::map<std::string, struct dataBind>::iterator it=songs.begin(); it != songs.end(); it++)
	{
		if((*it).first == "artist") { request += std::string(((nElems != 0)? " AND " : "" )) + "artists.name LIKE \"" + (*it).second.data + "\""; }
		else
		{
			request += std::string(((nElems != 0)? " AND " : "" )) + "songs." + (*it).first +
			(((*it).second.type==dataType_text )? " LIKE \"" : "=") + (*it).second.data + (((*it).second.type==dataType_text )? "\"" : "");
		}
		nElems++;
	}
	for(std::map<std::string, struct dataBind>::iterator it=songAudioProperties.begin(); it != songAudioProperties.end(); it++)
	{
		request += std::string(((nElems != 0)? " AND " : "" )) + "audioProperties." + (*it).first + (((*it).second.type==dataType_text )? " LIKE \"" : "=") + (*it).second.data + (((*it).second.type==dataType_text )? "\"" : "");
		nElems++;
	}

	if(genre != "")
	{
		request += std::string(((nElems != 0)? " AND " : "" )) + "genres.name LIKE \"" + genre + "\"";
		nElems++;
	}
	if(directories != "")
	{
		request += std::string(((nElems != 0)? " AND " : "" )) + "directories.path=\"" + directories + "\"";
		nElems++;
	}
	// Links between tables
	for(std::vector<std::string>::iterator it=finalSearchDB.begin(); it != finalSearchDB.end(); it++)
	{
		if((*it) == "albums") { request += std::string(((nElems != 0)? " AND " : "" )) + "songs.id_album=albums.id"; }
		if((*it) == "artists") { request += std::string(((nElems != 0)? " AND " : "" )) + "songs.id_artist=artists.id"; }
		if((*it) == "audioProperties") { request += std::string(((nElems != 0)? " AND " : "" )) + "audioProperties.id_song=songs.id"; }
		if((*it) == "genres") { request += std::string(((nElems != 0)? " AND " : "" )) + "songs.id_genre=genres.id"; }
		if((*it) == "directories") { request += std::string(((nElems != 0)? " AND " : "" )) + "songs.id_dirName=directories.id"; }
	}

	if(bAlbumRequest)
	{
		int nElemsBis=0;
		albumRequest = std::string("SELECT albums.id FROM albums") + ((songSearch.album.albumArtist != "")? ",artists" : "" ) + " WHERE ";
		for(std::map<std::string, struct dataBind>::iterator it=albums.begin(); it != albums.end(); it++)
		{
			if((*it).first == "artist")
			{
				albumRequest += std::string(((nElemsBis != 0)? " AND " : "" )) + "albums.id_artist=artists.id AND artists.name LIKE \"" + (*it).second.data + "\"";
				nElemsBis++;
			}
			else
			{
				albumRequest += std::string(((nElemsBis != 0)? " AND " : "" )) + "albums." + (*it).first + (((*it).second.type==dataType_text )? " LIKE \"" : "=") + (*it).second.data + (((*it).second.type==dataType_text )? "\"" : "");
				nElemsBis++;
			}
		}
		request += std::string(((nElems != 0)? " AND " : "" )) + "songs.id_album=(" + albumRequest + ")";
	}

	std::cout << "Request : " << std::endl << request << std::endl;


	// Request part //
	sqlite3_stmt* requestStatement;
	int returnVal = 0;

	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, request.c_str(), -1, &requestStatement, 0));

	if(returnVal != SQLITE_OK) { return searchResults; }	// void return


	do {
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement));
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_BUSY) { return searchResults; }
		if(returnVal != SQLITE_DONE)
		{
			searchResults.push_back(TagInfos(db, sqlite3_column_int(requestStatement, 0)));
		}
		if(returnVal == SQLITE_BUSY) { std::cout << "Database busy... waiting" << std::endl; sleep(1); }

	} while (returnVal != SQLITE_DONE);


	return searchResults;
}

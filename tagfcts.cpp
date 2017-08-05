#include <iostream>
#include <string>
#include "sqlite3.h"
#include <taglib/taglib.h>
#include <taglib/fileref.h>
#include <taglib/tpropertymap.h>
#include <taglib/oggflacfile.h>

#include "mySQLiteFunctions.hpp"
#include "tagfcts.hpp"

TagInfos::TagInfos(
	std::string	albums_name,
	std::string	albums_artist,
	int		albums_ntracks,
	std::string	albums_year,
	std::string	directories_path,
	std::string	genres_name,
	std::string	songs_name,
	std::string	songs_artists_name,
	int		songs_tracknbr,
	std::string	songs_comment,
	std::string	songs_path,
	int		length,
	int		bitrate,
	int		samplerate,
	int		channels
	) : 
	m_albums_name(albums_name),
	m_albums_artist(albums_artist),
	m_albums_ntracks(albums_ntracks),
	m_albums_year(albums_year),
	m_directories_path(directories_path),
	m_genres_name(genres_name),
	m_songs_name(songs_name),
	m_songs_artists_name(songs_artists_name),
	m_songs_tracknbr(songs_tracknbr),
	m_songs_comment(songs_comment),
	m_songs_path(songs_path),
	m_audioProperties_length(length),
	m_audioProperties_bitrate(bitrate),
	m_audioProperties_samplerate(samplerate),
	m_audioProperties_channels(channels)
{
	// TODO : Request to the SQL database //
}


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
			if( it->first == "ALBUMARTIST" ) { m_albums_artist = it->second.toString().to8Bit(true); }
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

TagInfos::~TagInfos()
{

}


struct musicInfo TagInfos::getData()
{
	struct musicInfo toReturn;

	toReturn.name = m_songs_name;
	toReturn.album.name = m_albums_name;
	toReturn.album.albumArtist = m_albums_artist;
	toReturn.album.nTracks = m_albums_ntracks;
	toReturn.album.year = m_albums_year;
	toReturn.artist = m_songs_artists_name;
	toReturn.tracknbr = m_songs_tracknbr;
	toReturn.genre = m_genres_name;
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
	toReturn += "\tAlbum artist : " + m_albums_artist + "\n";
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
	sqlite3_stmt* requestStatement;
	int returnVal = 0;
	int i = 0;

	// Request : songs.name,albums.name,artists.name, genres.name,songs.tracknbr,albums.ntrack,albums.year,songs.comment,directories.path,songs.path
	// Request : songs.name,albums.name,artists.name, genres.name,songs.tracknbr,directories.path,songs.path
	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
	SELECT songs.name,albums.name,artists.name, genres.name,songs.tracknbr,directories.path,songs.path \
	FROM songs,albums,artists,directories,genres \
	WHERE songs.name=? AND albums.name=? AND genres.name=? AND songs.tracknbr=? AND directories.path=? AND songs.path=? \
		AND  songs.id_album=albums.id AND songs.id_genre=genres.id AND songs.id_artist=artists.name AND songs.id_dirName=directories.path", -1, &requestStatement, 0), 0);
	if(returnVal != SQLITE_OK) { return false; }


	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_songs_name.c_str(), -1, SQLITE_STATIC), 0) != SQLITE_OK) { return false; };
	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 2, m_albums_name.c_str(), -1, SQLITE_STATIC), 0) != SQLITE_OK) { return false; };
	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 3, m_genres_name.c_str(), -1, SQLITE_STATIC), 0) != SQLITE_OK) { return false; };
	if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 3, m_songs_tracknbr), 0) != SQLITE_OK) { return false; };
	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 4, m_directories_path.c_str(), -1, SQLITE_STATIC), 0) != SQLITE_OK) { return false; };
	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 5, m_songs_path.c_str(), -1, SQLITE_STATIC), 0) != SQLITE_OK) { return false; };

	do{
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement), 0);
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_DONE) { return false; }
		if(returnVal != SQLITE_DONE) { i++; }

	} while (returnVal != SQLITE_DONE);

	sqliteReturnVal(sqlite3_finalize(requestStatement), 0);


	if(i == 0)	// Sync necessary
	{
		if(!compareArtist(db)) { std::cout << "Artist not found, adding..." << std::endl; insertArtist(db); }
		if(!compareAlbum(db)) { std::cout << "Album not found, adding..." << std::endl; insertAlbum(db); }
	}

	return true;
}

bool TagInfos::insertArtist(sqlite3* db)
{
	sqlite3_stmt* requestStatement;
	int returnVal = 0;

	if(m_songs_artists_name == m_albums_artist)	// The same artist
	{
		returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "INSERT OR REPLACE INTO artists(name) VALUES (?)", -1, &requestStatement, 0), 0);

		if(returnVal != SQLITE_OK) { return false; }

		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_songs_artists_name.c_str(), -1, SQLITE_STATIC), 0) != SQLITE_OK) { return false; };
	}
	else
	{
		returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "INSERT OR REPLACE INTO artists(name) VALUES (?),(?)", -1, &requestStatement, 0), 0);

		if(returnVal != SQLITE_OK) { return false; }

		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_songs_artists_name.c_str(), -1, SQLITE_STATIC), 0) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 2, m_albums_artist.c_str(), -1, SQLITE_STATIC), 0) != SQLITE_OK) { return false; };
	}

	do{
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement), 0);
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_DONE && returnVal != SQLITE_BUSY) { return false; }

	} while (returnVal != SQLITE_DONE);

	sqlite3_finalize(requestStatement);
	return true;
}

bool TagInfos::insertAlbum(sqlite3* db)
{
	if(compareArtist(db))
	{
		sqlite3_stmt* requestStatement;
		int returnVal = 0;
		int artistId = getArtistId(db, m_albums_artist);

		returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "INSERT OR REPLACE INTO albums(id_artist, name, nTrack, year) VALUES (?,?,?,?)", -1, &requestStatement, 0), 0);

		if(returnVal != SQLITE_OK) { return false; }

		if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 1, artistId), 0) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 2, m_albums_name.c_str(), -1, SQLITE_STATIC), 0) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_int(requestStatement, 3, m_albums_ntracks), 0) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 4, m_albums_year.c_str(), -1, SQLITE_STATIC), 0) != SQLITE_OK) { return false; };

		do{
			returnVal = sqliteReturnVal(sqlite3_step(requestStatement), 0);
			if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_DONE && returnVal != SQLITE_BUSY) { return false; }

		} while (returnVal != SQLITE_DONE);

		sqlite3_finalize(requestStatement);
		return true;
	}
	else { std::cerr << "TagInfos::insertAlbum() : missing artist (compareArtist)" << std::endl; return false; }
}

int TagInfos::getArtistId(sqlite3* db, std::string artistName)
{
	sqlite3_stmt* requestStatement;
	int returnVal = 0;
	int artistId = 0;

	returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
	SELECT artists.id \
	FROM artists\
	WHERE artists.name=?", -1, &requestStatement, 0), 0);
 
	if(returnVal != SQLITE_OK) { return false; }

	if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, artistName.c_str(), -1, SQLITE_STATIC), 0) != SQLITE_OK) { return false; };

	do{
		returnVal = sqliteReturnVal(sqlite3_step(requestStatement), 0);
		if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_DONE && returnVal != SQLITE_BUSY) { return false; }
		if(returnVal != SQLITE_DONE)
		{
			artistId = sqlite3_column_int(requestStatement, 0);
		}

	} while (returnVal != SQLITE_DONE);

	sqliteReturnVal(sqlite3_finalize(requestStatement), 0);

	return artistId;
}

bool TagInfos::compareArtist(sqlite3* db)	// true -> OK
{
	// Single artist
	if(m_songs_artists_name != "")
	{
		sqlite3_stmt* requestStatement;
		int returnVal = 0;
		int i = 0;

		returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
		SELECT artists.name \
		FROM artists\
		WHERE artists.name=?", -1, &requestStatement, 0), 0);
 
		if(returnVal != SQLITE_OK) { return false; }

		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_songs_artists_name.c_str(), -1, SQLITE_STATIC), 0) != SQLITE_OK) { return false; };

		do{
			returnVal = sqliteReturnVal(sqlite3_step(requestStatement), 0);
			if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_DONE && returnVal != SQLITE_BUSY) { return false; }
			if(returnVal != SQLITE_DONE)
			{
				if(m_songs_artists_name != std::string((char*)sqlite3_column_text(requestStatement, 0)))
				{ return false; }
				i++;
			}

		} while (returnVal != SQLITE_DONE);

		if(i == 0)	// No result
		{ return false; }

		sqliteReturnVal(sqlite3_finalize(requestStatement), 0);
	}

	if(m_albums_artist != "")
	{
		sqlite3_stmt* requestStatement;
		int returnVal = 0;
		int i = 0;

		returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
		SELECT artists.name \
		FROM artists \
		WHERE artists.name=?", -1, &requestStatement, 0), 0);
 
		if(returnVal != SQLITE_OK) { return false; }

		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_albums_artist.c_str(), -1, SQLITE_STATIC), 0) != SQLITE_OK) { return false; };

		do{
			returnVal = sqliteReturnVal(sqlite3_step(requestStatement), 0);
			if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_DONE && returnVal != SQLITE_BUSY) { return false; }
			if(returnVal != SQLITE_DONE)
			{
				if(m_albums_artist != std::string((char*)sqlite3_column_text(requestStatement, 0)))
				{ return false; }
				i++;
			}

		} while (returnVal != SQLITE_DONE);

		if(i == 0)	// No result
		{ return false; }

		sqliteReturnVal(sqlite3_finalize(requestStatement), 0);
	}

	return true;
}

bool TagInfos::compareAlbum(sqlite3* db)	// true -> OK
{
	if(m_albums_name != "")
	{
		sqlite3_stmt* requestStatement;
		int returnVal = 0;
		int i = 0;

		returnVal = sqliteReturnVal(sqlite3_prepare_v2(db, "\
	SELECT albums.name,artists.name \
	FROM albums,artists \
	WHERE albums.name=? AND artists.name=? \
		AND  albums.id_artist=artists.id", -1, &requestStatement, 0), 0);

		if(returnVal != SQLITE_OK) { return false; }

		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 1, m_albums_name.c_str(), -1, SQLITE_STATIC), 0) != SQLITE_OK) { return false; };
		if(sqliteReturnVal(sqlite3_bind_text(requestStatement, 2, m_albums_artist.c_str(), -1, SQLITE_STATIC), 0) != SQLITE_OK) { return false; };

		do{
			returnVal = sqliteReturnVal(sqlite3_step(requestStatement), 0);
			if(returnVal != SQLITE_DONE && returnVal!= SQLITE_ROW && returnVal != SQLITE_DONE && returnVal != SQLITE_BUSY) { return false; }
			if(returnVal != SQLITE_DONE)
			{
				if(m_albums_name != std::string((char*)sqlite3_column_text(requestStatement, 0)))
				{ return false; }
				if(m_albums_artist != std::string((char*)sqlite3_column_text(requestStatement, 1)))
				{ return false; }
				i++;
			}

		} while (returnVal != SQLITE_DONE);

		if(i == 0)	// No result
		{ return false; }

		sqliteReturnVal(sqlite3_finalize(requestStatement), 0);
	}


	return true;
}

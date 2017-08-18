#ifndef TAGFCTS_HPP
#define TAGFCTS_HPP

#include "sqlite3.h"
#include <taglib/taglib.h>
#include <taglib/fileref.h>
#include <string>

// Data structures
struct albumInfos {
	std::string name;
	std::string albumArtist;
	int nTracks;
	std::string year;
};

struct songInfos {
	std::string name;
	struct albumInfos album;
	std::string artist;
	int tracknbr;
	std::string genre;
	std::string comment;
	std::string fullPath;
};

struct audioProperties {
	int length;	// In seconds
	int bitrate;	// kb/s
	int samplerate;	// Hz
	int channels;
};

// Class that store all values from the database. It can check and update it if necessary
class TagInfos
{
public:
	TagInfos(
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
		);
	TagInfos(TagLib::FileRef targetFile);
	TagInfos(std::string path) : TagInfos(TagLib::FileRef(path.c_str(), TagLib::AudioProperties::Average)) {}
	TagInfos(sqlite3* db, int songDbId);	// Creating an object from the DB, with only the song ID
	~TagInfos();

	// Accessors
	struct songInfos getData();
	struct audioProperties getAudioProperties();
	std::string getDir()		{ return m_directories_path; }
	std::string getFileName()	{ return m_songs_path; }

	// Special functions
	bool sync(sqlite3* db);				// Return true if a modification were done : Check all value of the original file, and updates the DB if necessarry (ex: values modified, file deltted, ...)
	bool insertAlbum(sqlite3* db);			// Adding the albums_name,albums_artists_name(int),albums_ntracks,albums_year to the database
	bool insertSongArtist(sqlite3* db);			// Adding the songs_artists_name and albums_artists_name to the database
	bool insertAlbumArtist(sqlite3* db);			// Adding the songs_artists_name and albums_artists_name to the database
	bool insertAudioProperties(sqlite3* db);	// Adding the audio properties to the database
	bool insertDirPath(sqlite3* db);		// Adding the directories_path to the database
	bool insertGenre(sqlite3* db);			// Adding the genre to thje database
	bool insertSong(sqlite3* db);			// Adding the songs to the database -> If compareSongPath return false
	bool updateSong(sqlite3* db);	/*TODO*/	// Updating the song, if already exists but doesn't fit with the acutal datas
	std::string toString();

	bool compareAlbum(sqlite3* db);			// If the album exists in the database
	bool compareSongArtist(sqlite3* db);		// If the artist (album and song) exists in the databse
	bool compareAlbumArtist(sqlite3* db);		// If the artist (album and song) exists in the databse
	bool compareGenre(sqlite3* db);			// If the genre exist in the database
	bool compareDirPath(sqlite3* db);		// Checking if the directory path exists in the database
	bool compareSongPath(sqlite3* db);		// Checking if the file name exists in the database
	bool compareSongData(sqlite3* db);		// If the song exist in the database with the same data
	bool compareAudioProperties(sqlite3* db);	// Check for the presence of an audioProperty in the database
	// if compareSongPath == true && compareSongData == false -> SQL:UPDATE
	

	// Static functions //
	static int getAlbumId(sqlite3* db, std::string albumName,std::string artistName, int nTracks, std::string date);
	static int getGenreId(sqlite3* db, std::string genreName);
	static int getArtistId(sqlite3* db, std::string artistName);
	static int getDirnameId(sqlite3* db, std::string dirName);
	static int getSongId(sqlite3* db, std::string dirPath, std::string songFileName);
		// For the constructor //
	static bool getAlbumInfosById(sqlite3* db, int albumId, struct albumInfos* infos);
	static bool getSongInfosById(sqlite3* db, int songId, struct songInfos* infos) ;
	static bool getAudioPropertiesById(sqlite3* db, int songId, struct audioProperties* infos);


private:
	// Database tags fields
		// albums table
	std::string	m_albums_name			= "";	// artists table
	std::string	m_albums_artists_name		= "";
	int		m_albums_ntracks		= 0;
	std::string	m_albums_year			= "";
		// directories table
	std::string	m_directories_path		= "";
		// genres table
	std::string	m_genres_name			= "";
		// songs table
	std::string	m_songs_name			= "";
	std::string	m_songs_artists_name		= "";	// With foreign key songs.id_artist	--> artist of this song
	int		m_songs_tracknbr		= 0;
	std::string	m_songs_comment			= "";
	std::string	m_songs_path			= "";
		// audioProperties table
	int		m_audioProperties_length	= 0;
	int		m_audioProperties_bitrate	= 0;
	int		m_audioProperties_samplerate	= 0;
	int		m_audioProperties_channels	= 0;
};
#endif

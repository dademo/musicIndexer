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

struct musicInfo {
	std::string name;
	struct albumInfos album;
	std::string artist;
	int tracknbr;
	std::string genre;
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
	std::string	songs_path,
	int		length,
	int		bitrate,
	int		samplerate,
	int		channels
		);
	TagInfos(TagLib::FileRef targetFile);
	TagInfos(std::string path) : TagInfos(TagLib::FileRef(path.c_str(), TagLib::AudioProperties::Average)) {}
	~TagInfos();

	// Accessors
	struct musicInfo getData();
	struct audioProperties getAudioProperties();
	std::string getDir()		{ return m_directories_path; }
	std::string getFileName()	{ return m_songs_path; }

	// Special functions
	bool sync(sqlite3* db);	// Return true if a modification were done : Check all value of the original file, and updates the DB if necessarry (ex: values modified, file deltted, ...)
	std::string toString();

private:
	// Database tags fields
		// albums table
	std::string	m_albums_name			= "";	// artists table
	std::string	m_albums_artist			= "";
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
	std::string	m_songs_path			= "";
		// audioProperties table
	int		m_audioProperties_length	= 0;
	int		m_audioProperties_bitrate	= 0;
	int		m_audioProperties_samplerate	= 0;
	int		m_audioProperties_channels	= 0;
};
#endif

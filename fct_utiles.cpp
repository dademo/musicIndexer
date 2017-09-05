#include <iostream>

#include "fct_utiles.hpp"

void clearLine(int nChar)
{
	std::cout << '\r';	// Returning to the beginning of the line
	for(int i=0; i < nChar; i++)
	{
		std::cout << ' ';
	}
}

struct songInfos genVoidStructSongInfos()
{
	struct songInfos toReturn;

	toReturn.name="";
		toReturn.album.name="";
		toReturn.album.albumArtist="";
		toReturn.album.nTracks=0;
		toReturn.album.year="";
	toReturn.artist="";
	toReturn.tracknbr=0;
	toReturn.genre="";
	toReturn.comment="";
	toReturn.fullPath="";

	return toReturn;
}

struct audioProperties genVoidStructAudioProperties()
{
	struct audioProperties toReturn;

	toReturn.length=0;
	toReturn.bitrate=0;
	toReturn.samplerate=0;
	toReturn.channels=0;
	toReturn.bpm=0;

	return toReturn;
}

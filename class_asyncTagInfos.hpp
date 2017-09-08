#ifndef CLASS_ASYNCTAGINFOS_HPP
#define CLASS_ASYNCTAGINFOS_HPP

#include <thread>
#include "class_tagInfos.hpp"

class ASyncTagInfos : public TagInfos
{
public:
	ASyncTagInfos(TagLib::FileRef targetFile);
	ASyncTagInfos(std::string path) : TagInfos(TagLib::FileRef(path.c_str(), TagLib::AudioProperties::Average)) {}
	ASyncTagInfos(sqlite3* db, int songDbId);	// Creating an object from the DB, with only the song ID
	ASyncTagInfos(struct songInfos songTagInfos, struct audioProperties songAudioProperties);
	virtual ~ASyncTagInfos();

	virtual bool sync(sqlite3* db);

	// Thread functions //
	void join();

private:
	std::thread* m_currentThread;
};

#endif

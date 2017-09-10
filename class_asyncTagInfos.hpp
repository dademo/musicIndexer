#ifndef CLASS_ASYNCTAGINFOS_HPP
#define CLASS_ASYNCTAGINFOS_HPP

//#include <future>
#include <thread>
#include "class_tagInfos.hpp"

class ASyncTagInfos : public TagInfos
{
public:
	ASyncTagInfos(TagLib::FileRef targetFile);
	ASyncTagInfos(std::string path);
	ASyncTagInfos(sqlite3* db, int songDbId);	// Creating an object from the DB, with only the song ID
	ASyncTagInfos(struct songInfos songTagInfos, struct audioProperties songAudioProperties);
	ASyncTagInfos(const TagInfos& orig);
	virtual ~ASyncTagInfos();

	virtual bool sync(sqlite3* db);

	// Thread functions //
	void join();

private:
	bool TagInfos_sync(sqlite3* db);	// Function that call TagInfos::sync()
	//std::future<bool> m_currentAction;
	std::thread* m_currentThread;
};

#endif

#include <iostream>
#include "class_asyncTagInfos.hpp"

void testfct(int j)
{
	for(int i(0); i < j; i++)
	{
		std::cout << i << std::endl;
	}
}

ASyncTagInfos::ASyncTagInfos(TagLib::FileRef targetFile) : TagInfos(targetFile), m_currentThread(NULL)
{

}

ASyncTagInfos::ASyncTagInfos(std::string path) : TagInfos(TagLib::FileRef(path.c_str(), TagLib::AudioProperties::Average)), m_currentThread(NULL)
{

}

ASyncTagInfos::ASyncTagInfos(sqlite3* db, int songDbId) : TagInfos(db, songDbId), m_currentThread(NULL)
{

}

ASyncTagInfos::ASyncTagInfos(struct songInfos songTagInfos, struct audioProperties songAudioProperties) : TagInfos(songTagInfos, songAudioProperties), m_currentThread(NULL)
{

}

ASyncTagInfos::ASyncTagInfos(const TagInfos& orig) : TagInfos(orig), m_currentThread(NULL)
{

}


ASyncTagInfos::~ASyncTagInfos()
{
	if(m_currentThread != NULL)	// Thread has not be joined
	{
		m_currentThread->join();
		delete m_currentThread;
	}
}


bool ASyncTagInfos::sync(sqlite3* db)
{
	if(m_currentThread == NULL)
	{
		try {
			//m_currentThread = new std::thread(&TagInfos::sync, this, db);
			m_currentThread = new std::thread(&ASyncTagInfos::TagInfos_sync, this, db);
		} catch (std::system_error e) {
			std::cerr << e.what() << std::endl;
		}
		return true;
	} else { return false; }
}

bool ASyncTagInfos::TagInfos_sync(sqlite3* db)
{
	return TagInfos::sync(db);
}

void ASyncTagInfos::join()
{
	if(m_currentThread != NULL)
	{
		try {
			m_currentThread->join();
		} catch (std::system_error e) {
			std::cerr << "join" << e.what() << std::endl;
		}
		delete m_currentThread; m_currentThread = NULL;
	}
}

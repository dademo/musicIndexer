#include <iostream>
#include "class_asyncTagInfos.hpp"

ASyncTagInfos::ASyncTagInfos(TagLib::FileRef targetFile) : TagInfos(targetFile), m_currentThread(NULL)
{

}

ASyncTagInfos::ASyncTagInfos(sqlite3* db, int songDbId) : TagInfos(db, songDbId), m_currentThread(NULL)
{

}

ASyncTagInfos::ASyncTagInfos(struct songInfos songTagInfos, struct audioProperties songAudioProperties) : TagInfos(songTagInfos, songAudioProperties), m_currentThread(NULL)
{

}

ASyncTagInfos::~ASyncTagInfos()
{
	if(m_currentThread == NULL) { delete m_currentThread; }
}

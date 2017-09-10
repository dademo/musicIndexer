#include <iostream>

#include "fileManip.hpp"
#include "tagfcts.hpp"

std::vector<TagInfos> fastGetAllTags(std::string basePath, bool recursive)
{
	std::vector<std::string> fileList = getFileList(basePath, recursive);
	std::vector<TagInfos> allMyTags;

	int totalCount=fileList.size(), count=0;

	for(std::vector<std::string>::iterator it = fileList.begin(); it != fileList.end(); it++)
	{
		count ++;
		std::cout << "\rAnalyzing ... (" << count << " of " << totalCount << ")" << std::flush;
		allMyTags.push_back(TagInfos(basePath + "/" + *it));
	}
	std::cout << std::endl;

	return allMyTags;
}


std::vector<TagInfos> fullGetAllTags(std::string basePath, bool recursive)
{
		std::vector<std::string> fileList = getFileList(basePath, recursive);
		std::map<std::string, std::vector<std::string>> allDirectories = mapFileList(fileList);
		std::vector<TagInfos> allMyTags;

	// it -> mapped data => (*it).first == key == directory && (*it).second == vector data; it2 -> vector array data == file name
	for(std::map<std::string, std::vector<std::string>>::iterator it = allDirectories.begin(); it != allDirectories.end(); it++)
	{
		int i = 0;
		std::cout << "Analyzing : " << basePath + "/" + (*it).first << std::endl;
		for(std::vector<std::string>::iterator it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
		{
			i++;
			std::cout << "  (" << i << "/" << (*it).second.size() << ")  " << (*it2) << std::endl;
			allMyTags.push_back(TagInfos(basePath + "/"  + (*it).first + "/" + (*it2)));

		}
	}

	return allMyTags;
}

std::vector<ASyncTagInfos> fastGetAllTags_ASync(std::string basePath, bool recursive)
{
	std::vector<TagInfos> allMyTags = fastGetAllTags(basePath, recursive);
	std::vector<ASyncTagInfos> finalAllMytags;

	for(std::vector<TagInfos>::iterator it = allMyTags.begin(); it != allMyTags.end(); it++)
	{
		finalAllMytags.push_back(ASyncTagInfos(*it));
	}
	return finalAllMytags;
}

std::vector<ASyncTagInfos> fullGetAllTags_ASync(std::string basePath, bool recursive)
{
	std::vector<TagInfos> allMyTags = fullGetAllTags(basePath, recursive);
	std::vector<ASyncTagInfos> finalAllMytags;

	for(std::vector<TagInfos>::iterator it = allMyTags.begin(); it != allMyTags.end(); it++)
	{
		finalAllMytags.push_back(ASyncTagInfos(*it));
	}
	return finalAllMytags;
}

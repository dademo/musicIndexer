#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>

#include "fileManip.hpp"

std::vector<std::string> getFileList(std::string path, bool recursive)
{
	std::vector<std::string> toReturn, dirList;
	DIR* currentDir;
	struct dirent* currentFile;


	currentDir = opendir(path.c_str());
	if(currentDir == 0)		{ perror(std::string("getFileList->opendir() : " + path).c_str()); }
	else
	{
		while((currentFile = readdir(currentDir)) != 0)
		{
			//std::cout << currentFile->d_name << " : ";
			//std::cout << ( (isDir(path + "/" + currentFile->d_name))? "Is a directory" : "Not a directory" ) << std::endl;
			if(std::string(currentFile->d_name) != "." && std::string(currentFile->d_name) != "..")
			{
				if(!isDir(path + "/" + currentFile->d_name))
				{
					//std::cout << "Not a dir : " << path + "/" + currentFile->d_name << std::endl;
					bool ok = false;
					std::string extension = getFileLastExtension(currentFile->d_name);
					for(int i=0; i < 18; i++)
					{
						if(extension == supportedExtensions[i])
						{ ok = true; break; }
					}
					if(ok) {toReturn.push_back(currentFile->d_name); }
				}
				else { dirList.push_back(currentFile->d_name); }
			}
		}

		if(closedir(currentDir) == -1)	{ perror("getFileList->closedir()"); }


		for(std::vector<std::string>::iterator it = dirList.begin(); it != dirList.end(); it++)
		{
			std::vector<std::string> tmpVector = getFileList(path + "/" + *it, recursive);
			for(std::vector<std::string>::iterator itb = tmpVector.begin(); itb != tmpVector.end(); itb++)
			{
				toReturn.push_back(*it + "/" + *itb);
			}
		}
		//toReturn.push_back(val);

	}

	return toReturn;
}

std::map<std::string, std::vector<std::string>> mapFileList(std::vector<std::string> fileList)
{
	std::map<std::string,std::vector<std::string>> toReturn;

	for(std::vector<std::string>::iterator it = fileList.begin(); it != fileList.end(); it++)
	{
		std::string::size_type tmpPos = (*it).rfind("/");
		std::string fileDir = (*it).substr(0, (tmpPos > (*it).length())? 0 : tmpPos);
		toReturn[fileDir].push_back(
			(*it).substr( (tmpPos > (*it).length() )? 0 : tmpPos+1, std::string::npos)	// +1 to ignore the "/"
					);
	}

	//temp.substr(0,(pos > temp.length())? 0:pos)

	return toReturn;
}

bool fexists(std::string path)
{
	struct stat dirstat;

	if(stat(path.c_str(), &dirstat) != 0)
	{
		perror(std::string("fexists() : " + path).c_str());
		return false;
	}
	else { return true; }
}

bool isDir(std::string path)	// see man 2 stat
{
	struct stat dirStat;

	stat(path.c_str(), &dirStat);
	return S_ISDIR(dirStat.st_mode);
	//std::cout << "isDir : " << dirStat.st_mode << " <> " << S_ISDIR(dirStat.st_mode) << std::endl;

	//return true;
}


std::string getFileLastExtension(std::string file)
{
	std::string extension = file.substr(file.rfind(".")+1);

	return extension;
}


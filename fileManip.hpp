#ifndef FILEMANIP_HPP
#define FILEMANIP_HPP

#include <vector>
#include <string>

std::vector<std::string> getFileList(std::string dirPath, bool recursive);
bool fexists(std::string path);
bool isDir(std::string path);
std::string getFileLastExtension(std::string file);

#endif

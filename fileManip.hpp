#ifndef FILEMANIP_HPP
#define FILEMANIP_HPP

#include <vector>
#include <map>
#include <string>

const std::string supportedExtensions[18] = {"mp3", "ogg", "spx", "mpc", "ape", "flac", "wv", "tta", "wma", "m4a", "m4b", "m4p", "mp4", "3g2", "wav", "aif", "aiff", "opus"};

std::vector<std::string> getFileList(std::string dirPath, bool recursive);
std::map<std::string, std::vector<std::string>> mapFileList(std::vector<std::string> fileList);
bool fexists(std::string path);
bool isDir(std::string path);
std::string getFileLastExtension(std::string file);

#endif

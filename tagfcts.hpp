#ifndef TAGFCTS_HPP
#define TAGFCTS_HPP

#include <string>
#include <vector>

#include "class_tagInfos.hpp"

std::vector<TagInfos> fastGetAllTags(std::string basePath, bool recursive = true);
std::vector<TagInfos> fullGetAllTags(std::string basePath, bool recursive = true);

#endif

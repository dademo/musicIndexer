#ifndef MUSIC_SQLTABLES_HPP
#define MUSIC_SQLTABLES_HPP

#include "sqlite3.h"

int createTables(sqlite3* db);
void checkTables(sqlite3* db);

#endif

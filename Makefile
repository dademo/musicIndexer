# Variables
# Note : 
## Requirements for C++ : -lstdc++
## Requirements for sqlite : -pthread -ldl
CC=gcc
CFLAGS=--std=c11 -pthread -ldl -W -Wall -pedantic
CXXFLAGS=--std=c++11 -lstdc++ -pthread -ldl -ltag -laubio -W -Wall -pedantic
EXEC=musicIndexer

RED	=	"\e[1;31m"
WHITE	=	"\e[0m"

SRC=main.cpp sqlite3.cpp mySQLiteFunctions.cpp music_sqlTables.cpp fileManip.cpp tagfcts.cpp class_tagInfos.cpp fct_utiles.cpp aubiofcts.cpp
OBJ=$(SRC:.cpp=.o) $(CRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $(EXEC) $(OBJ) $(CXXFLAGS)

main.o: main.cpp
	$(CC) -c main.cpp $(CXXFLAGS)

#%.o: %.c %.h
# For the '.cpp' files
%.o: %.cpp
	$(CC) -c $^ $(CXXFLAGS)

# For the '.c' files
%.o: %.c
	$(CC) -c $^ $(CFLAGS)

# Exécution systématique                                                                                                                                                                      
.PHONY: clean

# Nettoyage                                                                                                                                                                                   
clean:
	@`which echo` -e $(RED)"[RM] "$(OBJ)$(WHITE)
	@rm -Rv *.o

fclean: clean
	@`which echo` -e $(RED)"[RM] "$(EXEC)$(WHITE)
	@rm -Rv $(EXEC)

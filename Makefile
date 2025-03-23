# Nom de l'exécutable
TARGET = parser

# Compilateur
CXX = g++

# Options de compilation
CXXFLAGS = -Wall -Wextra -std=c++17

# Fichiers sources
SRCS = LuaBParser.cpp LuaInstructions.cpp LuaRunner.cpp LuaChunk.cpp

# Fichiers objets générés (remplace .cpp par .o)
OBJS = $(SRCS:.cpp=.o)

# Règle principale : compiler l'exécutable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compilation des fichiers .cpp en .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage des fichiers compilés
clean:
	rm -f $(TARGET) $(OBJS)

CXX = g++
CXXFLAGS = -Wall -std=c++17 -O2
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
TRMT_FLAGS = -std=c++17 -O3

# Source definitions
GAME_SRC = main.cpp board.cpp movegen.cpp
ARENA_SRC = bot_main.cpp board.cpp movegen.cpp
TRMT_SRC = tournament.cpp board.cpp movegen.cpp


# Distinct object mapping
GAME_OBJ = $(GAME_SRC:.cpp=.o)
ARENA_OBJ = $(ARENA_SRC:.cpp=.o)
TRMT_OBJ = $(TRMT_SRC:.cpp=.o)

EXEC = chess_game bot_arena engine_trmt

all: $(EXEC)

# Target for human-playable game
chess_game: $(GAME_OBJ)
	$(CXX) $(GAME_OBJ) -o $@ $(LDFLAGS)

# Target for bot tournament arena
bot_arena: $(ARENA_OBJ)
	$(CXX) $(ARENA_OBJ) -o $@ $(LDFLAGS)

engine_trmt: $(TRMT_OBJ)
	$(CXX) $(TRMT_OBJ) -o $@ $(TRMT_FLAGS) $(LDFLAGS)
# Pattern rule to compile individual source files into objects
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(GAME_OBJ) $(ARENA_OBJ) $(TRMT_OBJ) $(EXEC)

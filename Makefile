CXX = g++
# Upgraded from -O2 to -O3 globally so ALL objects are compiled at max speed
CXXFLAGS = -Wall -std=c++17 -O3 
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Source definitions
GAME_SRC = main.cpp board.cpp movegen.cpp
ARENA_SRC = bot_main.cpp board.cpp movegen.cpp
TRMT_SRC = tournament.cpp board.cpp movegen.cpp
TEST_SRC = test.cpp board.cpp movegen.cpp

# Distinct object mapping
GAME_OBJ = $(GAME_SRC:.cpp=.o)
ARENA_OBJ = $(ARENA_SRC:.cpp=.o)
TRMT_OBJ = $(TRMT_SRC:.cpp=.o)
TEST_OBJ = $(TEST_SRC:.cpp=.o)

EXEC = chess_game bot_arena engine_trmt test

all: $(EXEC)

# Target for human-playable game
chess_game: $(GAME_OBJ)
	$(CXX) $(GAME_OBJ) -o $@ $(LDFLAGS)

# Target for bot tournament arena
bot_arena: $(ARENA_OBJ)
	$(CXX) $(ARENA_OBJ) -o $@ $(LDFLAGS)

# Target for tournament engine
engine_trmt: $(TRMT_OBJ)
	$(CXX) $(TRMT_OBJ) -o $@ $(LDFLAGS)

# Target for testing correctness (Headless Perft)
test: $(TEST_OBJ)
	$(CXX) $(TEST_OBJ) -o $@ $(LDFLAGS)

# Pattern rule to compile individual source files into objects
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXEC)

CXX = g++
CXXFLAGS = -Wall -std=c++17 -O2
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC = main.cpp board.cpp movegen.cpp
OBJ = $(SRC:.cpp=.o)
EXEC = chess_engine

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

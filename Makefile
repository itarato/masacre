UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    LIBS=-lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

ifeq ($(UNAME_S),Darwin)
    LIBS=-lm -lpthread -ldl -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib`
endif

CXXFLAGS=-std=c++2a -Wall -pedantic -Wformat -Werror -Wshadow

BIN=main

SRC=$(wildcard src/*.cpp)
OBJ=$(addsuffix .o,$(basename $(SRC)))

all: executable

debug: CXXFLAGS += -DDEBUG -g -O0
debug: executable

executable: $(OBJ)
	$(CXX) -o $(BIN) $^ $(CXXFLAGS) $(LIBS)

test_pf: src/tests/pf_test.cpp
	$(CXX) $(CXXFLAGS) -o test_pf $^ $(LIBS)

clean:
	rm -f ./src/*.o
	rm -f ./src/*.out
	rm -f ./src/tests/*.o
	rm -f ./src/tests/*.out
	rm -f ./$(BIN)
	rm -f ./test_pf

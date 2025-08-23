UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    LIBS=-lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

ifeq ($(UNAME_S),Darwin)
    LIBS=-lm -lpthread -ldl -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib`
endif

CXXFLAGS=-std=c++2a -Wall -pedantic -Wformat -Werror

BIN=main

SRC=$(wildcard src/*.cpp)
OBJ=$(addsuffix .o,$(basename $(SRC)))

all: executable

debug: CXXFLAGS += -DDEBUG -g
debug: executable

executable: $(OBJ)
	$(CXX) -o $(BIN) $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f ./src/*.o
	rm -f ./src/*.out

clean_all:
	rm -f ./src/*.o
	rm -f ./src/*.out
	rm -f ./$(BIN)

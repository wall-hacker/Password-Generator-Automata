# Makefile

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Ofast

build: main

run:
	./main

main: main.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f main
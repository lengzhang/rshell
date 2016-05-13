all: rshell

rshell: src/main.cpp
	mkdir bin
	g++ -Wall -Werror -ansi -pedantic src/main.cpp -o bin/rshell

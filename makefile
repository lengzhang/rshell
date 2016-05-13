all: rshell

rshell: src/main.cpp
	g++ -Wall -Werror -ansi -pedantic src/main.cpp -o rshell

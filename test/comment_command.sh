#!/bin/sh
g++ -Wall -Werror -ansi -pedantic ../src/main.cpp

str="echo unhappy # this command will print out unhappy; exit # quit"

echo ${str}
{
	echo ${str}
} | ./a.out

rm a.out

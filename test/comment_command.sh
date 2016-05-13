#!/bin/sh
g++ -Wall -Werror -ansi -pedantic ../src/main.cpp

str="echo unhappy #11221; exit"

echo ${str}
{
	echo ${str}
} | ./a.out

rm hello a.out

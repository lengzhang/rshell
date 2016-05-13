#!/bin/sh
g++ -Wall -Werror -ansi -pedantic ../src/main.cpp -o test

echo "----------"
str="ls -a; echo hello && rm hello || echo unhappy; exit"

echo ${str}
{
	echo ${str}
	sleep 1
} | ./test

echo "----------"
str="ls -a; echo hello && touch hello || echo unhappy; ls &&  exit"

echo ${str}
{
        echo ${str}
	sleep 1
} | ./test

rm hello test

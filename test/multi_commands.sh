#!/bin/sh
g++ -Wall -Werror -ansi -pedantic ../src/main.cpp

str="ls -a; echo hello && rm hello || echo unhappy; exit"

echo ${str}
{
	echo ${str}
} | ./a.out

str="ls -a; echo hello && touch hello || echo unhappy; ls &&  exit"

echo ${str}
{
        echo ${str}
} | ./a.out

rm hello a.out

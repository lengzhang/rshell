#!/bin/sh
g++ -Wall -Werror -ansi -pedantic ../src/main.cpp

str1="echo A"
str2="ls -a"
str3="exit"

{
	echo ${str1}
	echo ${str2}
	echo ${str3}
} | ./a.out

rm a.out

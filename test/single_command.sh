#!/bin/sh
g++ -Wall -Werror -ansi -pedantic ../src/main.cpp -o test

echo "----------"
str1="echo A"
str2="ls -a"
str3="exit"

{
	echo ${str1}
	sleep 1
	echo ${str2}
	sleep 1
	echo ${str3}
	sleep 1
} | ./test

rm test

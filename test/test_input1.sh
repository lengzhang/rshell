#!/bin/sh
g++ -Wall -Werror -ansi -pedantic ../src/main.cpp -o test.out

str="echo Hi Hi; test input1.txt && cat < input1.txt | tr A-Z a-z | tr a-z A-Z && echo Thank You"
{
	echo ${str}
	sleep 1
	echo quit
} | ./test.out

rm test.out
echo "\nrm test.out" 

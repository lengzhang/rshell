#!/bin/sh
g++ -Wall -Werror -ansi -pedantic ../src/main.cpp -o test.out

echo "----------"
str="echo \"F- grade\" >&1 | sed \"s/F-/A+/\""
{
	echo ${str}
	sleep 1
	echo quit
} | ./test.out

rm test.out
echo "\nrm test.out" 


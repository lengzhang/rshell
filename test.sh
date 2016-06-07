#!/bin/sh
g++ -Wall -Werror -ansi -pedantic src/main.cpp -o test.out

echo "----------"
str="cat < test1.txt | tr A-Z a-z | tee test2.txt | tr a-z A-Z >> test3.txt"
#str="cat < test1.txt | tr A-Z a-z | tee test2.txt | tr a-z A-Z > test3.txt"
#str="cat < test1.txt | tr A-Z a-z | tee test2.txt"
#str="cat < test1.txt | tr A-Z a-z | tr a-z A-Z"
#str="cat < test1.txt | tr A-Z a-z"
#str="cat < test1.txt"
#str="ls > test2.txt"
{
	echo ${str}
	sleep 1
} | ./test.out

rm test.out
echo "\nrm test.out" 

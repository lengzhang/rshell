#!/bin/sh
g++ -Wall -Werror -ansi -pedantic src/main.cpp -o test.out

echo "----------"
#str="ls && echo a || echo b"
str="ls && cat < test/test1.txt | tr A-Z a-z | tee test/test2.txt | tr a-z A-Z >> test/test3.txt && echo a"
#str="ls && cat < test1.txt | tr A-Z a-z && echo a"
#str="cat < test/test1.txt | tr A-Z a-z | tee test/test2.txt | tr a-z A-Z >> test/test3.txt"
#str="cat < test/test1.txt | tr A-Z a-z | tee test/test2.txt | tr a-z A-Z > test/test3.txt"
#str="cat < test/test1.txt | tr A-Z a-z | tee test/test2.txt"
#str="cat < test/test1.txt | tr A-Z a-z | tr a-z A-Z"
#str="cat < test/test1.txt | tr A-Z a-z"
#str="cat < test/test1.txt"
#str="ls ./ | tee test/test2.txt"
{
	echo ${str}
	sleep 1
	echo quit
} | ./test.out

rm test.out
echo "\nrm test.out" 

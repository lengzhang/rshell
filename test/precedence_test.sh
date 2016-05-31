#!/bin/sh
g++ -Wall -Werror -ansi -pedantic ../src/main.cpp -o test

echo "----------"
str="echo 1 && echo 2 || echo 3 && echo 4 || echo 5"

echo ${str}
{
	echo ${str}
	sleep 1
} | ./test

echo "----------"
str="(echo 1 && echo 2) || ((echo 3 && echo 4) || echo 5)"

echo ${str}
{
        echo ${str}
	sleep 1
} | ./test

echo "----------"
str="echo 1 && echo 2 || (echo 3 && echo 4 || echo 5)"

echo ${str}
{
echo ${str}
sleep 1
} | ./test

rm hello test

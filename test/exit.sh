#!/bin/sh
g++ -Wall -Werror -ansi -pedantic ../src/main.cpp -o test

echo "----------"
str="exit"

echo ${str}
{
echo ${str}
sleep 1
} | ./test


rm hello test

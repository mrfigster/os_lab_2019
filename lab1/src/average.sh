#!/bin/bash

sum=0

for val in "$@"
do
sum=$(($sum + $val))
done

sum=$(($sum / $#))

echo $sum

echo Кол-во параметров: $# 
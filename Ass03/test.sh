#!/bin/sh

input1=${1}
input2=${2}
for i in $(seq 1 $input1)
do
	for j in $(seq 1 $input2)
	do
		printf "$i * $j = $n"
		n= expr $i '*' $j
	done
echo "  "
done

exit 0

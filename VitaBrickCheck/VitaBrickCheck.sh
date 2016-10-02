#!/bin/bash

if [ $# -ne 1 ]
then
	echo -e "\nUsage: ./VitaBrickCheck.sh path_of_uncompressed_vpk"
	echo -e "Example ./VitaBrickCheck.sh /home/user/myvpk\n"
	exit 0
fi

IFS=$'\n'

for file in $(find "$1" -iname "*.bin" -or -iname "*.suprx" -or -iname "*.self")
do
	egrep -i "vs0|os0" "$file"
done

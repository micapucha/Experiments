#!/bin/bash

if [ $# -ne 1 ]
then
	echo -e "\nUsage: ./VitaBrickCheck.sh path_of_vpk_or_uncompressed_dir"
	echo -e "Example ./VitaBrickCheck.sh /home/user/myvpk.vpk\n"
	exit 0
fi

IFS=$'\n'

path="$1"
unzipped=0
if [ $(echo "$1" | grep -i -c ".vpk$") -gt 0 ]
then
	echo "Decompressing VPK"
	path=$(mktemp -d)
	unzip "$1" -d "$path" >/dev/null
	unzipped=1
fi

n=0
for file in $(find "$path" -iname "*.bin" -or -iname "*.suprx" -or -iname "*.self")
do
	if [ $(egrep -c -i "vs0|os0" "$file") -gt 0 ]
	then
		echo "WARNING: '$file' is suspicious"
		n=$(( $n + 1))
	fi
done

if [ $unzipped -eq 1 ]
then
	rm -rf "$path"
fi

echo -e "\nExecution finished for $1.\n$n suspicious file(s) found.\n"

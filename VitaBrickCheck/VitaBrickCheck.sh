#!/bin/bash
set -e

if [ $# -eq 0 ]
then
	echo -e "\nUsage:   ./VitaBrickCheck.sh path_of_vpk_or_uncompressed_dir [...]\n"
	echo -e "Example: ./VitaBrickCheck.sh /home/user/myvpk.vpk"
	echo -e "         ./VitaBrickCheck.sh /home/user/*.vpk\n"
	exit 0
fi

IFS=$'\n'

for param in "$@"
do
	path="$param"
	unzipped=0
	if [ $(echo "$param" | grep -i -c ".vpk$") -gt 0 ]
	then
		echo "Decompressing VPK"
		path=$(mktemp -d)
		unzip -o "$param" -d "$path" >/dev/null
		unzipped=1
	fi

	n=0
	for file in $(find "$path" -iname "*.bin" -or -iname "*.suprx" -or -iname "*.self")
	do
		if [ $(egrep -c -i "vs0|os0|vshIoMount" "$file") -gt 0 ]
		then
			echo "WARNING: '$file' is suspicious"
			n=$(( $n + 1))
		fi
	done

	if [ $unzipped -eq 1 ]
	then
		rm -rf "$path"
	fi

	echo -e "\nExecution finished for $param.\n$n suspicious file(s) found.\n"
done

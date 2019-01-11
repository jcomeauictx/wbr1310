#!/bin/sh

if [ "$1" = "" -o "$2" = "" ]; then
	echo "Usage: signwebs.sh {target file} {signature}"
	exit 1
fi

OLD=`grep image_sign $1`
if [ "$OLD" != "" ]; then
	echo -e "\033[32mImage Signature : [$2]\033[0m"
	sed "s/$OLD/image_sign=$2/" $1 > temp.conf
	rm -f $1
	mv temp.conf $1
fi

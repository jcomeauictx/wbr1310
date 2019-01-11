#!/bin/sh

if [ "$1" = "" ]; then
	echo "Usage: mark.sh {config file} {build no.} [version string]"
	exit 1
fi

OLD=`grep BUILD_NUMBER $1`
if [ "$OLD" != "" ]; then
	sed "s/$OLD/BUILD_NUMBER=$2/" $1 > temp.conf
	rm -f $1
	mv temp.conf $1
	echo -e "\033[32mPatch build number to [$2]\033[0m"
fi
echo $2 > buildno
if [ "$3" != "" ]; then
	OLD=`grep BUILD_VERSION $1`
	if [ "$OLD" != "" ]; then
		sed "s/$OLD/BUILD_VERSION=$3/" $1 > temp.conf
		rm -f $1
		mv temp.conf $1
		echo -e "\033[32mPatch version number to [$3].\033[0m"
	fi
fi

echo "BUILD_DATE=`date +\"%a %d %b %Y\"`" >> $1
echo -e "\033[32mPatch build date to [$2]\033[0m"

exit 0

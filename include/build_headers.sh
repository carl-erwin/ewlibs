#!/bin/sh

#set -v

LICENSE_HEADER=


string_capitalize()
{
    A=$(echo -n "$1" | cut -b1  /dev/stdin  | tr [:lower:] [:upper:])
    B=$(echo -n "$1" | cut -b2- /dev/stdin)
    echo -n "${A}${B}"
}

build_header_list()
{
    cd $1 || { return ; }


    local CURDIR_BASENAME
    CURDIR_BASENAME=$(basename $(pwd))

    local FILES;

    DIR=""
    for dir in * ; do
	if [ -d $dir ] ; then 
	    echo dir = $dir
	    ( build_header_list $dir )
	fi
    done    

    local FILES

    for i in * ; do
	if [ -f "$i" ] ; then
	    case $i in
		*.sh) : ;;
		*~) : ;;
		*)
		FILES="$FILES $i "
		;;
	    esac
	fi
    done

    local HDR
    HDR=$(echo -n $CURDIR_BASENAME | sed 's,\(.\)\(.*\),\1\2,g')

    HDR=$(string_capitalize $HDR)

    echo '/*' > "../${HDR}.hpp"
    echo "" >> "../${HDR}.hpp"
    cat ${LICENSE_HEADER_FILE} >> "../${HDR}.hpp"
    echo "" >> "../${HDR}.hpp"
    echo '*/' >> "../${HDR}.hpp"
    echo "" >> "../${HDR}.hpp"
    echo "" >> "../${HDR}.hpp"
    echo \#pragma once >> "../${HDR}.hpp"

    for file in $FILES
    do
	echo \#include \"${CURDIR_BASENAME}/$file\" >> ../${HDR}.hpp
    done

    cd -
}

cd $(dirname $0)
WORK_DIR=$PWD

cd ../
LICENSE_HEADER_FILE="${PWD}/LICENSE_HEADER"

echo "LICENSE_HEADER_FILE = '${LICENSE_HEADER_FILE}'"

cd $WORK_DIR


# clean previous genereted files
find . -type f -name '[A-Z]*.hpp'  |
while read file
do
 echo removing $file
 rm -f $file
done

build_header_list ew

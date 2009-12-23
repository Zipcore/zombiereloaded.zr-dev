#!/bin/sh

# Note: Copy this script to the source code repository and execute it
#       from that location.

# Program for printing date.
ZR_DATEPATH='date'

if [ "$1" ]
then
    ZR_DATEPATH=$1
fi

ZR_VERSION_FILE="src/zr/hgversion.h.inc"

ZR_PRODUCT_NAME="Zombie:Reloaded"
ZR_COPYRIGHT="Copyright (C) 2009  Greyscale, Richard Helgeby"
ZR_BRANCH="zr-3.0-dev"
ZR_LICENSE="GNU GPL, Version 3"
ZR_DATE=$($ZR_DATEPATH -R)

echo "#define ZR_VER_PRODUCT_NAME     \"$ZR_PRODUCT_NAME\"" > $ZR_VERSION_FILE
echo "#define ZR_VER_COPYRIGHT        \"$ZR_COPYRIGHT\"" >> $ZR_VERSION_FILE
echo "#define ZR_VER_VERSION          VERSION" >> $ZR_VERSION_FILE
echo "#define ZR_VER_BRANCH           \"$ZR_BRANCH\"" >> $ZR_VERSION_FILE
#echo "#define ZR_VER_BRANCH           \"$(hg id -b)\"" >> $ZR_VERSION_FILE
echo "#define ZR_VER_REVISION         \"$(hg id -n):$(hg id -i)\"" >> $ZR_VERSION_FILE
echo "#define ZR_VER_LICENSE          \"$ZR_LICENSE\"" >> $ZR_VERSION_FILE
echo "#define ZR_VER_DATE             \"$ZR_DATE\"" >> $ZR_VERSION_FILE

echo "Updated $ZR_VERSION_FILE"

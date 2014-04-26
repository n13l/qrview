#!/bin/sh
MAJOR=`echo $1 | cut -f1 -d"."`
MINOR=`echo $1 | cut -f2 -d"."`
PATCH=`echo $1 | cut -f3 -d"."`
if [ "x$with_patchlevel" != "x" ] ; then
	printf "%02d%02d%02d\\n" $MAJOR $MINOR $PATCHLEVEL
else
	if [ "x$nice" != "x" ] ; then
	printf "%d.%d\\n" $MAJOR $MINOR
        else
	if [ "x$PATCH" != "x" ] ; then
	printf "%02d%02d%02d\\n" $MAJOR $MINOR $PATCH
	else
	printf "%02d%02d\\n" $MAJOR $MINOR
	fi
	fi
fi

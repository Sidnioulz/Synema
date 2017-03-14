#!/bin/sh

if [ -e BUILD ]; then
	export SYNEMA_MACHINES_DIR="${PWD}/machines"
	export SYNEMA_DATA_DIR="${PWD}/BUILD/share/synema"
	export LD_LIBRARY_PATH="${PWD}/BUILD/lib"
	if [ "$1" = "gdb" ]; then
		gdb ./BUILD/bin/synema
	else
		./BUILD/bin/synema
	fi
else
	echo "This script requires that you made a BUILD directory and then typed:
  cd BUILD && cmake .. && make"
fi


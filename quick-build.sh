#!/bin/sh

if [ -e BUILD ]; then
	echo ""
else
	mkdir BUILD
fi

cd BUILD &&
cmake -DCMAKE_INSTALL_PREFIX="$PWD" $* .. &&
make &&
make install 1>/dev/null &&
exit

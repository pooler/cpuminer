#!/bin/sh

PATH=$PATH:/usr/i686-pc-mingw32/sys-root/mingw/bin	\
	nsiswrapper --run \
		--name "CPU miner" \
		--outfile cpuminer-installer.exe \
		minerd.exe

#!/bin/sh

MINGW_PATH=/usr/i686-pc-mingw32/sys-root/mingw/bin

PATH=$PATH:$MINGW_PATH \
    nsiswrapper --run \
	--name "CPU miner" \
	--outfile cpuminer-installer.exe \
	minerd.exe \
	$MINGW_PATH/libcurl-4.dll=libcurl-4.dll	\
	$MINGW_PATH/pthreadgc2.dll=pthreadgc2.dll \
	$MINGW_PATH/libidn-11.dll=libidn-11.dll \
	$MINGW_PATH/libssh2-1.dll=libssh2-1.dll \
	$MINGW_PATH/libssl-10.dll=libssl-10.dll \
	$MINGW_PATH/zlib1.dll=zlib1.dll \
	$MINGW_PATH/libcrypto-10.dll=libcrypto-10.dll \
	$MINGW_PATH/libiconv-2.dll=libiconv-2.dll \
	$MINGW_PATH/libintl-8.dll=libintl-8.dll



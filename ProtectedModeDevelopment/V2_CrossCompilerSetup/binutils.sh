#!/bin/bash
cd $HOME/src

mkdir build-binutils

../binutils-2.35/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror

make
make install
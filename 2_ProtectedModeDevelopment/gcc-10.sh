#!/bin/bash
cd $HOME/src

mkdir build-gcc
cd build-gcc

../gcc-10.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers

make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc




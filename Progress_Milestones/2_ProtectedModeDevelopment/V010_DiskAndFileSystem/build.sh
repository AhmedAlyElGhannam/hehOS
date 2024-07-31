#!/bin/bash
# already exported all environment variables into .bashrc so there is no need to do it here
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
make all
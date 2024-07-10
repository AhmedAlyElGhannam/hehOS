1. Echo the following lines into `.bashrc` then source `.bashrc`.
    ```
    export PREFIX="$HOME/opt/cross"
    export TARGET=i686-elf
    export PATH="$PREFIX/bin:$PATH"
    ```
2. Download `binutils-v2.35.tar.xz` from this link: https://ftp.gnu.org/gnu/binutils/binutils-2.35.tar.xz
3. Extract `binutils-v2.35.tar.xz` into your HOME directory.
4. Create a new folder `src` and move the `binutils-v2.35` folder into it.
5. Run the bash script `binutils.sh`.
6. Download `gcc-10.2.0` from this link: https://ftp.gnu.org/gnu/gcc/gcc-10.2.0/gcc-10.2.0.tar.gz
7. Extract `gcc-10.2.0.tar.gz` into the $HOME/src folder created before.
8. Run the script `gcc-10.sh`.
9. To make sure `gcc` was installd successfully, run:
    ```
    $HOME/opt/cross/bin/$TARGET-gcc --version
    ```

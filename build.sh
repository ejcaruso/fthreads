#!/bin/sh
if [ ! -f bin ]
then
  mkdir bin
fi
gcc -m32 -g -o bin/fthreads src/*.c src/*.s

#!/bin/sh

gcc $1.c -o $1.exe $(pkg-config --cflags --libs sdl2) -lm

./$1.exe

rm $1.exe
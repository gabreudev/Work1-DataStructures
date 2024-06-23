#!/bin/sh

set -xe

export PKG_CONFIG_PATH=$HOME/opt/raylib/lib/pkgconfig/

gcc `pkg-config --cflags raylib` -o main image.c main.c `pkg-config --libs raylib` -lm -lglfw -lXcursor -ldl -lrt  -lX11 -lpthread  -I/usr/include/python3.10 -lpython3.10 -g 

# gdb ./main
./main
 
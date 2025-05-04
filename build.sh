#!/bin/sh

set -xe

export PKG_CONFIG_PATH=$HOME/opt/raylib/lib/pkgconfig/

gcc `pkg-config --cflags raylib` -Wall -Wextra -o main main.c image.c  utils/image_utils.c -I/home/alef/programas/Work1-DataStructures/utils `pkg-config --libs raylib` -lm -lglfw -lXcursor -ldl -lrt  -lX11 -lpthread  -I/usr/include/python3.10 -lpython3.10 -g 


# gdb ./main
./main

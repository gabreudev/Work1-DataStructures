#include <stdio.h>
#include <stdlib.h>
#include "image.h"
struct Dimensoes {
    int altura, largura;
};

struct PixelRGB {
    int red, blue, green;
};

struct PixelGray {
    int value;
};

struct ImageGray {
    Dimensoes dim;
    PixelGray *pixels;
};

struct ImageRGB {
    Dimensoes dim;
    PixelRGB *pixels;
};

// Funções de criação e liberação
ImageGray *create_image_gray(int largura, int altura);
void free_image_gray(ImageGray *image);

ImageRGB *create_image_rgb(int largura, int altura);
void free_image_rgb(ImageRGB *image);

// Operações para ImageGray
ImageGray *flip_vertical_gray(ImageGray *image);
ImageGray *flip_horizontal_gray(ImageGray *image);
ImageGray *transpose_gray(const ImageGray *image);

// Operações para ImageRGB
ImageRGB *flip_vertical_rgb(const ImageRGB *image);
ImageRGB *flip_horizontal_rgb(const ImageRGB *image);
ImageRGB *transpose_rgb(const ImageRGB *image);

// Manipulação por pixel para ImageGray
ImageGray *clahe_gray(const ImageGray *image, int tile_width, int tile_height);
ImageGray *median_blur_gray(const ImageGray *image, int kernel_size);

// Manipulação por pixel para ImageRGB
ImageRGB *clahe_rgb(const ImageRGB *image, int tile_width, int tile_height);
ImageRGB *median_blur_rgb(const ImageRGB *image, int kernel_size);



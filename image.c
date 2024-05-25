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

ImageGray *alocar_image_gray(int largura, int altura)
{
    ImageGray *image = (ImageGray *)malloc(sizeof(ImageGray));
    image->dim.largura = largura;
    image->dim.altura = altura;

    return image;
}

PixelGray *alocar_pixel_gray(int largura, int altura)
{
    PixelGray *vetor = (PixelGray *)malloc(largura * altura * sizeof(PixelGray));

    return vetor;
}

ImageRGB *alocar_image_RGB(int largura, int altura)
{
    ImageRGB *image = (ImageRGB *)malloc(sizeof(ImageRGB));
    image->dim.largura = largura;
    image->dim.altura = altura;

    return image;
}

PixelRGB *alocar_pixel_RGB(int largura, int altura)
{
    PixelRGB *vetor = (PixelRGB *)malloc(largura * altura * sizeof(PixelRGB));

    return vetor;
}

// Funções de criação e liberação
ImageGray *create_image_gray(int largura, int altura) 
{
    ImageGray *image = alocar_image_gray(largura, altura);

    image->pixels = alocar_pixel_gray(largura, altura);
    return image;
}

ImageRGB *create_image_rgb(int largura, int altura)
{
    ImageRGB *image = alocar_image_RGB(largura, altura);

    image->pixels = alocar_pixel_RGB(largura, altura);
    return image;
}

void free_image_gray(ImageGray *image) 
{
    free(image->pixels);
    free(image);
}

void free_image_RGB(ImageRGB *image) 
{
    free(image->pixels);
    free(image);
}
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



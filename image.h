#ifndef IMAGE_H
#define IMAGE_H

typedef struct dimensoes {
    int altura, largura;
} Dimensoes;

typedef struct pixelRGB {
    int red, blue, green;
} PixelRGB;

typedef struct pixelGray {
    int value;
} PixelGray;

typedef struct imageGray {
    Dimensoes dim;
    PixelGray *pixels;
} ImageGray;

typedef struct imageRGB {
    Dimensoes dim;
    PixelRGB *pixels;
} ImageRGB;

// Funções de criação e liberação
ImageGray *create_image_gray(int largura, int altura);
void free_image_gray(ImageGray *image);

ImageRGB *create_image_rgb(int largura, int altura);
void free_image_rgb(ImageRGB *image);

// Operações para ImageGray
void flip_vertical_gray(ImageGray *image);
void flip_horizontal_gray(ImageGray *image);
ImageGray *rotate_90_clockwise_gray(const ImageGray *image);
ImageGray *rotate_90_counterclockwise_gray(const ImageGray *image);

// Operações para ImageRGB
void flip_vertical_rgb(ImageRGB *image);
void flip_horizontal_rgb(ImageRGB *image);
ImageRGB *rotate_90_clockwise_rgb(const ImageRGB *image);
ImageRGB *rotate_90_counterclockwise_rgb(const ImageRGB *image);

// Manipulação por pixel para ImageGray
ImageGray *clahe_gray(const ImageGray *image, int tile_width, int tile_height);
ImageGray *median_blur_gray(const ImageGray *image, int kernel_size);

// Manipulação por pixel para ImageRGB
ImageRGB *clahe_rgb(const ImageRGB *image, int tile_width, int tile_height);
ImageRGB *median_blur_rgb(const ImageRGB *image, int kernel_size);

#endif // IMAGE_H
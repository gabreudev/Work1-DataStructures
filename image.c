#include<stdio.h>
#include<stdlib.h>
// #include<stdbool.h>
#include<assert.h>
#include<errno.h>
#include<math.h>
#include"image.h"

//////////////////////////////////////////
////////////// DEFINES //////////////////
#define NIVEL_INTENSIDADE 256

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

// "Converte" posição de matriz para posição de vetor
int posicaoVetor(int largura, int i, int j) { return largura * i + j; }

ImageGray *alocar_image_gray(int largura, int altura)
{
    ImageGray *image = (ImageGray *)malloc(sizeof(ImageGray));

    if(!image) 
    {
        fprintf(stderr, "Erro ao alocar memória para imagem\n");
        exit(EXIT_FAILURE);
    }

    image->dim.largura = largura;
    image->dim.altura = altura;

    return image;
}

PixelGray *alocar_pixel_gray(int largura, int altura)
{
    PixelGray *pixel = (PixelGray *)malloc(largura * altura * sizeof(PixelGray));

    if(!pixel) 
    {
        fprintf(stderr, "Erro ao alocar memória para pixel\n");
        exit(EXIT_FAILURE);
    }

    return pixel;
}

ImageRGB *alocar_image_RGB(int largura, int altura)
{
    ImageRGB *image = (ImageRGB *)malloc(sizeof(ImageRGB));

    if(!image) 
    {
        fprintf(stderr, "Erro ao alocar memória para imagem\n");
        exit(EXIT_FAILURE);
    }

    image->dim.largura = largura;
    image->dim.altura = altura;

    return image;
}

PixelRGB *alocar_pixel_RGB(int largura, int altura)
{
    PixelRGB *pixel = (PixelRGB *)malloc(largura * altura * sizeof(PixelRGB));

    if(!pixel) 
    {
        fprintf(stderr, "Erro ao alocar memória para pixel\n");
        exit(EXIT_FAILURE);
    }

    return pixel;
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

void free_pixel_gray(PixelGray *pixel) 
{
    free(pixel);
    pixel = NULL;
}

void free_image_RGB(ImageRGB *image) 
{
    free(image->pixels);
    free(image);
}

void free_pixel_RGB(PixelRGB *pixel) 
{
    free(pixel);
    pixel = NULL;
}
///////////////////////////////////////////////////////////////
////////////////// Operações para Arquivo//////////////////////
FILE *open(char *name, char *operation)
{
    FILE *file = fopen(name, operation);

    if(!file)
    {
        fprintf(stderr, "Erro ao abrir o arquivo: %s\n", name);
        exit(EXIT_FAILURE);
    }

    return file;
}
///////////////////////////////////////////////////////////////
//////////////////// Operações para ERRO //////////////////////
void check_allocation(void *pointer, const char *mensage) 
{
    if (!pointer) 
    {
        fprintf(stderr, "Erro ao alocar memória para %s: %d - %s\n", mensage, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}


///////////////////////////////////////////////////////////////
//////////////// Operações para ImageGray//////////////////////
ImageGray *flip_vertical_gray(ImageGray *image);
ImageGray *flip_horizontal_gray(ImageGray *image);
ImageGray *transpose_gray(const ImageGray *image)
{

    ImageGray *newImage = alocar_image_gray(image->dim.altura, image->dim.largura);
    newImage->dim.altura = image->dim.altura;
    newImage->dim.largura = image->dim.largura;

    for (int i = 0; i < image->dim.altura; i++)
    {
        for (int j = 0; j < image->dim.largura; j++)
        {
            newImage->pixels[posicaoVetor(image->dim.altura, image->dim.largura, image->dim.altura)] = image->pixels[posicaoVetor(image->dim.largura, image->dim.altura, image->dim.largura)];  
        }

    }

    return newImage;
}

//////////////////////////////////////////////////////////////////
/////////////// Operações para ImageRGB //////////////////////////
ImageRGB *flip_vertical_rgb(const ImageRGB *image)
{
    ImageRGB *newImage = alocar_image_RGB(image->dim.altura, image->dim.largura);
    newImage->dim.altura = image->dim.altura;
    newImage->dim.largura = image->dim.largura;
    for (int i = 0; i < image->dim.altura; i++)
    {
        for (int j = 0; j < image->dim.largura; j++)
        {
            newImage->pixels[posicaoVetor(image->dim.largura, image->dim.altura, image->dim.largura)] = image->pixels[posicaoVetor(image->dim.largura, image->dim.altura, image->dim.largura-1-j)];
        }
    }

    return newImage;
}
ImageRGB *flip_horizontal_rgb(const ImageRGB *image)
{
    ImageRGB *newImage = alocar_image_gray(image->dim.altura, image->dim.largura);
    newImage->dim.altura = image->dim.altura;
    newImage->dim.largura = image->dim.largura;
    for (int i = 0; i < image->dim.altura; i++)
    {
        for (int j = 0; j < image->dim.largura; j++)
        {
            newImage->pixels[posicaoVetor(image->dim.largura, image->dim.altura, image->dim.largura)] = image->pixels[posicaoVetor(image->dim.largura, image->dim.altura-1-i, image->dim.largura)];
        }
    }
    return newImage;
}

ImageRGB *transpose_rgb(const ImageRGB *image)
{
    ImageRGB *newImage = alocar_image_RGB(image->dim.altura, image->dim.largura);
    newImage->dim.altura = image->dim.altura;
    newImage->dim.largura = image->dim.largura;

    for (int i = 0; i < image->dim.altura; i++)
    {
        for (int j = 0; j < image->dim.largura; j++)
        {
            newImage->pixels[posicaoVetor(image->dim.altura, image->dim.largura, image->dim.altura)] = image->pixels[posicaoVetor(image->dim.largura, image->dim.altura, image->dim.largura)];
        }
    }

    return newImage;
}
//////////////////////////////////////////////////////////////////
/////////// Manipulação por pixel para ImageGray//////////////////

////////// AUXILIARES //////////////////////

/// Procura o Valor minimo do CDF maior que zero
int min_cdf(int *CDF) 
{
    for (int i = 0; i < NIVEL_INTENSIDADE; i++) 
        if (CDF[i] != 0) 
            return CDF[i];

    return 0; 
}

// Calcula a soma cumulativa dos valores do histograma
int *cumulative_histogram(int *histogram) 
{
    int *CDF = (int *)calloc(NIVEL_INTENSIDADE, sizeof(int));
    check_allocation(CDF, "CDF");

    CDF[0] = histogram[0];

    for (int i = 1; i < NIVEL_INTENSIDADE; i++) 
        CDF[i] = CDF[i - 1] + histogram[i];

    return CDF;
}

/////////////////////////////////////////////////

// Calcula cada tile individual do histograma 
void histogram_tile_gray(const ImageGray *image, int *histogram, int x1, int x2, int tile_width, int tile_height) 
{
    for (int i = x1; i < x1 + tile_width && i < image->dim.largura; i++) 
    {
        for (int j = x2; j < x2 + tile_height && j < image->dim.altura; j++) 
        {
            int index = posicaoVetor(image->dim.largura, j, i);
            histogram[image->pixels[index].value]++;
        }
    }
}

// Recebe o CDF e calcula o novo valor do pixel para a imagem
void histogram_equalizer_gray(ImageGray *image, int *CDF, int tile_width, int tile_height, int tile_increasew, int tile_increaseh)
{
    int cdf_min = min_cdf(CDF);

    for (int x = 0; x < tile_height; x++) 
    {
        for (int y = 0; y < tile_width; y++) 
        {
            int index = posicaoVetor(image->dim.largura, x + tile_increaseh, y + tile_increasew);
            int pixel_value = image->pixels[index].value;
            int new_value = round(((float)(CDF[pixel_value] - cdf_min) / (tile_width * tile_height - cdf_min)) * (NIVEL_INTENSIDADE - 1));
            image->pixels[index].value = new_value;
        }
    }
}

// Aplica o clahe, retornando a imagem equalizada 
ImageGray *clahe_gray(const ImageGray *image, int tile_width, int tile_height) 
{
    ImageGray *equalized_image = create_image_gray(image->dim.largura, image->dim.altura);

    for (int x1 = 0; x1 < image->dim.largura; x1 += tile_width) 
    {
        for (int x2 = 0; x2 < image->dim.altura; x2 += tile_height) 
        {
            int histogram[NIVEL_INTENSIDADE] = {0};

            histogram_tile_gray(image, histogram, x1, x2, tile_width, tile_height);
            int *CDF = cumulative_histogram(histogram);
            histogram_equalizer_gray(equalized_image, CDF, tile_width, tile_height, x1, x2);

            free(CDF);
        }
    }

    return equalized_image;
}

// Calcula a soma dos tiles ao redor do central
int soma_kernel_gray(const ImageGray *image, int index_i, int index_j, int kernel)
{
    int lin = index_i, col = index_j, soma = 0;

    for(int i = 0; i < kernel; i++)
    {
        for(int j = 0; j < kernel; j++)
        {
            int index = posicaoVetor(image->dim.largura, lin, col);

            soma += image->pixels[index].value;

            if(i % 2 == 1) 
                (j < i) ? col++ : lin++;
            else
                (j < i) ? col-- : lin--;
        }
    }

    return soma;
}

ImageGray *median_blur_gray(const ImageGray *image, int kernel_size)
{
    int limite = kernel_size / 2;
    int divisor = kernel_size * kernel_size;
    int soma = 0;

    ImageGray *image_blur = create_image_gray(image->dim.largura, image->dim.altura);
    
    for (int i = limite; i < image->dim.largura - limite; i++)
    {
        for(int j = limite; j < image->dim.altura - limite; j++)
        {
            int soma = soma_kernel_gray(image, i, j, kernel_size);
            int new_pixel = soma / divisor;
            
            image_blur->pixels[posicaoVetor(image_blur->dim.largura, i, j)].value = new_pixel;
        }
    }

    for (int i = 0; i < image->dim.largura; i++)
    {
        for(int j = 0; j < image->dim.altura; j++)
        {
            if (i < limite || i >= image->dim.altura - limite || j < limite || j >= image->dim.largura - limite) 
            {
                int index = posicaoVetor(image->dim.largura, i, j);
                image_blur->pixels[index].value = image->pixels[index].value;
            }
        }
    }
    
    return image_blur;
}

/////////////////////////////////////////////////////////////
///////// Manipulação por pixel para ImageRGB///////////////

// RGB: Calcula a soma cumulativa dos valores do histograma
PixelRGB *cumulative_histogram_rgb(PixelRGB *histogram) 
{
    PixelRGB *CDF = (PixelRGB *)calloc(256, sizeof(PixelRGB));
    check_allocation(CDF, "CDF");

    CDF[0].red = histogram[0].red;
    CDF[0].green = histogram[0].green;
    CDF[0].blue = histogram[0].blue;

    for (int i = 1; i < 256; i++) 
    {
        CDF[i].red = CDF[i - 1].red + histogram[i].red;
        CDF[i].green = CDF[i - 1].green + histogram[i].green;
        CDF[i].blue = CDF[i - 1].blue + histogram[i].blue;
    }

    return CDF;
}

// RGB: Calcula cada tile individual do histograma 
void histogram_tile_rgb(const ImageRGB *image, PixelRGB *histogram, int x1, int x2, int tile_width, int tile_height) 
{
    for (int i = x1; i < x1 + tile_width && i < image->dim.largura; i++) 
    {
        for (int j = x2; j < x2 + tile_height && j < image->dim.altura; j++) 
        {
            int index = posicaoVetor(image->dim.largura, j, i);
            histogram[image->pixels[index].red].red++;
            histogram[image->pixels[index].green].green++;
            histogram[image->pixels[index].blue].blue++;
        }
    }
}

// RGB: Recebe o CDF e calcula o novo valor do pixel para a imagem
void equalize_tile_rgb(ImageRGB *image, PixelRGB *CDF, int tile_width, int tile_height, int tile_increasew, int tile_increaseh) 
{
    int cdf_min_r = min_cdf(&CDF[0].red);
    int cdf_min_g = min_cdf(&CDF[0].green);
    int cdf_min_b = min_cdf(&CDF[0].blue);

    for (int x = 0; x < tile_height; x++) 
    {
        for (int y = 0; y < tile_width; y++) 
        {
            int index = posicaoVetor(image->dim.largura, x + tile_increaseh, y + tile_increasew);

            int pixel_value_r = image->pixels[index].red;
            int pixel_value_g = image->pixels[index].green;
            int pixel_value_b = image->pixels[index].blue;

            image->pixels[index].red = round(((float)(CDF[pixel_value_r].red - cdf_min_r) / (tile_width * tile_height - cdf_min_r)) * (NIVEL_INTENSIDADE - 1));
            image->pixels[index].green = round(((float)(CDF[pixel_value_g].green - cdf_min_g) / (tile_width * tile_height - cdf_min_g)) * (NIVEL_INTENSIDADE - 1));
            image->pixels[index].blue = round(((float)(CDF[pixel_value_b].blue - cdf_min_b) / (tile_width * tile_height - cdf_min_b)) * (NIVEL_INTENSIDADE - 1));
        }
    }
}

// RGB: Aplica o clahe, retornando a imagem equalizada 
ImageRGB *clahe_rgb(const ImageRGB *image, int tile_width, int tile_height)
{
    ImageRGB *equalized_image = create_image_rgb(image->dim.largura, image->dim.altura);

    for (int x1 = 0; x1 < image->dim.largura; x1 += tile_width) 
    {
        for (int x2 = 0; x2 < image->dim.altura; x2 += tile_height) 
        {
            PixelRGB histogram[256] = {0};

            histogram_tile_rgb(image, histogram, x1, x2, tile_width, tile_height);
            PixelRGB *CDF = cumulative_histogram(histogram);
            equalize_tile_rgb(equalized_image, CDF, tile_width, tile_height, x1, x2);

            free(CDF);
        }
    }

    return equalized_image;
}

// Calcula a soma dos tiles ao redor do central
PixelRGB *soma_kernel_RGB(const ImageRGB *image, int index_i, int index_j, int kernel)
{
    int lin = index_i, col = index_j;

    PixelRGB *soma_pixel = alocar_pixel_RGB(1, 1);

    for(int i = 0; i < kernel; i++)
    {
        for(int j = 0; j < kernel; j++)
        {
            int index = posicaoVetor(image->dim.largura, lin, col);

            soma_pixel->blue += image->pixels[index].blue;
            soma_pixel->red += image->pixels[index].red;
            soma_pixel->green += image->pixels[index].green;

            if(i % 2 == 1) 
                (j < i) ? col++ : lin++;
            else
                (j < i) ? col-- : lin--;
        }
    }

    return soma_pixel;
}

// RGB: substitui cada pixel pela média dos pixels em sua vizinhança
ImageRGB *median_blur_RGB(const ImageRGB *image, int kernel_size)
{
    int limite = kernel_size / 2;
    int divisor = kernel_size * kernel_size;
    int soma = 0;

    ImageRGB *image_blur = create_image_gray(image->dim.largura, image->dim.altura);
    
    for (int i = limite; i < image->dim.largura - limite; i++)
    {
        for(int j = limite; j < image->dim.altura - limite; j++)
        {
            PixelRGB *pixel = soma_kernel_RGB(image, i, j, kernel_size);
            int index = posicaoVetor(image->dim.largura, i, j);
            
            image_blur->pixels[index].blue = pixel->blue / divisor;
            image_blur->pixels[index].green = pixel->green / divisor;
            image_blur->pixels[index].red = pixel->red / divisor;
        }
    }

    // pega os valores das bordas 
    for (int i = 0; i < image->dim.largura; i++)
    {
        for(int j = 0; j < image->dim.altura; j++)
        {
            if (i < limite || i >= image->dim.altura - limite || j < limite || j >= image->dim.largura - limite) 
            {
                int index = posicaoVetor(image->dim.largura, i, j);
                image_blur->pixels[index].green = image->pixels[index].green;
                image_blur->pixels[index].blue = image->pixels[index].blue;
                image_blur->pixels[index].red = image->pixels[index].red;
            }
        }
    }
    
    return image_blur;
}





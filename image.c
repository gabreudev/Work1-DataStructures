#include<stdio.h>
#include<stdlib.h>
// #include<stdbool.h>
#include<assert.h>
#include<errno.h>
#include<math.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>
#include<Python.h>
#include<raylib.h>
#include"image.h"

///////////////////////////////////////////////////////////////
//////////////////// Operações para ERRO //////////////////////

// Checa a alocação de memoria 
void check_allocation(void *pointer, const char *mensage) 
{
    if(!pointer) 
    {
        fprintf(stderr, "Erro ao alocar memória para %s: %d - %s\n", mensage, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

// "Converte" posição de matriz para posição de vetor
int vector_position(int largura, int i, int j) {return largura * i + j;}
///////////////////////////////////////////////////////////////
//////////////////// Operações para alocação //////////////////

PixelGray *alocar_pixel_gray(int largura, int altura)
{
    PixelGray *pixel = (PixelGray *)malloc(largura * altura * sizeof(PixelGray));
    check_allocation(pixel, "pixelGray");

    return pixel;
}

ImageGray *alocar_image_gray(int largura, int altura)
{
    ImageGray *image = (ImageGray *)malloc(sizeof(ImageGray));
    image->pixels = alocar_pixel_gray(largura, altura);
    check_allocation(image, "imageGray");

    image->dim.largura = largura;
    image->dim.altura = altura;

    return image;
}

PixelRGB *alocar_pixel_RGB(int largura, int altura)
{
    PixelRGB *pixel = (PixelRGB *)malloc(largura * altura * sizeof(PixelRGB));
    check_allocation(pixel, "pixelRGB");

    return pixel;
}

ImageRGB *alocar_image_RGB(int largura, int altura)
{
    ImageRGB *image = (ImageRGB *)malloc(sizeof(ImageRGB));
    image->pixels = alocar_pixel_RGB(largura, altura);

    check_allocation(image, "imageRGB");

    image->dim.largura = largura;
    image->dim.altura = altura;

    return image;
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

History *allocate_history() 
{
    History *history = (History *)malloc(sizeof(History));
    check_allocation(history, "History");

    history->left = NULL;
    history->right = NULL;

    return history;
}

void free_history(History *history) 
{
    if (history) 
    {
        if (history->type == GRAY_ && history->gray_image) 
            free_image_gray(history->gray_image);
        else if (history->type == RGB_ && history->rgb_image) 
            free_image_RGB(history->rgb_image);
        
        free(history);
    }
}


///////////////////////////////////////////////////////////////
////////////////// Operações para Historico//////////////////////


// função de desfazer operações, removendo a imagem atual e retornando à anterior 
// mode (1 - apaga a imagem, 0 - deixa a imagem no historico).
History *back_image(History *history, int mode)
{
    History *aux = history, *prox = history->right, *ret;

    if(mode == 1)
    {
        while(aux)
        {
            if(!prox)
            {
                ret = aux->left;
                ret->right = NULL;
                free_history(aux);
                
                return ret;
            }
            aux = prox;
            prox = prox->right;
        }
    }
    else
    {
        if(aux->left)
            aux = aux->left;

        return aux;
    }

    return aux;
}



History *add_image(History *history, void *image) 
{
    // Create a new history node
    History *new_node = allocate_history();
    new_node->type = history->type;

    if (history->type == GRAY_) 
        new_node->gray_image = (ImageGray *)image;
    else if (history->type == RGB_) 
        new_node->rgb_image = (ImageRGB *)image;
    
    // Attach the new node to the end of the history list
    if(history) 
    {
        while (history->right) 
            history = history->right;
        
        history->right = new_node;
        new_node->left = history;
    }

    return new_node;
}

 // função de refazer operações, avançando para a próxima imagem, se possível.
History *next_image(History *history)
{
    if(!history || !history->right)
        return history;

    return history->right;
}

// função de navegação pelo histórico, permitindo ir para versões específicas da imagem.
History *browse_history(History *history, int version)
{
    if(version < 1 || !history)
        return history;
    
    History *aux = history;
    int cont = 0;

    while(cont < version || !aux)
    {
        aux = aux->right;
        cont += 1;
    }
    
    if(cont != version)
    {
        printf("Versão não encontrada!");
        return history;
    }

    return aux;
}


///////////////////////////////////////////////////////////////
////////////////// Operações para Arquivo//////////////////////

// Abrir arquivo nome do arquivo e tipo de operação(ler/escrever)
// FILE *open_file(char *name, char *operation)
// {
//     FILE *file = fopen(name, operation);

//     if(!file)
//     {
//         fprintf(stderr, "Erro ao abrir o arquivo: %s\n", name);
//         exit(EXIT_FAILURE);
//     }

//     return file;
// }

// Ler txt e converter em imagem -> ImageRGB
ImageRGB *read_rgb_image(FILE *arquivo)
{
    ImageRGB temp;

    fscanf(arquivo, "%d", &temp.dim.altura);
    fgetc(arquivo);
    fscanf(arquivo, "%d", &temp.dim.largura);
    fgetc(arquivo);
    
    ImageRGB *image = create_image_rgb(temp.dim.altura, temp.dim.largura);

    for (int i = 0, cont = 0; i < image->dim.altura * image->dim.largura; i++, cont++)
    {
        if(cont == image->dim.largura)
        {
            fgetc(arquivo);
            cont = 0;
        }

        fscanf(arquivo, "%d %d %d,", &image->pixels[i].red, &image->pixels[i].green, &image->pixels[i].blue);
    }

    return image;    
}

// Ler txt e converter em imagem -> Imagegray
ImageGray *read_gray_image(FILE *arquivo)
{
    ImageGray temp;
    
    fscanf(arquivo, "%d", &temp.dim.altura);
    fgetc(arquivo);
    fscanf(arquivo, "%d", &temp.dim.largura);
    fgetc(arquivo);
    
    ImageGray *image = create_image_gray(temp.dim.altura, temp.dim.largura);
    
    for (int i = 0, cont = 0; i < image->dim.altura * image->dim.largura; i++, cont++)
    {
        if(cont == image->dim.largura)
        {
            fgetc(arquivo);
            cont = 0;
        }

        fscanf(arquivo, "%d", &image->pixels[i].value);
        fgetc(arquivo);
    }
    
    return image;    
}

// Exportar imagem RGB para txt -> txt salvo
void save_image_rgb(ImageRGB *image, FILE *arquivo)
{
    int tam = image->dim.altura * image->dim.largura;

    fprintf(arquivo, "%d", image->dim.altura);
    fprintf(arquivo, "\n");
    fprintf(arquivo, "%d", image->dim.largura);
    fprintf(arquivo, "\n");

    for (int i = 0, cont = 0; i < tam; i++, cont++)
    {
        if(cont == image->dim.largura)
        {
            fprintf(arquivo, "\n");
            cont = 0;
        }

        fprintf(arquivo, "%d %d %d,", image->pixels[i].red, image->pixels[i].green, image->pixels[i].blue);
    }
}


// Exportar imagem nível cinza para txt -> txt salvo
void save_image_gray(ImageGray *image, FILE *arquivo)
{
    int tam = image->dim.altura * image->dim.largura;

    fprintf(arquivo, "%d", image->dim.altura);
    fprintf(arquivo, "\n");
    fprintf(arquivo, "%d", image->dim.largura);
    fprintf(arquivo, "\n");

    for (int i = 0, cont = 0; i < tam; i++, cont++)
    {
        if(cont == image->dim.largura)
        {
            fprintf(arquivo, "\n");
            cont = 0;
        }

        fprintf(arquivo, "%d,", image->pixels[i].value);
    }
}

///////////////////////////////////////////////////////////////
//////////////// Operações para ImageGray//////////////////////
ImageGray *flip_vertical_gray(ImageGray *image)
{
    ImageGray *newImage = create_image_gray(image->dim.altura, image->dim.largura);

    newImage->dim.altura = image->dim.altura;
    newImage->dim.largura = image->dim.largura;

    for (int i = 0; i < image->dim.altura; i++)
    {
        for (int j = 0; j < image->dim.largura; j++)
        {
            newImage->pixels[vector_position(image->dim.largura, i, j)] = image->pixels[vector_position(image->dim.largura, image->dim.altura - 1 - i, j)];
        }
    }

    return newImage;
}


ImageGray *flip_horizontal_gray(ImageGray *image)
{
    ImageGray *newImage = create_image_gray(image->dim.altura, image->dim.largura);

    for (int i = 0; i < image->dim.altura; i++)
    {
        for (int j = 0; j < image->dim.largura; j++)
        {
            newImage->pixels[vector_position(newImage->dim.largura, i, j)] = image->pixels[vector_position(image->dim.largura, i, image->dim.largura-1-j)];
        }
    }
    return newImage;
}

ImageGray *transpose_gray(const ImageGray *image)
{
    ImageGray *newImage = create_image_gray(image->dim.largura, image->dim.altura);

    for (int i = 0; i < image->dim.altura; i++)
    {
        for (int j = 0; j < image->dim.largura; j++)
        {
            newImage->pixels[vector_position(newImage->dim.largura, j, i)] = image->pixels[vector_position(image->dim.largura, i, j)];
        }
    }

    return newImage;
}


//////////////////////////////////////////////////////////////////
/////////////// Operações para ImageRGB //////////////////////////
ImageRGB *flip_vertical_rgb(const ImageRGB *image)
{
    ImageRGB *newImage = create_image_rgb(image->dim.altura, image->dim.largura);

    for (int i = 0; i < image->dim.altura; i++)
    {
        for (int j = 0; j < image->dim.largura; j++)
        {
            newImage->pixels[vector_position(image->dim.largura, i, j)] = image->pixels[vector_position(image->dim.largura, image->dim.altura - 1 - i, j)];
        }
    }

    return newImage;
}

ImageRGB *flip_horizontal_rgb(const ImageRGB *image)
{
    ImageRGB *newImage = create_image_rgb(image->dim.altura, image->dim.largura);
 
    for (int i = 0; i < image->dim.altura; i++)
    {
        for (int j = 0; j < image->dim.largura; j++)
        {
            newImage->pixels[vector_position(newImage->dim.largura, i, j)] = image->pixels[vector_position(image->dim.largura, i, image->dim.largura-1-j)];
        }
    }

    return newImage;
}


ImageRGB *transpose_rgb(const ImageRGB *image)
{
    ImageRGB *newImage = create_image_rgb(image->dim.altura, image->dim.largura);

    for (int i = 0; i < image->dim.altura; i++)
    {
        for (int j = 0; j < image->dim.largura; j++)
        {
            newImage->pixels[vector_position(newImage->dim.largura, j, i)] = image->pixels[vector_position(image->dim.largura, i, j)];
        }
    }

    return newImage;
}
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
void histogram_tile_gray(const ImageGray *image, int *histogram, int x, int y, int tile_width, int tile_height) 
{
    for (int i = x; i < x + tile_width && i < image->dim.largura; i++) 
    {
        for (int j = y; j < y + tile_height && j < image->dim.altura; j++) 
        {
            int index = vector_position(image->dim.largura, j, i);
            histogram[image->pixels[index].value]++;
        }
    }
}

// Recebe o CDF e calcula o novo valor do pixel para a imagem
void histogram_equalizer_gray(const ImageGray *image,  ImageGray *equalize, int *CDF, int tile_width, int tile_height, int tile_increasew, int tile_increaseh)
{
    int cdf_min = min_cdf(CDF);

    for (int x = 0; x < tile_height; x++) 
    {
        for (int y = 0; y < tile_width; y++) 
        {
            int index = vector_position(image->dim.largura, x + tile_increaseh, y + tile_increasew);
            int pixel_value = image->pixels[index].value;
            int new_value = round(((float)(CDF[pixel_value] - cdf_min) / (tile_width * tile_height - cdf_min)) * (NIVEL_INTENSIDADE - 1));
            equalize->pixels[index].value = new_value;
        }
    }
}

// Aplica o clahe, retornando a imagem equalizada 
ImageGray *clahe_gray(const ImageGray *image, int tile_width, int tile_height) 
{
    ImageGray *equalized_image = create_image_gray(image->dim.largura, image->dim.altura);

    for (int x = 0; x < image->dim.largura; x += tile_width) 
    {
        for (int y = 0; y < image->dim.altura; y += tile_height) 
        {
            int histogram[NIVEL_INTENSIDADE] = {0};

            histogram_tile_gray(image, histogram, x, y, tile_width, tile_height);
            int *CDF = cumulative_histogram(histogram);
            histogram_equalizer_gray(image, equalized_image, CDF, tile_width, tile_height, x, y);

            free(CDF);
        }
    }

    return equalized_image;
}


// Calcula a soma dos tiles ao redor do central
int soma_kernel_gray(const ImageGray *image, int index_i, int index_j, int kernel)
{
    int lin = index_i, col = index_j, ind = 0;

    int sum = 0;

    for(int i = 0; i < kernel; i++)
    {
        for(int j = 0; j < kernel; j++)
        {
            int index = vector_position(image->dim.largura, lin, col);

            if(lin >= 0 && col >= 0)
            {
                sum += image->pixels[index].value;
                ind += 1;
            }

            if(i % 2 == 1) 
                (j < i) ? col++ : lin++;
            else
                (j < i) ? col-- : lin--;
        }
    }

    int result = sum / ind;

    return result;
}

ImageGray *median_blur_gray(const ImageGray *image, int kernel_size)
{
    ImageGray *image_blur = create_image_gray(image->dim.largura, image->dim.altura);
    
    for (int i = 0; i < image->dim.largura; i++)
    {
        for(int j = 0; j < image->dim.altura; j++)
        {
            int new_pixel = soma_kernel_gray(image, i, j, kernel_size);
            
            image_blur->pixels[vector_position(image_blur->dim.largura, i, j)].value = new_pixel;
        }
    }

    return image_blur;
}



/////////////////////////////////////////////////////////////
///////// Manipulação por pixel para ImageRGB///////////////

// RGB: Calcula a soma cumulativa dos valores do histograma
PixelRGB *cumulative_histogram_rgb(PixelRGB *histogram) 
{
    PixelRGB *CDF = (PixelRGB *)calloc(NIVEL_INTENSIDADE, sizeof(PixelRGB));
    check_allocation(CDF, "CDF");

    CDF[0].red = histogram[0].red;
    CDF[0].green = histogram[0].green;
    CDF[0].blue = histogram[0].blue;

    for (int i = 1; i < NIVEL_INTENSIDADE; i++) 
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
            int index = vector_position(image->dim.largura, j, i);
            histogram[image->pixels[index].red].red++;
            histogram[image->pixels[index].green].green++;
            histogram[image->pixels[index].blue].blue++;
        }
    }
}

// RGB: Recebe o CDF e calcula o novo valor do pixel para a imagem
void equalize_tile_rgb(const ImageRGB *image, ImageRGB *equalized, PixelRGB *CDF, int tile_width, int tile_height, int tile_increasew, int tile_increaseh) 
{
    int cdf_min_r = min_cdf(&CDF[0].red);
    int cdf_min_g = min_cdf(&CDF[0].green);
    int cdf_min_b = min_cdf(&CDF[0].blue);

    for (int x = 0; x < tile_height; x++) 
    {
        for (int y = 0; y < tile_width; y++) 
        {
            int index = vector_position(image->dim.largura, x + tile_increaseh, y + tile_increasew);

            int pixel_value_r = image->pixels[index].red;
            int pixel_value_g = image->pixels[index].green;
            int pixel_value_b = image->pixels[index].blue;

            equalized->pixels[index].red = round(((float)(CDF[pixel_value_r].red - cdf_min_r) / (tile_width * tile_height - cdf_min_r)) * (NIVEL_INTENSIDADE - 1));
            equalized->pixels[index].green = round(((float)(CDF[pixel_value_g].green - cdf_min_g) / (tile_width * tile_height - cdf_min_g)) * (NIVEL_INTENSIDADE - 1));
            equalized->pixels[index].blue = round(((float)(CDF[pixel_value_b].blue - cdf_min_b) / (tile_width * tile_height - cdf_min_b)) * (NIVEL_INTENSIDADE - 1));
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
            PixelRGB histogram[NIVEL_INTENSIDADE] = {0};

            histogram_tile_rgb(image, histogram, x1, x2, tile_width, tile_height);
            PixelRGB *CDF = cumulative_histogram_rgb(histogram);
            equalize_tile_rgb(image, equalized_image, CDF, tile_width, tile_height, x1, x2);

            free(CDF);
        }
    }

    return equalized_image;
}

int compare_pixel(const void *a, const void *b) 
{
    PixelRGB *pixelA = (PixelRGB *)a;
    PixelRGB *pixelB = (PixelRGB *)b;
    
    int avgA = (pixelA->red + pixelA->green + pixelA->blue) / 3;
    int avgB = (pixelB->red + pixelB->green + pixelB->blue) / 3;
    
    return avgA - avgB;
}


PixelRGB *sort_pixel(PixelRGB *pixel, int ind)
{
    qsort(pixel, ind, sizeof(PixelRGB), compare_pixel);

    PixelRGB *result = (PixelRGB *)malloc(1 * sizeof(PixelRGB));
    check_allocation(result, "sort_pixel");

    *result = pixel[ind / 2];

    return result;
}

// Calcula a soma dos tiles ao redor do central
PixelRGB *soma_kernel_RGB(const ImageRGB *image, int index_i, int index_j, int kernel)
{
    int lin = index_i, col = index_j, ind = 0;

    PixelRGB *median_pixel = alocar_pixel_RGB(kernel, kernel);
    check_allocation(median_pixel, "median_pixel");

    for(int i = 0; i < kernel; i++)
    {
        for(int j = 0; j < kernel; j++)
        {
            int index = vector_position(image->dim.largura, lin, col);

            if(lin >= 0 && col >= 0)
            {
                median_pixel[ind].blue  = image->pixels[index].blue;
                median_pixel[ind].red   = image->pixels[index].red;
                median_pixel[ind].green = image->pixels[index].green;

                ind += 1;
            }

            if(i % 2 == 1) 
                (j < i) ? col++ : lin++;
            else
                (j < i) ? col-- : lin--;
        }
    }

    PixelRGB *result = sort_pixel(median_pixel, ind);
    free_pixel_RGB(median_pixel);

    return result;
}

// RGB: substitui cada pixel pela média dos pixels em sua vizinhança
ImageRGB *median_blur_RGB(const ImageRGB *image, int kernel_size)
{
    ImageRGB *image_blur = create_image_rgb(image->dim.largura, image->dim.altura);
    
    for (int i = 0; i < image->dim.largura ; i++)
    {
        for(int j = 0; j < image->dim.altura ; j++)
        {
            PixelRGB *pixel = soma_kernel_RGB(image, i, j, kernel_size);
            int index = vector_position(image->dim.largura, i, j);

            image_blur->pixels[index].blue = pixel[0].blue;
            image_blur->pixels[index].green = pixel[0].green;
            image_blur->pixels[index].red = pixel[0].red;
        }

    }
    
    return image_blur;
}

// RandomList *random_efects(History *history, int width, int height){    
//     srand(time(NULL));
//     RandomList *randomList;
//     randomList->image = history->image;
//     RandomList *aux = randomList;
//     if (history->type==GRAY_)
//         {
            
//             for (int i = 0; i < 5; i++)
//             {
//                 int chosed = rand() % 5;

//                 switch (chosed)
//                 {
//                 case 0:
//                     aux->right->image = flip_horizontal_gray(aux->image);
//                     break;
//                 case 1:
//                     aux->right->image = flip_vertical_gray(aux->image);
//                     break;
//                 case 2:
//                     aux->right->image = transpose_gray(aux->image);
//                     break;
//                 case 3:
//                     aux->right->image = median_blur_gray(aux->image, 8);
//                     break;
//                 case 4:
//                     aux->right->image = clahe_gray(aux->image,width, height);
//                     break;

//                 aux=aux->right;
                
//                 }
//             }
//             return randomList;
//         }

            
//             for (int i = 0; i < 5; i++)
//             {
//                 int chosed = rand() % 5;

//                 switch (chosed)
//                 {
//                 case 0:
//                     aux->right->image = flip_horizontal_rgb(aux->image);
//                     break;
//                 case 1:
//                     aux->right->image = flip_vertical_rgb(aux->image);
//                     break;
//                 case 2:
//                     aux->right->image = transpose_rgb(aux->image);
//                     break;
//                 case 3:
//                     aux->right->image = median_blur_RGB(aux->image, 8);
//                     break;
//                 case 4:
//                     aux->right->image = clahe_rgb(aux->image,width, height);
//                     break;
                
//                 aux=aux->right;

//                 }
//                 return randomList;
//             }
                
//         }


// int main()
// {
//     FILE *path = fopen("/home/alef/Linguagens/faculdade/Work1-DataStructures/utils/input_image_example_RGB.txt", "r");
//     check_allocation(path, "path");

//     ImageRGB *image = read_rgb_image(path);
//     fclose(path);

//     FILE *load = fopen("load.txt", "w");

//     ImageRGB *blur = median_blur_RGB(image, 8);
//     save_image_rgb(blur, load);

//     fclose(load);
// }


int gray_or_rgb(int type)
{
    if(type == GRAY_)
        return 1;
    return 0;
}

// transforma uma imagem RGB ou GRAY em TXT
void txt_from_image(const char* image_path, const char* output_path, int type) 
{
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;
    

    printf("%s %s %d\n", image_path, output_path, type);

    // Inicializar o interpretador Python
    Py_Initialize();

    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\".\")");

    // Nome do módulo Python (arquivo .py sem a extensão)
    pName = PyUnicode_DecodeFSDefault("utils.image_utils");
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) 
    {
        // Nome da função a ser chamada

        pFunc = PyObject_GetAttrString(pModule, "txt_from_image_gray");
    
        int is_gray = gray_or_rgb(type);

        if (pFunc && PyCallable_Check(pFunc)) 
        {
            // Criar argumentos para a função Python
            pArgs = PyTuple_Pack(3, PyUnicode_FromString(image_path), PyUnicode_FromString(output_path), PyLong_FromLong(is_gray));

            // Chamar a função Python
            PyObject *pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);

            if (pValue != NULL) 
            {
                printf("Chamou txt_from_image_gray com sucesso\n");
                Py_DECREF(pValue);
            } 
            else 
            {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr, "Chamada da função txt_from_image_gray falhou\n");
                return;
            }
        } 
        else 
        {
            if (PyErr_Occurred())
                PyErr_Print();

            fprintf(stderr, "Não conseguiu encontrar a função txt_from_image_gray\n");
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    } 
    else 
    {
        PyErr_Print();
        fprintf(stderr, "Falha ao carregar o módulo image_processing\n");
        return;
    }

    // Finalizar o interpretador Python
    Py_Finalize();
}

// Transforma um TXT em uma imagem RGB ou GRAY
void image_from_txt(const char* txt_path, const char* output_path, int type) 
{
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;

    // Inicializar o interpretador Python
    Py_Initialize();

    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\".\")");

    // Nome do módulo Python (arquivo .py sem a extensão)
    pName = PyUnicode_DecodeFSDefault("utils.image_utils");
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) 
    {
        printf("%s %s %d\n", txt_path, output_path, type);
        // Nome da função a ser chamada
        if(type == RGB_)
            pFunc = PyObject_GetAttrString(pModule, "image_rgb_from_txt");
        else
            pFunc = PyObject_GetAttrString(pModule, "image_gray_from_txt");
        // pFunc = PyObject_GetAttrString(pModule, "execute");

        if (pFunc && PyCallable_Check(pFunc)) 
        {
            // Criar argumentos para a função Python
            pArgs = PyTuple_Pack(2, PyUnicode_FromString(txt_path), PyUnicode_FromString(output_path));
            // pArgs = PyTuple_Pack(0);

            // Chamar a função Python
            PyObject *pValue  = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);

            if (pValue != NULL) 
            {
                printf("Chamou image_gray_from_txt com sucesso\n");
                Py_DECREF(pValue);
            } 
            else
            {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr, "Chamada da função image_gray_from_txt falhou\n");
                return;
            }
        } 
        else 
        {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Não conseguiu encontrar a função image_gray_from_txt\n");
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    } else 
    {
        PyErr_Print();
        fprintf(stderr, "Falha ao carregar o módulo image_processing\n");
        return;
    }

    // Finalizar o interpretador Python
    Py_Finalize();
}

void adjust_image_format(Image *image) 
{
    if (PIXELFORMAT_UNCOMPRESSED_GRAYSCALE == image->format) 
        ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE); 
    else if (PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA == image->format)  
        ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA);    
    else if (PIXELFORMAT_UNCOMPRESSED_R5G6B5 == image->format) 
        ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_R5G6B5);        
    else if (PIXELFORMAT_UNCOMPRESSED_R8G8B8 == image->format)  
        ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_R8G8B8);       
    else if (PIXELFORMAT_UNCOMPRESSED_R5G5B5A1 == image->format) 
        ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_R5G5B5A1);       
    else if (PIXELFORMAT_UNCOMPRESSED_R4G4B4A4 == image->format)
        ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_R4G4B4A4);        
    else if (PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 == image->format)
        ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);       
    else if (PIXELFORMAT_UNCOMPRESSED_R32 == image->format) 
        ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_R32);            
    else if (PIXELFORMAT_UNCOMPRESSED_R32G32B32 == image->format) 
        ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_R32G32B32);      
    else if (PIXELFORMAT_UNCOMPRESSED_R32G32B32A32 == image->format)
        ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_R32G32B32A32);    
    else if (PIXELFORMAT_UNCOMPRESSED_R16 == image->format) 
        ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_R16);            
    else if (PIXELFORMAT_UNCOMPRESSED_R16G16B16 == image->format)   
        ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_R16G16B16);     
    else if (PIXELFORMAT_UNCOMPRESSED_R16G16B16A16 == image->format) 
        ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);    
    else if (PIXELFORMAT_COMPRESSED_DXT1_RGB == image->format)
        ImageFormat(image, PIXELFORMAT_COMPRESSED_DXT1_RGB);          
    else if (PIXELFORMAT_COMPRESSED_DXT1_RGBA == image->format)
        ImageFormat(image, PIXELFORMAT_COMPRESSED_DXT1_RGBA);          
    else if (PIXELFORMAT_COMPRESSED_DXT3_RGBA == image->format) 
        ImageFormat(image, PIXELFORMAT_COMPRESSED_DXT3_RGBA);       
    else if (PIXELFORMAT_COMPRESSED_DXT5_RGBA == image->format)
        ImageFormat(image, PIXELFORMAT_COMPRESSED_DXT5_RGBA);        
    else if (PIXELFORMAT_COMPRESSED_ETC1_RGB == image->format)  
        ImageFormat(image, PIXELFORMAT_COMPRESSED_ETC1_RGB);       
    else if (PIXELFORMAT_COMPRESSED_ETC2_RGB == image->format) 
        ImageFormat(image, PIXELFORMAT_COMPRESSED_ETC2_RGB);        
    else if (PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA == image->format)
        ImageFormat(image, PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA);    
    else if (PIXELFORMAT_COMPRESSED_PVRT_RGB == image->format)
        ImageFormat(image, PIXELFORMAT_COMPRESSED_PVRT_RGB);         
    else if (PIXELFORMAT_COMPRESSED_PVRT_RGBA == image->format)
        ImageFormat(image, PIXELFORMAT_COMPRESSED_PVRT_RGBA);        
    else if (PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA == image->format)  
        ImageFormat(image, PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA);  
    else if (PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA == image->format)
        ImageFormat(image, PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA); 
}

// carrega a nova imagem que será mostrada na tela 
void load_new_texture(Image *image, Texture2D *texture, History *history, char *file_path, int mode)
{
    /// MANDAR PARA O CODIGO PYTHON 
    if(mode == 1)
    {
        txt_from_image(file_path, TXT_PATH, history->type);
        
        FILE *load_txt = fopen(TXT_PATH, "r");

        if(history->type == RGB_)  
            history->rgb_image = read_rgb_image(load_txt);
        else
            history->gray_image = read_gray_image(load_txt);
        
        fclose(load_txt);
    }

    // Load new image and update texture
    Image new_image = LoadImage(file_path);
    *image = ImageCopy(new_image);

    // Adjust image format based on the type
    if (history->type == RGB_)
        ImageFormat(&new_image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    else
        ImageFormat(&new_image, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);

    *texture = LoadTextureFromImage(new_image);

    Color *pixels = LoadImageColors(new_image);

    UpdateTexture(*texture, pixels);
    UnloadImageColors(pixels);
    UnloadImage(new_image);
}




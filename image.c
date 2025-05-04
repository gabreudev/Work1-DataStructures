#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<errno.h>
#include<math.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>
#include<raylib.h>

// o arquivo image_utils está no diretorio utils no mesmo nivel do arquivo image.c, logo
#include "utils/image_utils.h"
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

void free_image_rgb(ImageRGB *image) 
{
    free(image->pixels);
    free(image);
}

void free_pixel_rgb(PixelRGB *pixel) 
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
    while(history->left) history = history->left;
    
    while (history != NULL)
    {
        History *next = history->right;
        
        if (history->type == GRAY_ && history->gray_image)
            free_image_gray(history->gray_image);
        else if (history->type == RGB_ && history->rgb_image)
            free_image_rgb(history->rgb_image);
        
        free(history);
        history = next;
    }
}


RandomList *alloc_random()
{
    RandomList *l = (RandomList *)malloc(sizeof(RandomList));
    check_allocation(l, "Random list");

    l->image_gray = NULL;
    l->image_rgb = NULL;
    l->right = NULL;

    return l;
}

void free_random(RandomList *list)
{
    while (list != NULL)
    {
        RandomList *next = list->right;
        
        (list->type == RGB_) ? 
            free_image_rgb(list->image_rgb) : free_image_gray(list->image_gray);
        
        free(list);
        list = next;
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
                // free_history(aux);
                if(aux->type == RGB_)
                    free_image_rgb(aux->rgb_image);
                else
                    free_image_gray(aux->gray_image);
                
                free(aux);
                
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
// dunção para avancar para a proxima imagem da lista randômica
RandomList *next_random_image(RandomList *history)
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

void initialize_history(History *history, ImageType type)
{
    FILE *initial_image;
    if(type == RGB_)
    {
        initial_image = fopen("utils/rgb.txt", "r");
        check_allocation(initial_image, "initial_image");

        history->type = RGB_;
        history->rgb_image = read_rgb_image(initial_image);
    }
    else
    {
        initial_image = fopen("utils/gray.txt", "r");
        check_allocation(initial_image, "initial_image");

        history->gray_image = read_gray_image(initial_image);
        history->type = GRAY_;
    }
    fclose(initial_image);
}

void initialize_random_effects(RandomList *rl, ImageType type) 
{
    FILE *initial_image;
    if(type == RGB_) 
    {
        initial_image = fopen("utils/rgb.txt", "r");
        check_allocation(initial_image, "initial_image rgb");

        rl->type = RGB_;
        rl->image_rgb = read_rgb_image(initial_image);
        rl->image_gray = NULL;
    } 
    else 
    {
        initial_image = fopen("utils/gray.txt", "r");
        check_allocation(initial_image, "initial_image gray");

        rl->type = GRAY_;
        rl->image_gray = read_gray_image(initial_image);
        rl->image_rgb = NULL;
    }
    fclose(initial_image);
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

// Simplified kernel calculation for RGB blur
PixelRGB calculate_average_kernel(const ImageRGB *image, int x, int y) 
{
    PixelRGB avg = {0, 0, 0};
    int count = 0;
    
    // Use a fixed 3x3 neighborhood
    for(int i = -1; i <= 7; i++) 
    {
        for(int j = -1; j <= 7; j++) 
        {
            int new_x = x + i;
            int new_y = y + j;
            
            // Check bounds
            if(new_x >= 0 && new_x < image->dim.largura && new_y >= 0 && new_y < image->dim.altura) 
            {
                int index = vector_position(image->dim.largura, new_x, new_y);

                avg.red += image->pixels[index].red;
                avg.green += image->pixels[index].green;
                avg.blue += image->pixels[index].blue;
                count++;
            }
        }
    }
    
    // Calculate average
    avg.red /= count;
    avg.green /= count;
    avg.blue /= count;
    
    return avg;
}

// Simplified blur function
ImageRGB *median_blur_RGB(const ImageRGB *image, int kernel_size) 
{
    ImageRGB *image_blur = create_image_rgb(image->dim.largura, image->dim.altura);
    
    for(int i = 0; i < image->dim.largura; i++) {
        for(int j = 0; j < image->dim.altura; j++) {
            PixelRGB avg = calculate_average_kernel(image, i, j);
            int index = vector_position(image->dim.largura, i, j);
            image_blur->pixels[index] = avg;
        }
    }
    
    return image_blur;
}

int gray_or_rgb(int type)
{
    if(type == GRAY_)
        return 1;
    return 0;
}


void adjust_image_size(Image *image)
{
    int screenWidth = GetScreenWidth() * 0.90;
    int screenHeight = GetScreenHeight() * 0.90;

    if ((*image).width > screenWidth || (*image).height > screenHeight) 
    {
        float aspectRatio = (float)(*image).width / (float)(*image).height;
        int newWidth = (*image).width;
        int newHeight = (*image).height;

        if ((*image).width > screenWidth) 
        {
            newWidth = screenWidth;
            newHeight = newWidth / aspectRatio;
        }
        if (newHeight > screenHeight) 
        {
            newHeight = screenHeight;
            newWidth = newHeight * aspectRatio;
        }

        ImageResize(&(*image), newWidth, newHeight);
    }
}

// carrega a nova imagem que será mostrada na tela 
void load_new_texture(Texture2D *texture, History *history, char *file_path, int mode)
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
    // Resize image if larger than screen dimensions
    int screenWidth = GetScreenWidth() * 0.90;
    int screenHeight = GetScreenHeight() * 0.90;

    // Load new image and update texture
    // Image new_image;
    Image new_image = LoadImage(file_path);

    // Color color[texture->height * texture->width];
    // if(history->type == RGB_)
    // {
    //     for(int i = 0; i < texture->height * texture->width; i++)
    //     {
    //         color[i].a = 255;
    //         color[i].r = history->rgb_image->pixels->red;
    //         color[i].b = history->rgb_image->pixels->blue;
    //         color[i].g = history->rgb_image->pixels->green;
    //     }
    // }
    // else
    // {
    //     for(int i = 0; i < texture->height * texture->width; i++)
    //     {
    //         color[i].a = 255;
    //         color[i].r = history->gray_image->pixels->value;
    //         color[i].b = history->gray_image->pixels->value;
    //         color[i].g = history->gray_image->pixels->value;
    //     }
    // }

    // new_image.format = texture->format;
    // new_image.height = texture->height;
    // new_image.width = texture->width;
    // new_image.mipmaps = texture->mipmaps;
    // new_image.data = color;

    if(new_image.width > (GetScreenWidth() * 0.90) || new_image.height > (GetScreenHeight() * 0.90))
        adjust_image_size(&new_image);

    if(texture->id > 0) 
        UpdateTexture(*texture, new_image.data) ;
    else *texture = 
        LoadTextureFromImage(new_image);

    // UnloadImage(new_image);
}

// carrega a nova imagem que será mostrada na tela 
void load_new_texture_random(Texture2D *texture, RandomList *rl, char *file_path, int mode) 
{
    // Manda para o código Python se o modo for 1
    if (mode == 1) 
    {
        txt_from_image(file_path, TXT_PATH, rl->type);
        
        FILE *load_txt = fopen(TXT_PATH, "r");

        if (rl->type == RGB_)  
            rl->image_rgb = read_rgb_image(load_txt);
        else
            rl->image_gray = read_gray_image(load_txt);
        
        fclose(load_txt);
    }

    // Redimensiona a imagem se for maior que as dimensões da tela
    int screenWidth = GetScreenWidth() * 0.90;
    int screenHeight = GetScreenHeight() * 0.90;

    // Carrega a nova imagem e atualiza a textura
    Image new_image = LoadImage(file_path);

    if (new_image.width > screenWidth || new_image.height > screenHeight)
        adjust_image_size(&new_image);

    if (texture->id > 0)
        UpdateTexture(*texture, new_image.data);
    else
        *texture = LoadTextureFromImage(new_image);

    UnloadImage(new_image);
}
RandomList *random_efects(History *history, int width, int height)
{

    srand(time(NULL));
    RandomList *randomList = alloc_random();
    if(history->type == GRAY_)
        randomList->image_gray = history->gray_image;
    else
        randomList->image_rgb = history->rgb_image;

    RandomList *aux = randomList;
    if (history->type== GRAY_)
    {
            
        for (int i = 0; i < 5; i++)
        {
            int chosed = rand() % 5;

            switch (chosed)
            {
            case 0:
                aux->right->image_gray = flip_horizontal_gray(aux->image_gray);
                break;
            case 1:
                aux->right->image_gray = flip_vertical_gray(aux->image_gray);
                break;
            case 2:
                aux->right->image_gray = transpose_gray(aux->image_gray);
                break;
            case 3:
                aux->right->image_gray = median_blur_gray(aux->image_gray, 8);
                break;
            case 4:
                aux->right->image_gray = clahe_gray(aux->image_gray,width, height);
                break;

            aux=aux->right;
            
            }
        }
    }
    else
    {

        for (int i = 0; i < 5; i++)
        {
            int chosed = rand() % 5;

            switch (chosed)
            {
            case 0:
                aux->right->image_rgb = flip_horizontal_rgb(aux->image_rgb);
                break;
            case 1:
                aux->right->image_rgb = flip_vertical_rgb(aux->image_rgb);
                break;
            case 2:
                aux->right->image_rgb = transpose_rgb(aux->image_rgb);
                break;
            case 3:
                aux->right->image_rgb = median_blur_RGB(aux->image_rgb, 12);
                break;
            case 4:
                aux->right->image_rgb = clahe_rgb(aux->image_rgb,width, height);
                break;
            
            aux=aux->right;
            }
        }
    }
    return randomList;
}  


void random_effects(ImageType type, RandomList *rl)
{    
    srand(time(NULL));
    RandomList *aux = rl;

    int load_list = 0;

    if (IsFileDropped())
    {
        if(aux) free_random(aux);
        aux = alloc_random();
        aux->type = type;
        
        FilePathList droppedFiles = LoadDroppedFiles();
        
        txt_from_image(droppedFiles.paths[0], TXT_PATH, type);
        
        FILE *file_path = fopen(TXT_PATH, "r");


        if(type == RGB_)
            aux->image_rgb = read_rgb_image(file_path);
        else
            aux->image_gray = read_gray_image(file_path);

        fclose(file_path);
        UnloadDroppedFiles(droppedFiles);    // Unload filepaths from memory
    }

    int esc = 0;
    if(type)
    {
        for (int i = 0; i < 5; i++)
        {
            aux->right = alloc_random();
            esc = rand() % 5;
            switch(esc)
            {
            case 0:
                aux->right->image_rgb = median_blur_RGB(aux->image_rgb, 8);
                break;
            case 1:
                aux->right->image_rgb = clahe_rgb(aux->image_rgb, aux->image_rgb->dim.largura, aux->image_rgb->dim.altura);
                break;
            case 2: 
                aux->right->image_rgb = flip_vertical_rgb(aux->image_rgb);
                break;
            case 3: 
                aux->right->image_rgb = flip_horizontal_rgb(aux->image_rgb);
                break;
            case 4:
                aux->right->image_rgb = transpose_rgb(aux->image_rgb);
            default:
                break;
            }
            aux = aux->right;
            aux->type = type;
        }
    }
    else
    {
        for (int i = 0; i < 5; i++)
        {
            aux->right = alloc_random();
            esc = rand() % 5;
            switch(esc)
            {
            case 0:
                aux->right->image_gray = median_blur_gray(aux->image_gray, 8);
                break;
            case 1:
                aux->right->image_gray = clahe_gray(aux->image_gray, aux->image_gray->dim.largura, aux->image_gray->dim.altura);
                break;
            case 2: 
                aux->right->image_gray = flip_vertical_gray(aux->image_gray);
                break;
            case 3: 
                aux->right->image_gray = flip_horizontal_gray(aux->image_gray);
                break;
            case 4:
                aux->right->image_gray = transpose_gray(aux->image_gray);
            default:
                break;
            }
            aux = aux->right;
            aux->type = type;
        }
    }
    aux->right = NULL;
}

//////////////////////// screens //////////////////////////
MenuScreen current_screen = MAIN_MENU;
// tela principal relacionada as ações de historico
void main_menu_screen(History **history, bool *textureReload, ImageType actual_type, int currentProcess,Texture2D *texture, int mouseHoverRec, Rectangle *recs_main)
{
    if (IsFileDropped())
    {        
        FilePathList droppedFiles = LoadDroppedFiles();
        char *file_path = (char *)malloc(100 * sizeof(char));

        if(droppedFiles.count > 0)
            strcpy(file_path, droppedFiles.paths[0]);

        UnloadDroppedFiles(droppedFiles);    

        Image image = LoadImage(file_path);
        adjust_image_size(&image);
        *texture = LoadTextureFromImage(image);
    }

    if (*textureReload && currentProcess > 0)
    {
        FILE *load_txt = fopen(TXT_PATH, "w");
        void *new_image;

        switch(currentProcess)
        {
            case BLUR:
                if((*history)->type == RGB_)
                {
                    new_image = median_blur_RGB((*history)->rgb_image, 24);
                    save_image_rgb((ImageRGB *)new_image, load_txt);
                }
                else
                {
                    new_image = median_blur_gray((*history)->gray_image, 12);
                    save_image_gray((ImageGray *)new_image, load_txt);
                }
                
                *history = add_image(*history, new_image);
                break;
            case EQUALIZER:
                if((*history)->type == RGB_)
                {
                    new_image = clahe_rgb((*history)->rgb_image, (*history)->rgb_image->dim.largura, (*history)->rgb_image->dim.altura);
                    save_image_rgb((ImageRGB *)new_image, load_txt);
                }
                else
                {
                    new_image = clahe_gray((*history)->gray_image, (*history)->gray_image->dim.largura, (*history)->gray_image->dim.altura);
                    save_image_gray((ImageGray *)new_image, load_txt);
                }

                *history = add_image(*history, new_image); 
                ///////////////
                break;
            case VERTICAL:                
                if((*history)->type == RGB_)
                {
                    new_image = flip_vertical_rgb((*history)->rgb_image);
                    save_image_rgb((ImageRGB *)new_image, load_txt);
                }
                else
                {
                    new_image = flip_vertical_gray((*history)->gray_image);
                    save_image_gray((ImageGray *)new_image, load_txt);
                }

                *history = add_image(*history, new_image); 
                ///////////////
                break;
            case HORIZONTAL: 
                if((*history)->type == RGB_)
                {
                    new_image = flip_horizontal_rgb((*history)->rgb_image);
                    save_image_rgb((ImageRGB *)new_image, load_txt);
                }
                else
                {
                    new_image = flip_horizontal_gray((*history)->gray_image);
                    save_image_gray((ImageGray *)new_image, load_txt);
                }

                *history = add_image(*history, new_image);
                ///////////////
                break;
            case TRANSPOSE:
                if((*history)->type == RGB_)
                {
                    new_image = transpose_rgb((*history)->rgb_image);
                    save_image_rgb((ImageRGB *)new_image, load_txt);
                }
                else
                {
                    new_image = transpose_gray((*history)->gray_image);
                    save_image_gray((ImageGray *)new_image, load_txt);
                }

                *history = add_image(*history, new_image); 
                ///////////////
                break;
            case UNDO:
                // printf("%d\n", (*history)->type);
                if((*history)->left || (*history)->right) 
                {
                    *history = back_image(*history, 1);
                    ((*history)->type == RGB_) ? 
                        save_image_rgb((*history)->rgb_image, load_txt) : save_image_gray((*history)->gray_image, load_txt);
                    
                    if((*history)->type == RGB_)
                        image_rgb_from_txt(TXT_PATH, "image.png");
                    else 
                        image_gray_from_txt(TXT_PATH, "image.png");

                    load_new_texture(texture, *history, "image.png", 0);
                }
                ///////////////
                break;
            case NEXT:
                if((*history)->right) 
                {
                    *history = next_image(*history);
                    ((*history)->type == RGB_) ? 
                        save_image_rgb((*history)->rgb_image, load_txt) : save_image_gray((*history)->gray_image, load_txt);

                    if((*history)->type == RGB_)
                        image_rgb_from_txt(TXT_PATH, "image.png");
                    else 
                        image_gray_from_txt(TXT_PATH, "image.png");

                    load_new_texture(texture, *history, "image.png", 0);
                } 
                ///////////////
                break;
            case PREVIOUS:
                if((*history)->left) 
                {
                    *history = back_image(*history, 0);
                    ((*history)->type == RGB_) ? 
                        save_image_rgb((*history)->rgb_image, load_txt) : save_image_gray((*history)->gray_image, load_txt);

                    if((*history)->type == RGB_)
                        image_rgb_from_txt(TXT_PATH, "image.png");
                    else 
                        image_gray_from_txt(TXT_PATH, "image.png");

                    load_new_texture(texture, *history, "image.png", 0);
                } 
                break;
            case RANDOM_EFFECTS:
                currentProcess = NONE;
                current_screen = RANDOM_MENU;
                break;
            default: 
                break;
        }

        if(currentProcess < 6)
        {
            // transforma o history em png e atualiza a textura a ser mostrada
            if((*history)->type == RGB_)
            image_rgb_from_txt(TXT_PATH, "image.png");
            else 
                image_gray_from_txt(TXT_PATH, "image.png");

            load_new_texture(texture, *history, "image.png", 0);
        }

        *textureReload = false;
        remove("image.png");
        fclose(load_txt);
    }
}

// tela para as ações relacionadas a randomlist
RandomList *random_menu_screen(RandomList **rl, Texture2D *texture, ImageType type, int *current_proc, int mouse_hover, bool *textureReload)
{
    switch(*current_proc)
    {
    case RANDOM_NEXT:
        if((*rl)->right && *textureReload)
        {   
            FILE *file_path = fopen(TXT_PATH, "w");
            check_allocation(file_path, "file path");

            (*rl) = (*rl)->right;
            ((*rl)->type == RGB_) ? 
                save_image_rgb((*rl)->image_rgb, file_path) : save_image_gray((*rl)->image_gray, file_path);

            if((*rl)->type == RGB_)
                image_rgb_from_txt(TXT_PATH, IMAGE_PATH);
            else 
                image_gray_from_txt(TXT_PATH, IMAGE_PATH);

            // Load new image and update texture
            Image new_image = LoadImage(IMAGE_PATH);
            if(new_image.width > (GetScreenWidth() * 0.90) || new_image.height > (GetScreenHeight() * 0.90))
                adjust_image_size(&new_image);

            if(texture->id > 0) UpdateTexture(*texture, new_image.data);
            else *texture = LoadTextureFromImage(new_image);

            UnloadImage(new_image);
            fclose(file_path);
            *textureReload = false;
            remove(IMAGE_PATH);
        }
        break;
    case NEW_FIVE:
        if(*textureReload)
        {
            // RandomList *new = alloc_random(), *aux = (*rl);
            // while(aux->right) aux = aux->right;            
            // new->type = type;

            // if(type == RGB_) new->image_rgb = (*rl)->image_rgb;
            // else new->image_gray = (*rl)->image_gray;

            // random_effects(type, new);
            // aux->right = new->right;
            // printf("NEW FIVE\n");
            // *textureReload = false;
            // current_proc = NONE;

            RandomList *new = alloc_random();
            new->type = type;
            initialize_random_effects(new, type);
            random_effects(type, new);

            printf("NEW FIVE\n");
            *textureReload = false;
            current_proc = NONE;
            free_random((*rl));
            (*rl) = new;
            if(type == GRAY_) load_new_texture_random(texture, new, LENA_GRAY, 0);
            else load_new_texture_random(texture, new, LENA_RGB, 0);
        }
        break;
    case BACK_MENU:
        *current_proc = NONE; 
        current_screen = MAIN_MENU;
        break;
    default:
        break;
    }

    return (*rl);
}

void init()
{
    const int screenWidth = 700;
    const int screenHeight = 450;
    char load_type[] = "DROPPED FILE TYPE - RGB:  ";
    ///////////////////////////////////////////////////////////
    ImageType actual_type = RGB_;
    ///////////////////////////////////////////////////////////
    Texture2D texture, texture_random, texture_rgb, texture_gray, random_gray, random_rgb;
    ///////////////////////////////////////////////////////////
    History *history_gray = allocate_history();
    History *history_rgb = allocate_history();
    RandomList *randomlist_gray = alloc_random(); 
    RandomList *randomlist_rgb = alloc_random(); 
    ///////////////////////////////////////////////////////////
    InitWindow(screenWidth, screenHeight, "PROCESSAMENTO DE IMAGENS");
    ///////////////////////////////////////////////////////////
    Font font = LoadFont("utils/font.png");
    initialize_random_effects(randomlist_rgb, RGB_);
    initialize_random_effects(randomlist_gray, GRAY_);
    ///////////////////////////////////////////////////////////
    initialize_history(history_rgb, RGB_);
    initialize_history(history_gray, GRAY_);
    ///////////////////////////////////////////////////////////
    random_effects(RGB_, randomlist_rgb);
    random_effects(GRAY_, randomlist_gray);
    ///////////////////////////////////////////////////////////
    load_new_texture(&texture_rgb, history_rgb, LENA_RGB, 0);
    load_new_texture(&texture_gray, history_gray, LENA_GRAY, 0);
    load_new_texture_random(&random_rgb, randomlist_rgb, LENA_RGB, 0);
    load_new_texture_random(&random_gray, randomlist_gray, LENA_GRAY, 0);
    ///////////////////////////////////////////////////////////
    texture = texture_rgb;
    texture_random = random_rgb;
    ///////////////////////////////////////////////////////////
    int currentProcess = NONE;
    bool textureReload = false;

    // fazendo os retangulos de click
    Rectangle recs_main[NUM_PROCESSES] = {0};
    Rectangle recs_random[PROCESSES_RANDOM] = {0};
    
    int mouseHoverRec = -1;
    ///////////////////////////////////////
    for (int i = 0; i < NUM_PROCESSES; i++) 
        recs_main[i] = (Rectangle){ 40.0f, (float)(50 + 32*i), 150.0f, 30.0f };

    for(int i = 0; i < PROCESSES_RANDOM; i++)
        recs_random[i] = (Rectangle){ 40.0f, (float)(50 + 32*i), 150.0f, 30.0f };
 
    SetTargetFPS(60);
  
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        switch(current_screen)
        {
            case MAIN_MENU: 
                if(actual_type == RGB_)
                    main_menu_screen(&history_rgb, &textureReload, actual_type, currentProcess, &texture, mouseHoverRec, recs_main);
                else 
                    main_menu_screen(&history_gray, &textureReload, actual_type, currentProcess, &texture, mouseHoverRec, recs_main); 
                break;
            case RANDOM_MENU: 
                if(actual_type == RGB_)
                    random_menu_screen(&randomlist_rgb, &texture_random, actual_type, &currentProcess, mouseHoverRec, &textureReload); 
                else
                    random_menu_screen(&randomlist_gray, &texture_random, actual_type, &currentProcess, mouseHoverRec, &textureReload); 
                break;
            default: break;
        }

 
        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            if(actual_type == RGB_)
            {
                actual_type = GRAY_;
                texture = texture_gray;
                texture_random = random_gray;
                strcpy(load_type, "DROPPED FILE TYPE - GRAY:");
            }
            else
            {
                actual_type = RGB_;
                texture = texture_rgb;
                texture_random = random_rgb;
                strcpy(load_type, "DROPPED FILE TYPE - RGB:");
            }
        }
 
        // Mouse toggle group logic
        if(current_screen == MAIN_MENU)
        {
            for (int i = 0; i < NUM_PROCESSES; i++)
            {
                if (CheckCollisionPointRec(GetMousePosition(), recs_main[i]))
                {
                    mouseHoverRec = i;

                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                    {
                        currentProcess = i;
                        textureReload = true;
                    }
                    break;
                }
                else mouseHoverRec = -1;
            }
        }
        else
        {
            for (int i = 0; i < PROCESSES_RANDOM; i++)
            {
                if (CheckCollisionPointRec(GetMousePosition(), recs_random[i]))
                {
                    mouseHoverRec = i;

                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                    {
                        currentProcess = i;
                        textureReload = true;
                    }
                    break;
                }
                else mouseHoverRec = -1;
            }
        }

        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            switch(current_screen)
            {
            case MAIN_MENU:
                ClearBackground(DARKBROWN);

                DrawTextEx(font, load_type, (Vector2) {35, 40}, 20, 1, GRAY);

                // Draw rectangles
                for (int i = 1; i < NUM_PROCESSES; i++)
                {
                    DrawRectangleRec(recs_main[i], ((i == currentProcess) || (i == mouseHoverRec)) ? SKYBLUE : LIGHTGRAY);
                    DrawRectangleLines((int)recs_main[i].x, (int) recs_main[i].y, (int) recs_main[i].width, (int) recs_main[i].height, ((i == currentProcess) || (i == mouseHoverRec)) ? BLUE : GRAY);
                    DrawRectangleLinesEx(recs_main[i], 2, ((i == currentProcess) || (i == mouseHoverRec)) ? BLUE : GRAY); 
                    DrawText( processText[i], (int)( recs_main[i].x + recs_main[i].width/2 - MeasureText(processText[i], 10)/2), (int) recs_main[i].y + 11, 10, ((i == currentProcess) || (i == mouseHoverRec)) ? DARKBLUE : DARKGRAY);
                }

                DrawTexture(texture, screenWidth - texture.width - 60, screenHeight/2 - texture.height/2, WHITE);
                DrawRectangleLines(screenWidth - texture.width - 60, screenHeight/2 - texture.height/2, texture.width, texture.height, BLACK);
                DrawRectangleLinesEx((Rectangle){screenWidth - texture.width - 60, screenHeight/2 - texture.height/2, texture.width, texture.height}, 5, GRAY);
                break;
            case RANDOM_MENU:
                ClearBackground(RAYWHITE);
                DrawText(load_type, 40, 30, 10, DARKGRAY);
            
                for(int i = 1; i < PROCESSES_RANDOM; i++)
                {
                    DrawRectangleRec(recs_random[i], ((i == currentProcess) || (i == mouseHoverRec)) ? SKYBLUE : LIGHTGRAY);
                    DrawRectangleLines((int)recs_random[i].x, (int) recs_random[i].y, (int) recs_random[i].width, (int) recs_random[i].height, ((i == currentProcess) || (i == mouseHoverRec)) ? BLUE : GRAY);
                    DrawText( randomText[i], (int)( recs_random[i].x + recs_random[i].width/2 - MeasureText(randomText[i], 10)/2), (int) recs_random[i].y + 11, 10, ((i == currentProcess) || (i == mouseHoverRec)) ? DARKBLUE : DARKGRAY);
                }
                
                DrawTexture(texture_random, screenWidth - texture_random.width - 60, screenHeight/2 - texture_random.height/2, WHITE);
                DrawRectangleLines(screenWidth - texture_random.width - 60, screenHeight/2 - texture_random.height/2, texture_random.width, texture_random.height, BLACK);
                break;
            default:
                break;
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    remove("load.txt");
    // Liberar texturas
    UnloadTexture(texture);
    UnloadTexture(texture_random);
    UnloadTexture(texture_rgb);
    UnloadTexture(texture_gray);
    UnloadTexture(random_gray);
    UnloadTexture(random_rgb);
    // liberar font
    UnloadFont(font);
    // Liberar históricos
    free_history(history_gray);
    free_history(history_rgb);
    // Liberar listas aleatórias
    free_random(randomlist_gray);
    free_random(randomlist_rgb);
    // Fechar a janela do Raylib
    CloseWindow();
    //--------------------------------------------------------------------------------------
}



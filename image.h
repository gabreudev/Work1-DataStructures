#ifndef IMAGE_H
#define IMAGE_H

#include<raylib.h>

#define NIVEL_INTENSIDADE 256
#define KEEP 1
#define DELETE 0
#define TXT_PATH "load.txt"
#define IMAGE_PATH "image.png"
#define LENA_RGB "utils/lena.png"
#define LENA_GRAY "utils/lena_gray.png"
#define NUM_PROCESSES  10
#define PROCESSES_RANDOM 4

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


typedef enum imageType {
    GRAY_,
    RGB_
} ImageType;

typedef enum menuScreen {
    MAIN_MENU,
    RANDOM_MENU
} MenuScreen;

typedef enum {
    NONE = 0,
    BLUR,
    EQUALIZER,
    VERTICAL,
    HORIZONTAL,
    TRANSPOSE,
    UNDO,
    NEXT,
    PREVIOUS,
    RANDOM_EFFECTS
} ImageProcess;
typedef enum {
    NOONE,
    RANDOM_NEXT,
    NEW_FIVE,
    BACK_MENU
} RandomProcess;

static const char *processText[] = {
    "NO PROCESSING",
    "BLUR",
    "EQUALIZER",
    "VERTICAL",
    "HORIZONTAL",
    "TRANSPOSE",
    "UNDO",
    "NEXT >",
    "< PREVIOUS",
    "RANDOM EFFECTS"
};

static const char *randomText[] = {
    "NO PROCESSING",
    "NEXT >",
    "NEW FIVE",
    "BACK TO EDIT MENU"
};


typedef struct history {
    ImageType type;
    ImageGray *gray_image;
    ImageRGB *rgb_image;
    struct history *right; 
    struct history *left;  
} History;

typedef struct randomList {
    ImageGray *image_gray;
    ImageRGB *image_rgb;

    ImageType type;
    struct randomList *right;   
} RandomList;


// Operações de ERRO
void check_allocation(void *pointer, const char *mensage);

int vector_position(int largura, int i, int j);

// Funções de criação e liberação

ImageGray *create_image_gray(int largura, int altura);
void free_image_gray(ImageGray *image);
void free_pixel_gray(PixelGray *pixel);

ImageRGB *create_image_rgb(int largura, int altura);
void free_image_rgb(ImageRGB *image);
void free_pixel_rgb(PixelRGB *pixel);

History *allocate_history();
void free_history(History *history);

// aloca a linked list random 
RandomList *alloc_random();
// libera a linked list random 
void free_random(RandomList *list);

//////////////////////

// função pra adicionar um novo node, ela vai receber o historico e uma imagem, 
// convertendo automaticamente dependendo do tipo de historico 
History *add_image(History *history, void *image);

// retorna a imagem anterior, desfazendo ou não a ultima imagem
History *back_image(History *history, int mode);
 // função de refazer operações, avançando para a próxima imagem, se possível.
History *next_image(History *history);
// função de navegação pelo histórico, permitindo ir para versões específicas da imagem.
History *browse_history(History *history, int version);


////// Operações de arquivos /////////
// FILE *open_file(char *name, char *operation);

// Ler txt e converter em imagem -> Image
ImageRGB *read_rgb_image(FILE *arquivo);
ImageGray *read_gray_image(FILE *arquivo);

// Exportar imagem RGB para txt -> txt salvo
void save_image_rgb(ImageRGB *image, FILE *arquivo);
void save_image_gray(ImageGray *image, FILE *arquivo);

// Operações para ImageGray
ImageGray *flip_vertical_gray(ImageGray *image);
ImageGray *flip_horizontal_gray(ImageGray *image);
ImageGray *transpose_gray(const ImageGray *image);

// Operações para ImageRGB
ImageRGB *flip_vertical_rgb(const ImageRGB *image);
ImageRGB *flip_horizontal_rgb(const ImageRGB *image);
ImageRGB *transpose_rgb(const ImageRGB *image);

// Manipulação por pixel para ImageGray

// AUXILIARES
/// Procura o Valor minimo do CDF maior que zero
int min_cdf(int *CDF);
// Calcula a soma cumulativa dos valores do histograma
int *cumulative_histogram(int *histogram);

/////////////////

// Calcula cada tile individual do histograma 
void histogram_tile_gray(const ImageGray *image, int *histogram, int x1, int x2, int tile_width, int tile_height);

// Recebe o CDF e calcula o novo valor do pixel para a imagem
void histogram_equalizer_gray(const ImageGray *image,  ImageGray *equalize, int *CDF, int tile_width, int tile_height, int tile_increasew, int tile_increaseh);

// Aplica o clahe, retornando a imagem equalizada 
ImageGray *clahe_gray(const ImageGray *image, int tile_width, int tile_height);

// Calcula a soma dos tiles ao redor do central
int soma_kernel_gray(const ImageGray *image, int index_i, int index_j, int kernel);

// Substitui cada pixel pela média dos pixels em sua vizinhança
ImageGray *median_blur_gray(const ImageGray *image, int kernel_size);

// Manipulação por pixel para ImageRGB
// RGB: Calcula a soma cumulativa dos valores do histograma
PixelRGB *cumulative_histogram_rgb(PixelRGB *histogram);

// RGB: Calcula cada tile individual do histograma 
void histogram_tile_rgb(const ImageRGB *image, PixelRGB *histogram, int x1, int x2, int tile_width, int tile_height);

// RGB: Recebe o CDF e calcula o novo valor do pixel para a imagem
void equalize_tile_rgb(const ImageRGB *image, ImageRGB *equalized,PixelRGB *CDF, int tile_width, int tile_height, int tile_increasew, int tile_increaseh);

// RGB: Aplica o clahe, retornando a imagem equalizada 
ImageRGB *clahe_rgb(const ImageRGB *image, int tile_width, int tile_height);

// compara o pixeis para usar o qsort
int compare_pixel(const void *a, const void *b);

// ordenar os pixeis usando qsort
PixelRGB *sort_pixel(PixelRGB *pixel, int ind);

// RGB: Calcula a soma dos tiles ao redor do central
PixelRGB *soma_kernel_RGB(const ImageRGB *image, int index_i, int index_j, int kernel);

// RGB: substitui cada pixel pela média dos pixels em sua vizinhança
ImageRGB *median_blur_RGB(const ImageRGB *image, int kernel_size);

// iniicializa o historico com o tipo de imagem desejado
void initialize_history(History *history, ImageType type);
// iniicializa a randomlist com o tipo de imagem desejado
void initialize_random_effects(RandomList *rl, ImageType type);

//////////////////// PYTHON STUFF ////////////////////////////////////////

// eh menino ou menina 
int gray_or_rgb(int type);
// ajusta o tamanho da imagem a ser mostrada na interface 
void adjust_image_size(Image *image);

/////////////////// TEXTURES ///////////////////////////////////////

// carrega a nova imagem que será mostrada na tela 
void load_new_texture(Texture2D *texture, History *history, char *file_path, int mode);
// carrega a nova imagem que será mostrada na tela
void load_new_texture_random(Texture2D *texture, RandomList *rl, char *file_path, int mode);

/////////////////////// RANDOM STUFF //////////////////////////////////////

// aplica 5 efeitos aleatorios para uma imagem 
void random_effects(ImageType type, RandomList *rl);

/////////////////// SCREENS ///////////////////
void main_menu_screen(History **history, bool *textureReload, ImageType actual_type, int currentProcess,Texture2D *texture, int mouseHoverRec, Rectangle *recs_main);
RandomList *random_menu_screen(RandomList **rl, Texture2D *texture, ImageType type, int *current_proc, int mouse_hover, bool *textureReload);
// inicializa todo o programa
void init(void);

#endif // IMAGE_H
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


typedef enum imageType {
    GRAY,
    RGB
} ImageType;


typedef struct history {
    void *image;
    ImageType type;
    struct history *right; 
    struct history *left;  
} History;


// Operações de ERRO
void check_allocation(void *pointer, const char *mensage);

// Funções de criação e liberação
ImageGray *create_image_gray(int largura, int altura);
void free_image_gray(ImageGray *image);
void free_pixel_gray(PixelGray *pixel);

ImageRGB *create_image_rgb(int largura, int altura);
void free_image_rgb(ImageRGB *image);
void free_pixel_RGB(PixelRGB *pixel);

History *allocate_history();
void free_history(History *history);

//////////////////////
int verify_NULL(History *history);

History *back_image(History *history, int mode);
 // função de refazer operações, avançando para a próxima imagem, se possível.
History *next_image(History *history);
// função de navegação pelo histórico, permitindo ir para versões específicas da imagem.
History *browse_history(History *history, int version);


////// Operações de arquivos /////////
FILE *open(char *name, char *operation);

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
void histogram_equalizer_gray(ImageGray *image, int *CDF, int tile_width, int tile_height, int tile_increasew, int tile_increaseh);

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
void equalize_tile_rgb(ImageRGB *image, PixelRGB *CDF, int tile_width, int tile_height, int tile_increasew, int tile_increaseh);

// RGB: Aplica o clahe, retornando a imagem equalizada 
ImageRGB *clahe_rgb(const ImageRGB *image, int tile_width, int tile_height);

// RGB: Calcula a soma dos tiles ao redor do central
PixelRGB *soma_kernel_RGB(const ImageRGB *image, int index_i, int index_j, int kernel);

// RGB: substitui cada pixel pela média dos pixels em sua vizinhança
ImageRGB *median_blur_RGB(const ImageRGB *image, int kernel_size);

#endif // IMAGE_H

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "image_utils.h"

// Checa a alocação de memoria 
void check_alloc(void *pointer, const char *mensage) 
{
    if(!pointer) 
    {
        fprintf(stderr, "Erro ao alocar memória para %s: %d - %s\n", mensage, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void txt_from_image(const char* image_path, const char* output_path, int gray) 
{
    int width, height, channels;
    unsigned char* img = stbi_load(image_path, &width, &height, &channels, 0);
    check_alloc(img, "imagem");

    FILE* file = fopen(output_path, "w");
    check_alloc(file, "arquivo");

    // Escreve largura e altura
    fprintf(file, "%d\n%d\n", width, height);

    for (int y = 0; y < height; y++) 
    {
        for (int x = 0; x < width; x++) 
        {
            int idx = (y * width + x) * channels;
            if (gray && channels >= 3) 
            {
                // Escala de cinza: calcula a média ponderada
                int pixel = (int)(0.299 * img[idx] + 0.587 * img[idx + 1] + 0.114 * img[idx + 2]);
                fprintf(file, "%d,", pixel);
            } 
            else if (!gray && channels >= 3) // RGB: escreve R G B separados por espaços, com vírgula no final
                fprintf(file, "%d %d %d,", img[idx], img[idx + 1], img[idx + 2]);
            else // Caso a imagem já seja em escala de cinza ou tenha apenas um canal
                fprintf(file, "%d,", img[idx]);

        }
        fprintf(file, "\n");
    }

    fclose(file);
    stbi_image_free(img);
}

void image_gray_from_txt(const char* txt_path, const char* output_path) 
{
    FILE* file = fopen(txt_path, "r");
    check_alloc(file, "arquivo");

    int width = 0, height = 0;
    char line[1024];

    if (!fgets(line, sizeof(line), file)) 
    {
        printf("Erro ao ler largura\n");
        fclose(file);
        return;
    }

    width = atoi(line);

    if (!fgets(line, sizeof(line), file))
    {
        printf("Erro ao ler altura\n");
        fclose(file);
        return;
    }

    height = atoi(line);

    unsigned char* pixels = (unsigned char*)calloc(width * height, sizeof(unsigned char));
    check_alloc(pixels, "pixels");

    for (int i = 0, cont = 0; i < width * height; i++, cont++) 
    {
        if (cont == width) 
        {
            fgetc(file); // Pula a quebra de linha
            cont = 0;
        }

        int value;
        if (fscanf(file, "%d,", &value) == 1 && value >= 0 && value <= 255) 
            pixels[i] = (unsigned char)value;
        else 
            printf("Erro ao ler pixel %d\n", i);
    }

    fclose(file);
    stbi_write_png(output_path, width, height, 1, pixels, width);
    free(pixels);
}

void image_rgb_from_txt(const char* txt_path, const char* output_path) 
{
    FILE* file = fopen(txt_path, "r");
    check_alloc(file, "arquivo");

    int width = 0, height = 0;
    char line[1024];

    if (!fgets(line, sizeof(line), file)) 
    {
        printf("Erro ao ler largura\n");
        fclose(file);
        return;
    }

    width = atoi(line);

    if (!fgets(line, sizeof(line), file)) 
    {
        printf("Erro ao ler altura\n");
        fclose(file);
        return;
    }

    height = atoi(line);

    unsigned char* pixels = (unsigned char*)calloc(width * height * 3, sizeof(unsigned char));
    check_alloc(pixels, "pixels");

    for (int i = 0, cont = 0; i < width * height; i++, cont++) 
    {
        if (cont == width) 
        {
            fgetc(file); // Pula a quebra de linha
            cont = 0;
        }

        int r, g, b;

        if (fscanf(file, "%d %d %d,", &r, &g, &b) == 3 &&
            r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255) {
            pixels[i * 3 + 0] = (unsigned char)r;
            pixels[i * 3 + 1] = (unsigned char)g;
            pixels[i * 3 + 2] = (unsigned char)b;
        } else {
            printf("Erro ao ler pixel %d\n", i);
        }
    }

    fclose(file);
    stbi_write_png(output_path, width, height, 3, pixels, width * 3);
    free(pixels);
}


// int main() {
//     txt_from_image("lena.png", "rgb.txt", 0);
//     // image_rgb_from_txt("load.txt", "output_rgb.png");

//     txt_from_image("lena.png", "gray.txt", 1);
//     // image_gray_from_txt("load.txt", "output_gray.png");
//     return 0;
// }
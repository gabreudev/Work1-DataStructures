#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"

// Prototipação das funções de efeitos para facilitar a escolha
ImageGray* apply_gray_effect(ImageGray* image, int choice);
ImageRGB* apply_rgb_effect(ImageRGB* image, int choice);

// Função para imprimir o menu de opções
void print_menu() {
    printf("Escolha um efeito para aplicar:\n");
    printf("1. Flip Vertical\n");
    printf("2. Flip Horizontal\n");
    printf("3. Transpose\n");
    printf("4. CLAHE\n");
    printf("5. Median Blur\n");
    printf("6. Sair\n");
}

int main() {
    History* history = allocate_history();
    char filename[100];
    int img_type, choice;


    FILE* file = open("lena.txt", "r");

    printf("Escolha o tipo de imagem:\n1. Grayscale\n2. RGB\n");
    scanf("%d", &img_type);
    printf("passou");
    if (img_type == 1) {
        ImageGray* img_gray = read_gray_image(file);
        history->image = img_gray;
        history->type = GRAY;
    } else {
        ImageRGB* img_rgb = read_rgb_image(file);
        history->image = img_rgb;
        history->type = RGB;
    }

    fclose(file);

    History* current = history;

    while (1) {
        print_menu();
        scanf("%d", &choice);

        if (choice == 6) {
            break;
        }

        History* new_history = allocate_history();
        new_history->left = current;
        current->right = new_history;

        if (history->type == GRAY) {
            new_history->image = apply_gray_effect(current->image, choice);
            new_history->type = GRAY;
        } else {
            new_history->image = apply_rgb_effect(current->image, choice);
            new_history->type = RGB;
        }

        current = new_history;
    }

    printf("Digite o nome do arquivo para salvar a imagem final (incluindo a extensão): ");
    scanf("%s", filename);

    FILE* output_file = open(filename, "w");

    if (history->type == GRAY) {
        save_image_gray(current->image, output_file);
    } else {
        save_image_rgb(current->image, output_file);
    }

    fclose(output_file);
    free_history(history);

    return 0;
}

ImageGray* apply_gray_effect(ImageGray* image, int choice) {
    switch (choice) {
        case 1:
            return flip_vertical_gray(image);
        case 2:
            return flip_horizontal_gray(image);
        case 3:
            return transpose_gray(image);
        case 4: {
            int tile_width, tile_height;
            printf("Digite a largura e a altura do tile para CLAHE: ");
            scanf("%d %d", &tile_width, &tile_height);
            return clahe_gray(image, tile_width, tile_height);
        }
        case 5: {
            int kernel_size;
            printf("Digite o tamanho do kernel para Median Blur: ");
            scanf("%d", &kernel_size);
            return median_blur_gray(image, kernel_size);
        }
        default:
            printf("Opção inválida!\n");
            return image;
    }
}

ImageRGB* apply_rgb_effect(ImageRGB* image, int choice) {
    switch (choice) {
        case 1:
            return flip_vertical_rgb(image);
        case 2:
            return flip_horizontal_rgb(image);
        case 3:
            return transpose_rgb(image);
        case 4: {
            int tile_width, tile_height;
            printf("Digite a largura e a altura do tile para CLAHE: ");
            scanf("%d %d", &tile_width, &tile_height);
            return clahe_rgb(image, tile_width, tile_height);
        }
        case 5: {
            int kernel_size;
            printf("Digite o tamanho do kernel para Median Blur: ");
            scanf("%d", &kernel_size);
            return median_blur_RGB(image, kernel_size);
        }
        default:
            printf("Opção inválida!\n");
            return image;
    }
}

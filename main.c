#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"


int main() 
{
    History* history = allocate_history();

    FILE *file = open("utils/input_image_example_RGB.txt", "r");

    // history->type = GRAY;
    history->type = RGB;
    history->image = read_rgb_image(file);

    ImageRGB *teste = flip_horizontal_rgb(history->image);


    // FILE *saida = open("LENA_EQUALIZADA.txt", "w");
    FILE *saida = open("LENA_TESTE.txt", "w");
    save_image_rgb(history->image, saida);
    fclose(saida);
    free_history(history);
}
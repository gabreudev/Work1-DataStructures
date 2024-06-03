#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "image.c"


int main() 
{
    History* history = allocate_history();

    FILE *file = open("utils/input_image_example_RGB.txt", "r");

    history->type = GRAY;
    history->image = read_rgb_image(file);

    ImageRGB *teste = clahe_rgb(history->image, 512, 512);
    // ImageRGB *teste = median_blur_RGB(history->image, 12);

    FILE *saida = open("LENA_EQUALIZADA.txt", "w");
    // FILE *saida = open("LENA_BLUR.txt", "w");
    save_image_rgb(teste, saida);
    fclose(saida);
}
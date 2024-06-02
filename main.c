#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "image.c"


int main() 
{
    History* history = allocate_history();
    int img_type;


    FILE *file = open("utils/input_image_example_Gray.txt", "r");

    history->type = GRAY;
    history->image = read_gray_image(file);

    ImageGray *teste = clahe_gray(history->image, 64, 64);

    FILE *saida = open("LENA_EQUALIZADA.txt", "w");
    save_image_gray(teste, saida);
    fclose(saida);
}
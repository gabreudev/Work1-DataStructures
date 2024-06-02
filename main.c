#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"





int main() {
    History* history = allocate_history();
    int img_type;


    FILE *file = open("utils/input_image_example_Gray.txt", "r");

    printf("Escolha o tipo de imagem:\n1. Grayscale\n2. RGB\n");
    scanf("%d", &img_type);
    history->type = img_type;

    if(img_type - 1 == RGB) 
        history->image = create_image_rgb(512, 512); 
    else
        history->image = create_image_gray(512, 512);

    printf("virou");
    ImageRGB *image = read_rgb_image(file);

    ////////////////GRAY/////////////////
    // ImageGray *fliped = flip_vertical_gray(image); [OK]
    //ImageGray *fliped = flip_horizontal_gray(image); [OK]
    //ImageGray *fliped = transpose_gray(image); [OK]
    /////////////////////////////////////
    /////////////////RGB/////////////////
    ImageRGB *fliped = flip_horizontal_rgb(image);
    /////////////////////////////////////


    FILE *fileOuth = open("outhput_image_example_RGB.txt", "w");
    save_image_rgb(fliped, fileOuth);
    fclose(file);
    fclose(fileOuth);

   

}
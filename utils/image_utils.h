#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include "stb_image.h"
#include "stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


// Checa a alocação de memoria 
void check_alloc(void *pointer, const char *mensage) ;

// Função para converter imagem em txt
// A função lê a imagem, converte os valores de intensidade e salva como um arquivo txt
void txt_from_image(const char* image_path, const char* output_path, int gray);

// Função para converter txt em imagem em escala de cinza
// A função lê o arquivo txt, converte os valores de intensidade e salva como uma imagem PNG
void image_gray_from_txt(const char* txt_path, const char* output_path);

// Função para converter txt em imagem RGB
// A função lê o arquivo txt, converte os valores RGB e salva como uma imagem PNG
void image_rgb_from_txt(const char* txt_path, const char* output_path);

#endif
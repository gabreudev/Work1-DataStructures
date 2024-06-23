#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
// #include "image.c"
#include "raylib.h"
#include <Python.h>


int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    char load_type[] = "DROPPED FILE TYPE - RGB:  ";
    ImageType actual_type = RGB_;
  
    InitWindow(screenWidth, screenHeight, "PROCESSAMENTO DE IMAGENS");

    const char initial_path[] = {"utils/lena.png"};
  
    Image texture_image = LoadImage(initial_path);   // Loaded in CPU memory (RAM)
    ImageFormat(&texture_image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    // ImageFormat(&texture_image, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
    // adjust_image_format(&texture_image);         // Format image to RGBA 32bit (required for texture update) <-- ISSUE
    Texture2D texture = LoadTextureFromImage(texture_image);    // Image converted to texture, GPU memory (VRAM)

    Image copy_image = ImageCopy(texture_image);

    History *history = allocate_history();
    // imagem inicial rgb
    history->type = RGB_;

    FILE *initial_image = fopen("utils/input_image_example_RGB.txt", "r");
    check_allocation(initial_image, "initial_image");

    history->rgb_image = read_rgb_image(initial_image);

    fclose(initial_image);

    // imagem inicial gray
    // history->type = GRAY_;

    // FILE *initial_image = fopen("utils/input_image_example_Gray.txt", "r");
    // check_allocation(initial_image, "initial_image");

    // history->gray_image = read_gray_image(initial_image);

    // fclose(initial_image);
    // printf("a\n");
    ///////////////////////////////////////////////////////////

    int currentProcess = NONE;
    bool textureReload = false;

    // fazendo os retangulos de click
    Rectangle toggleRecs[NUM_PROCESSES] = { 0 };
    int mouseHoverRec = -1;

    for (int i = 0; i < NUM_PROCESSES; i++) 
        toggleRecs[i] = (Rectangle){ 40.0f, (float)(50 + 32*i), 150.0f, 30.0f };

    SetTargetFPS(60);

  
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsFileDropped())
        {
            free_history(history);

            history = allocate_history();
            history->type = actual_type;
            
            FilePathList droppedFiles = LoadDroppedFiles();
            // printf("new file droped\n");
            char *file_path = (char *)malloc(100 * sizeof(char));

            if(droppedFiles.count > 0)
                strcpy(file_path, droppedFiles.paths[0]);

            UnloadDroppedFiles(droppedFiles);    // Unload filepaths from memory
           
            load_new_texture(&texture_image, &texture, history, file_path, 1);
            textureReload = false;
            free(file_path);
        }

        // Mouse toggle group logic
        for (int i = 0; i < NUM_PROCESSES; i++)
        {
            if (CheckCollisionPointRec(GetMousePosition(), toggleRecs[i]))
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

        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            if(actual_type == RGB_)
            {
                actual_type = GRAY_;
                strcpy(load_type, "DROPPED FILE TYPE - GRAY:");
            }
            else
            {
                actual_type = RGB_;
                strcpy(load_type, "DROPPED FILE TYPE - RGB:");
            }
        }

        // Reload texture when required
        if (textureReload)
        {
            FILE *load_txt = fopen(TXT_PATH, "w");
            void *new_image;

            switch(currentProcess)
            {
                case BLUR:
                    if(history->type == RGB_)
                    {
                        new_image = median_blur_RGB(history->rgb_image, 12);
                        save_image_rgb((ImageRGB *)new_image, load_txt);
                    }
                    else
                    {
                        new_image = median_blur_gray(history->gray_image, 12);
                        save_image_gray((ImageGray *)new_image, load_txt);
                    }
                    
                    history = add_image(history, new_image);
                    break;
                case EQUALIZER:
                    if(history->type == RGB_)
                    {
                        new_image = clahe_rgb(history->rgb_image, history->rgb_image->dim.largura, history->rgb_image->dim.altura);
                        save_image_rgb((ImageRGB *)new_image, load_txt);
                    }
                    else
                    {
                        new_image = clahe_gray(history->gray_image, history->gray_image->dim.largura, history->gray_image->dim.altura);
                        save_image_gray((ImageGray *)new_image, load_txt);
                    }

                    history = add_image(history, new_image); 
                     ///////////////
                    break;
                case VERTICAL:                
                    if(history->type == RGB_)
                    {
                        new_image = flip_vertical_rgb(history->rgb_image);
                        save_image_rgb((ImageRGB *)new_image, load_txt);
                    }
                    else
                    {
                        new_image = flip_vertical_gray(history->gray_image);
                        save_image_gray((ImageGray *)new_image, load_txt);
                    }

                    history = add_image(history, new_image); 

                    ///////////////
                    break;
                case HORIZONTAL: 
                    if(history->type == RGB_)
                    {
                        new_image = flip_horizontal_rgb(history->rgb_image);
                        save_image_rgb((ImageRGB *)new_image, load_txt);
                    }
                    else
                    {
                        new_image = flip_horizontal_gray(history->gray_image);
                        save_image_gray((ImageGray *)new_image, load_txt);
                    }

                    history = add_image(history, new_image);
                    ///////////////
                    break;
                case TRANSPOSE:
                    if(history->type == RGB_)
                    {
                        new_image = transpose_rgb(history->rgb_image);
                        save_image_rgb((ImageRGB *)new_image, load_txt);
                    }
                    else
                    {
                        new_image = transpose_gray(history->gray_image);
                        save_image_gray((ImageGray *)new_image, load_txt);
                    }

                    history = add_image(history, new_image); 
                    ///////////////
                    break;
                case UNDO:
                    if(history->left || history->right) 
                    {
                        history = back_image(history, 1);
                        (history->type == RGB_) ? 
                            save_image_rgb(history->rgb_image, load_txt) : save_image_gray(history->gray_image, load_txt);

                        image_from_txt(TXT_PATH, "image.png", history->type);
                        load_new_texture(&texture_image, &texture, history, "image.png", 0);
                    }
                    ///////////////
                    break;
                case NEXT:
                    if(history->right) 
                    {
                        history = next_image(history);
                        (history->type == RGB_) ? 
                            save_image_rgb(history->rgb_image, load_txt) : save_image_gray(history->gray_image, load_txt);

                        image_from_txt(TXT_PATH, "image.png", history->type);
                        load_new_texture(&texture_image, &texture, history, "image.png", 0);
                    } 
                    ///////////////
                    break;
                case PREVIOUS:
                    if(history->left) 
                    {
                        history = back_image(history, 0);
                        (history->type == RGB_) ? 
                            save_image_rgb(history->rgb_image, load_txt) : save_image_gray(history->gray_image, load_txt);

                        image_from_txt(TXT_PATH, "image.png", history->type);
                        load_new_texture(&texture_image, &texture, history, "image.png", 0);
                    } 
                    ///////////////
                    break;
                default: 
                    break;
            }

            if(currentProcess < 6)
            {
            // transforma o history em png e atualiza a textura a ser mostrada
                image_from_txt(TXT_PATH, "image.png", history->type);
                load_new_texture(&texture_image, &texture, history, "image.png", 0);
            }
                

            Color *pixels = LoadImageColors(texture_image);    // Load pixel data from image (RGBA 32bit)
            UpdateTexture(texture, pixels);             // Update texture with new image data
            UnloadImageColors(pixels);                  // Unload pixels data from RAM

            textureReload = false;
            remove("image.png");
            fclose(load_txt);
        }
      
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText(load_type, 40, 30, 10, DARKGRAY);

            // Draw rectangles
            for (int i = 0; i < NUM_PROCESSES; i++)
            {
                DrawRectangleRec(toggleRecs[i], ((i == currentProcess) || (i == mouseHoverRec)) ? SKYBLUE : LIGHTGRAY);
                DrawRectangleLines((int)toggleRecs[i].x, (int) toggleRecs[i].y, (int) toggleRecs[i].width, (int) toggleRecs[i].height, ((i == currentProcess) || (i == mouseHoverRec)) ? BLUE : GRAY);
                DrawText( processText[i], (int)( toggleRecs[i].x + toggleRecs[i].width/2 - MeasureText(processText[i], 10)/2), (int) toggleRecs[i].y + 11, 10, ((i == currentProcess) || (i == mouseHoverRec)) ? DARKBLUE : DARKGRAY);
            }

            DrawTexture(texture, screenWidth - texture.width - 60, screenHeight/2 - texture.height/2, WHITE);
            DrawRectangleLines(screenWidth - texture.width - 60, screenHeight/2 - texture.height/2, texture.width, texture.height, BLACK);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);       // Unload texture from VRAM
    UnloadImage(texture_image);        // Unload image-origin from RAM
    UnloadImage(copy_image);   // Unload image-copy from RAM
    free_history(history);          

    CloseWindow();                // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}


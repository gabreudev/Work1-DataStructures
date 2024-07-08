#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
// #include "image.c"
#include <Python.h>

MenuScreen current_screen = MAIN_MENU;

void initialize_history(History *history, ImageType type)
{
    FILE *initial_image;
    if(type == RGB_)
    {
        initial_image = fopen("utils/input_image_example_RGB.txt", "r");
        check_allocation(initial_image, "initial_image");

        history->type = RGB_;
        history->rgb_image = read_rgb_image(initial_image);
    }
    else
    {
        initial_image = fopen("utils/input_image_example_Gray.txt", "r");
        check_allocation(initial_image, "initial_image");

        history->gray_image = read_gray_image(initial_image);
        history->type = GRAY_;
    }
    fclose(initial_image);
}

void initialize_random_effects(RandomList *rl, ImageType type) 
{
    FILE *initial_image;
    if(type == RGB_) 
    {
        initial_image = fopen("utils/input_image_example_RGB.txt", "r");
        check_allocation(initial_image, "initial_image");

        rl->type = RGB_;
        rl->image_rgb = read_rgb_image(initial_image);
        rl->image_gray = NULL;
    } 
    else 
    {
        initial_image = fopen("utils/input_image_example_Gray.txt", "r");
        check_allocation(initial_image, "initial_image");

        rl->type = GRAY_;
        rl->image_gray = read_gray_image(initial_image);
        rl->image_rgb = NULL;
    }
    fclose(initial_image);
}


void load_new_texture_random(Texture2D *texture, RandomList *rl, char *file_path, int mode) 
{
    // Manda para o código Python se o modo for 1
    if (mode == 1) 
    {
        txt_from_image(file_path, TXT_PATH, rl->type);
        
        FILE *load_txt = fopen(TXT_PATH, "r");

        if (rl->type == RGB_)  
            rl->image_rgb = read_rgb_image(load_txt);
        else
            rl->image_gray = read_gray_image(load_txt);
        
        fclose(load_txt);
    }

    // Redimensiona a imagem se for maior que as dimensões da tela
    int screenWidth = GetScreenWidth() * 0.90;
    int screenHeight = GetScreenHeight() * 0.90;

    // Carrega a nova imagem e atualiza a textura
    Image new_image = LoadImage(file_path);

    if (new_image.width > screenWidth || new_image.height > screenHeight)
        adjust_image_size(&new_image);

    if (texture->id > 0)
        UpdateTexture(*texture, new_image.data);
    else
        *texture = LoadTextureFromImage(new_image);

    UnloadImage(new_image);
}



void main_menu_screen(History **history, bool *textureReload, ImageType actual_type, int currentProcess,Texture2D *texture, int mouseHoverRec, Rectangle *recs_main)
{
    if (IsFileDropped())
    {
        free_history(*history);

        *history = allocate_history();
        (*history)->type = actual_type;
        
        FilePathList droppedFiles = LoadDroppedFiles();
        char *file_path = (char *)malloc(100 * sizeof(char));

        if(droppedFiles.count > 0)
            strcpy(file_path, droppedFiles.paths[0]);

        UnloadDroppedFiles(droppedFiles);    // Unload filepaths from memory
        
        load_new_texture(texture, *history, file_path, 1);
        *textureReload = false;
        free(file_path);
    }

    if (*textureReload && currentProcess > 0)
    {
        FILE *load_txt = fopen(TXT_PATH, "w");
        void *new_image;

        switch(currentProcess)
        {
            case BLUR:
                if((*history)->type == RGB_)
                {
                    new_image = median_blur_RGB((*history)->rgb_image, 12);
                    save_image_rgb((ImageRGB *)new_image, load_txt);
                }
                else
                {
                    new_image = median_blur_gray((*history)->gray_image, 12);
                    save_image_gray((ImageGray *)new_image, load_txt);
                }
                
                *history = add_image(*history, new_image);
                break;
            case EQUALIZER:
                if((*history)->type == RGB_)
                {
                    new_image = clahe_rgb((*history)->rgb_image, (*history)->rgb_image->dim.largura, (*history)->rgb_image->dim.altura);
                    save_image_rgb((ImageRGB *)new_image, load_txt);
                }
                else
                {
                    new_image = clahe_gray((*history)->gray_image, (*history)->gray_image->dim.largura, (*history)->gray_image->dim.altura);
                    save_image_gray((ImageGray *)new_image, load_txt);
                }

                *history = add_image(*history, new_image); 
                ///////////////
                break;
            case VERTICAL:                
                if((*history)->type == RGB_)
                {
                    new_image = flip_vertical_rgb((*history)->rgb_image);
                    save_image_rgb((ImageRGB *)new_image, load_txt);
                }
                else
                {
                    new_image = flip_vertical_gray((*history)->gray_image);
                    save_image_gray((ImageGray *)new_image, load_txt);
                }

                *history = add_image(*history, new_image); 
                ///////////////
                break;
            case HORIZONTAL: 
                if((*history)->type == RGB_)
                {
                    new_image = flip_horizontal_rgb((*history)->rgb_image);
                    save_image_rgb((ImageRGB *)new_image, load_txt);
                }
                else
                {
                    new_image = flip_horizontal_gray((*history)->gray_image);
                    save_image_gray((ImageGray *)new_image, load_txt);
                }

                *history = add_image(*history, new_image);
                ///////////////
                break;
            case TRANSPOSE:
                if((*history)->type == RGB_)
                {
                    new_image = transpose_rgb((*history)->rgb_image);
                    save_image_rgb((ImageRGB *)new_image, load_txt);
                }
                else
                {
                    new_image = transpose_gray((*history)->gray_image);
                    save_image_gray((ImageGray *)new_image, load_txt);
                }

                *history = add_image(*history, new_image); 
                ///////////////
                break;
            case UNDO:
                // printf("%d\n", (*history)->type);
                if((*history)->left || (*history)->right) 
                {
                    *history = back_image(*history, 1);
                    ((*history)->type == RGB_) ? 
                        save_image_rgb((*history)->rgb_image, load_txt) : save_image_gray((*history)->gray_image, load_txt);

                    image_from_txt(TXT_PATH, "image.png", (*history)->type);
                    load_new_texture(texture, *history, "image.png", 0);
                }
                ///////////////
                break;
            case NEXT:
                if((*history)->right) 
                {
                    *history = next_image(*history);
                    ((*history)->type == RGB_) ? 
                        save_image_rgb((*history)->rgb_image, load_txt) : save_image_gray((*history)->gray_image, load_txt);

                    image_from_txt(TXT_PATH, "image.png", (*history)->type);
                    load_new_texture(texture, *history, "image.png", 0);
                } 
                ///////////////
                break;
            case PREVIOUS:
                if((*history)->left) 
                {
                    *history = back_image(*history, 0);
                    ((*history)->type == RGB_) ? 
                        save_image_rgb((*history)->rgb_image, load_txt) : save_image_gray((*history)->gray_image, load_txt);

                    image_from_txt(TXT_PATH, "image.png", (*history)->type);
                    load_new_texture(texture, *history, "image.png", 0);
                } 
                break;
            case RANDOM_EFFECTS:
                currentProcess = NONE;
                current_screen = RANDOM_MENU;
                break;
            default: 
                break;
        }

        if(currentProcess < 6)
        {
            // transforma o history em png e atualiza a textura a ser mostrada
            image_from_txt(TXT_PATH, "image.png", (*history)->type);
            load_new_texture(texture, *history, "image.png", 0);
        }

        *textureReload = false;
        remove("image.png");
        fclose(load_txt);
    }
}

RandomList *random_menu_screen(RandomList **rl, Texture2D *texture, ImageType type, int *current_proc, int mouse_hover, bool *textureReload)
{
    switch(*current_proc)
    {
    case RANDOM_NEXT:
        if((*rl)->right && *textureReload)
        {   
            FILE *file_path = fopen(TXT_PATH, "w");
            check_allocation(file_path, "file path");

            (*rl) = (*rl)->right;
            ((*rl)->type == RGB_) ? 
                save_image_rgb((*rl)->image_rgb, file_path) : save_image_gray((*rl)->image_gray, file_path);

            image_from_txt(TXT_PATH, IMAGE_PATH, (*rl)->type);

            // Load new image and update texture
            Image new_image = LoadImage(IMAGE_PATH);
            if(new_image.width > (GetScreenWidth() * 0.90) || new_image.height > (GetScreenHeight() * 0.90))
                adjust_image_size(&new_image);

            if(texture->id > 0) UpdateTexture(*texture, new_image.data);
            else *texture = LoadTextureFromImage(new_image);

            UnloadImage(new_image);
            fclose(file_path);
            *textureReload = false;
            remove(IMAGE_PATH);
        }
        break;
    case NEW_FIVE:
        RandomList *new_random = alloc_random(), *aux = (*rl);
        initialize_random_effects(new_random, type);

        while(aux->right) aux = aux->right;
        aux->right = new_random;

        // if(type == RGB_) load_new_texture_random(texture, new_random, LENA_RGB, 0);
        // else load_new_texture_random(texture, new_random, LENA_GRAY, 0);
        *textureReload = false;
        break;
    case BACK_MENU:
        *current_proc = NONE;
        current_screen = MAIN_MENU;
        break;
    default:
        break;
    }

    return (*rl);
}

int main(void)
{
    Py_Initialize();
    const int screenWidth = 700;
    const int screenHeight = 450;
    char load_type[] = "DROPPED FILE TYPE - RGB:  ";
    ImageType actual_type = RGB_;
    // Texture2D texture, texture_random;
    Texture2D texture, texture_random, texture_rgb, texture_gray, random_gray, random_rgb;
    // History *history = allocate_history();
    History *history_gray = allocate_history();
    History *history_rgb = allocate_history();
    RandomList *randomlist_gray = alloc_random(); 
    RandomList *randomlist_rgb = alloc_random(); 
    
    InitWindow(screenWidth, screenHeight, "PROCESSAMENTO DE IMAGENS");
    // initialize_random_effects(randomlist, actual_type);
    initialize_random_effects(randomlist_rgb, RGB_);
    initialize_random_effects(randomlist_gray, GRAY_);
    // initialize_history(history, actual_type);
    initialize_history(history_rgb, RGB_);
    initialize_history(history_gray, GRAY_);
    // random_effects(actual_type, randomlist);
    random_effects(RGB_, randomlist_rgb);
    random_effects(GRAY_, randomlist_gray);
    // load_new_texture(&texture, history, LENA_RGB, 0);
    load_new_texture(&texture_rgb, history_rgb, LENA_RGB, 0);
    load_new_texture(&texture_gray, history_gray, LENA_GRAY, 0);
    load_new_texture_random(&random_rgb, randomlist_rgb, LENA_RGB, 0);
    load_new_texture_random(&random_gray, randomlist_gray, LENA_GRAY, 0);
    ///////////////////////////////////////////////////////////
    texture = texture_rgb;
    texture_random = random_rgb;
    ///////////////////////////////////////////////////////////
    int currentProcess = NONE;
    bool textureReload = false;

    // fazendo os retangulos de click
    Rectangle recs_main[NUM_PROCESSES] = {0};
    Rectangle recs_random[PROCESSES_RANDOM] = {0};
    
    int mouseHoverRec = -1;
    ///////////////////////////////////////

    for (int i = 0; i < NUM_PROCESSES; i++) 
        recs_main[i] = (Rectangle){ 40.0f, (float)(50 + 32*i), 150.0f, 30.0f };

    for(int i = 0; i < PROCESSES_RANDOM; i++)
        recs_random[i] = (Rectangle){ 40.0f, (float)(50 + 32*i), 150.0f, 30.0f };
 
    SetTargetFPS(60);
  
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        switch(current_screen)
        {
            case MAIN_MENU: 
                if(actual_type == RGB_)
                    main_menu_screen(&history_rgb, &textureReload, actual_type, currentProcess, &texture, mouseHoverRec, recs_main);
                else 
                    main_menu_screen(&history_gray, &textureReload, actual_type, currentProcess, &texture, mouseHoverRec, recs_main); 
                break;
            case RANDOM_MENU: 
                if(actual_type == RGB_)
                    random_menu_screen(&randomlist_rgb, &texture_random, actual_type, &currentProcess, mouseHoverRec, &textureReload); 
                else
                    random_menu_screen(&randomlist_gray, &texture_random, actual_type, &currentProcess, mouseHoverRec, &textureReload); 
                break;
            default: break;
        }

 
        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            if(actual_type == RGB_)
            {
                actual_type = GRAY_;
                texture = texture_gray;
                texture_random = random_gray;
                strcpy(load_type, "DROPPED FILE TYPE - GRAY:");
            }
            else
            {
                actual_type = RGB_;
                texture = texture_rgb;
                texture_random = random_rgb;
                strcpy(load_type, "DROPPED FILE TYPE - RGB:");
            }
        }
 
        // Mouse toggle group logic
        if(current_screen == MAIN_MENU)
        {
            for (int i = 0; i < NUM_PROCESSES; i++)
            {
                if (CheckCollisionPointRec(GetMousePosition(), recs_main[i]))
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
        }
        else
        {
            for (int i = 0; i < PROCESSES_RANDOM; i++)
            {
                if (CheckCollisionPointRec(GetMousePosition(), recs_random[i]))
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
        }

        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            switch(current_screen)
            {
            case MAIN_MENU:
                ClearBackground(RAYWHITE);

                DrawText(load_type, 40, 30, 10, DARKGRAY);

                // Draw rectangles
                for (int i = 0; i < NUM_PROCESSES; i++)
                {
                    DrawRectangleRec(recs_main[i], ((i == currentProcess) || (i == mouseHoverRec)) ? SKYBLUE : LIGHTGRAY);
                    DrawRectangleLines((int)recs_main[i].x, (int) recs_main[i].y, (int) recs_main[i].width, (int) recs_main[i].height, ((i == currentProcess) || (i == mouseHoverRec)) ? BLUE : GRAY);
                    DrawText( processText[i], (int)( recs_main[i].x + recs_main[i].width/2 - MeasureText(processText[i], 10)/2), (int) recs_main[i].y + 11, 10, ((i == currentProcess) || (i == mouseHoverRec)) ? DARKBLUE : DARKGRAY);
                }

                DrawTexture(texture, screenWidth - texture.width - 60, screenHeight/2 - texture.height/2, WHITE);
                DrawRectangleLines(screenWidth - texture.width - 60, screenHeight/2 - texture.height/2, texture.width, texture.height, BLACK);
                break;
            case RANDOM_MENU:
                ClearBackground(RAYWHITE);
                DrawText(load_type, 40, 30, 10, DARKGRAY);
            
                for(int i = 0; i < PROCESSES_RANDOM; i++)
                {
                    DrawRectangleRec(recs_random[i], ((i == currentProcess) || (i == mouseHoverRec)) ? SKYBLUE : LIGHTGRAY);
                    DrawRectangleLines((int)recs_random[i].x, (int) recs_random[i].y, (int) recs_random[i].width, (int) recs_random[i].height, ((i == currentProcess) || (i == mouseHoverRec)) ? BLUE : GRAY);
                    DrawText( randomText[i], (int)( recs_random[i].x + recs_random[i].width/2 - MeasureText(randomText[i], 10)/2), (int) recs_random[i].y + 11, 10, ((i == currentProcess) || (i == mouseHoverRec)) ? DARKBLUE : DARKGRAY);
                }
                
                DrawTexture(texture_random, screenWidth - texture_random.width - 60, screenHeight/2 - texture_random.height/2, WHITE);
                DrawRectangleLines(screenWidth - texture_random.width - 60, screenHeight/2 - texture_random.height/2, texture_random.width, texture_random.height, BLACK);
                break;
            default:
                break;
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // Liberar texturas
    UnloadTexture(texture);
    UnloadTexture(texture_random);
    UnloadTexture(texture_rgb);
    UnloadTexture(texture_gray);
    UnloadTexture(random_gray);
    UnloadTexture(random_rgb);
    // Liberar históricos
    free_history(history_gray);
    free_history(history_rgb);
    // Liberar listas aleatórias
    free_random(randomlist_gray);
    free_random(randomlist_rgb);

    // Fechar a janela do Raylib
    CloseWindow();
               // Close window and OpenGL context
    Py_Finalize();
    //--------------------------------------------------------------------------------------

    return 0;
}


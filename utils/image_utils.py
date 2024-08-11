from PIL import Image

def txt_from_image_gray(image_path, output_path, gray=False):
    
    img = Image.open(image_path)

    if gray:
        img = img.convert('L')
    
    largura, altura = img.size
    pixels = list(img.getdata())
    
    with open(output_path, 'w') as file:
        file.write(f"{largura}\n")
        file.write(f"{altura}\n")
        for y in range(altura):
            for x in range(largura):
                pixel = str(pixels[y * largura + x]).replace(",", "").replace("(", "").replace(")", "")
                file.write(f"{pixel},")
            file.write("\n")
    print("sai da função")

def image_gray_from_txt(txt_path, output_path):
    with open(txt_path, 'r') as file:
        lines = file.readlines()
        gray_error = 0
        largura = int(lines[0].strip())
        altura = int(lines[1].strip())

        nova_imagem = Image.new('L', (largura, altura))

        for y in range(altura):
            for x in range(largura):
                try: 
                    gray_value = int(lines[2 + y].split(',')[x].strip())
                    nova_imagem.putpixel((x, y), gray_value)
                    gray_error = gray_value
                except:
                    nova_imagem.putpixel((x, y), gray_error)


        # Salva a imagem resultante
        nova_imagem.save(output_path)


def image_rgb_from_txt(txt_path, output_path):
    with open(txt_path, 'r') as file:
        lines = file.readlines()
        rgb_error = None
        largura = int(lines[0].strip())
        altura = int(lines[1].strip())

        nova_imagem = Image.new('RGB', (largura, altura))

        for y in range(altura - 1):
            for x in range(largura):
                try:
                    pixel = tuple(map(int, lines[2 + y].split(',')[x].strip().split()))
                    nova_imagem.putpixel((x, y), pixel) 
                    rgb_error = pixel
                except:
                    nova_imagem.putpixel((x, y), rgb_error) 

        # Salva a imagem resultante
        nova_imagem.save(output_path)



# image_gray_from_txt("Work1-DataStructures/utils/input_image_example_Gray.txt", "saida.png")
# image_rgb_from_txt("Work1-DataStructures/LENA_BLUR.txt", "saida_rgb.png")
# image_rgb_from_txt("Work1-DataStructures/LENA_EQUALIZADA.txt", "saida_rgb.png")
# image_rgb_from_txt("Work1-DataStructures/load.txt", "saida_rgb.png")

# image_gray_from_txt("Work1-DataStructures/utils/input_image_example_Gray.txt", "lena_gray.png");

# image_rgb_from_txt("Work1-DataStructures/load.txt", "image.jpeg")

# def execute():
#     image_rgb_from_txt("load.txt", "image.png")

# txt_from_image_gray("/home/alef/Downloads/logo-corinthians-2048.png" , "load.txt", gray=False)
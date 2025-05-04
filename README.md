# Manipulação de Imagens com Histórico e Efeitos Aleatórios

Este projeto implementa, em C, um sistema para manipulação de imagens (escala de cinza e RGB) com histórico reversível de operações e aplicação de efeitos aleatórios. O sistema possui interface gráfica baseada em Raylib e permite ao usuário aplicar operações de processamento de imagem, desfazer/refazer ações e visualizar o histórico.

## Funcionalidades

- **Suporte a imagens em escala de cinza e RGB**
- **Operações de processamento:** rotação, inversão, filtros (blur, equalização de histograma, transpose, etc)
- **Histórico reversível:** desfazer e refazer operações usando lista duplamente encadeada
- **Efeitos aleatórios:** aplicação automática de uma sequência de efeitos, exibidos em lista encadeada simples
- **Interface gráfica:** seleção de operações, navegação pelo histórico e visualização das imagens processadas

## Estrutura de Dados

- **Lista Duplamente Encadeada:** armazena o histórico de versões da imagem, permitindo navegação para frente e para trás
- **Lista Simplesmente Encadeada:** armazena a sequência de imagens geradas por efeitos aleatórios

## Organização dos Arquivos

- `image.h` / `image.c`: Estruturas, funções de manipulação de imagens, histórico e efeitos
- `utils/image_utils.h` / `utils/image_utils.c`: Funções auxiliares para conversão entre imagens e arquivos txt/png
- `utils/stb_image.h`: Biblioteca para leitura de imagens (stb_image)
- Outros arquivos de recursos (imagens de exemplo, fontes, etc)

## Como Executar

1. Compile o projeto usando o arquivo de build:

   ```./build.sh```
2. Use a interface gráfica para carregar imagens, aplicar operações, desfazer/refazer e experimentar efeitos aleatórios.

## Dependências

- [Raylib](https://www.raylib.com/) (para interface gráfica)
- [stb_image.h](https://github.com/nothings/stb) (incluso no projeto)

## Contribuições

Contribuições não são aceitas no momento, mas sinta-se à vontade para abrir issues ou discutir melhorias.

## Licença
Este projeto é de uso pessoal e educacional. Não é permitido o uso comercial sem autorização prévia.

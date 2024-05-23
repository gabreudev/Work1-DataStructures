**Descrição do Trabalho: Implementação de Operações de Manipulação de Imagens com Histórico de Reversão**

**Objetivo:**
O objetivo deste projeto é desenvolver uma solução para manipulação de imagens que permita a aplicação de várias operações, como rotação, inversão e filtros, em imagens em escala de cinza e em cores. Além disso, o sistema deve fornecer um histórico reversível das operações realizadas, permitindo ao usuário voltar para versões anteriores da imagem.

**Implementação:**
1. **Estrutura de Dados:**
   - Utilização de uma lista duplamente encadeada para armazenar o histórico de imagens. Cada nó da lista conterá uma referência para a imagem resultante de uma operação e ponteiros para o próximo e o anterior nós na lista.

2. **Operações de Manipulação de Imagem:**
   - Implementação de uma variedade de operações, como rotação, inversão (vertical e horizontal), aplicação de filtros (como o filtro de média), equalização de histograma e outras técnicas de processamento de imagem.
   - Cada operação deve retornar uma nova imagem processada, preservando a imagem original no histórico, esta sempre será a imagem inicial da lista.

3. **Interface do Usuário:**
   - Desenvolvimento de uma interface com as opcoes de menu indicada para cada tipo de imagem, i.e., RBG ou Gray, 
   - Visualize o histórico de operações (usar os cod python para gerar as imagens).
   - A interface deve fornecer controles para aplicar novas operações, desfazer operações e navegar pelo histórico (ou seja, pelas posicoes da lista).

4. **Operações randomicas:**
   - Nessa opcao basta apenas escolher um tipo de imagem a qual as operacoes serao aplicadas. Sortear de forma randomica quais operacoes serao aplicadas a imagem. Feito isso, apresentar numa lista encadeada simples, o resultado final, isto é, cada posicao da lista que corresponde a uma operacao, e o resultado final.


5. **Observaçoes:**
   - Muitas funcoes nao foram mapeadas, vai do programador fazer isso.
   - Atentar aos requisitos dispostos anteriormente




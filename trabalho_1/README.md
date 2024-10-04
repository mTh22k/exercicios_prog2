# 📖 README - TRABALHO 1, PROG2

> Este projeto implementa um sistema para processamento de imagens usando o método Local Binary Patterns (LBP). O sistema é capaz de calcular histogramas LBP, comparar imagens e gerar imagens LBP a partir de imagens de entrada. Abaixo está a descrição da estrutura do programa e dos arquivos que o compõem.

# 💻 1. Estrutura do Programa  

* ### LBP.c : Contém a função principal do programa e gerencia a lógica de execução.
* ### leitura.c : Contém funções para leitura e escrita de arquivos de imagem no formato PGM e de arquivos LBP.
* ### operações.c : Implementa funções relacionadas ao cálculo de LBP, histogramas e manipulação de imagens.
* ### processamento.c : Contém a lógica de processamento de imagem, incluindo a geração de histogramas LBP.
* ### LBP.h : Cabeçalho que declara as funções implementadas em LBP.c e outros arquivos. Define também constantes e inclui bibliotecas necessárias.
* ### Makefile : Script para compilar o programa.

# 💻 2. Descrição dos Arquivos

## 2.1 LBP.c

* ###   A função main é o ponto de entrada do programa e gerencia a execução com base nos argumentos fornecidos na linha de comando. Ela verifica se os argumentos estão corretos para encontrar a imagem mais similar ou para verificar a imagem de entrada. Dependendo da condição atendida, chama as funções achar_imagem_similar ou verificar_pgm. O programa termina com return 0, indicando uma execução bem-sucedida.

## 2.2 leitura.c
### Função ler_cabeçalho

* ### A função ler_cabeçalho é responsável por ler o cabeçalho de uma imagem PGM. Ela recebe um ponteiro para o arquivo de entrada e variáveis para armazenar informações como o número mágico (tipo da imagem), largura, altura e valor máximo dos pixels. O processo é realizado da seguinte forma:

#### 1. Ignorar Comentários: Chama ignorar_comentarios para pular linhas que começam com o caractere #.
#### 2. Ler Número Mágico: Utiliza fscanf para ler o tipo da imagem (P5 ou P2) e verifica se a leitura foi bem-sucedida.
#### 3. Verificar Tipo: Confirma se o tipo da imagem é válido; se não, retorna 0.
#### 4. Ler Dimensões: Lê a largura e altura da imagem.
#### 5. Ler Valor Máximo: Lê o valor máximo de pixel.
#### 6. Pular Nova Linha: Ignora o caractere de nova linha após o cabeçalho.
#### 7. Retorna 1 se tudo ocorrer corretamente.

### Função ignorar_comentarios

* ### A função ignorar_comentarios lê o arquivo de entrada e ignora linhas de comentários que começam com #. Quando encontra um caractere que não é #, usa ungetc para devolver o caractere ao fluxo, permitindo que a leitura continue normalmente.

### Função ler_p5

* ### A função ler_p5 lê os dados da imagem em formato P5 (imagem binária). Ela recebe o arquivo de entrada e um ponteiro para um array que armazena os dados da imagem. Utiliza fread para ler o número total de pixels. Retorna 0 se a leitura falhar ou 1 em caso de sucesso.

### Função salvar_arquivo_lbp

*  ### A função salvar_arquivo_lbp salva o histograma LBP em um arquivo binário. Ela recebe o nome do arquivo e um ponteiro para o histograma LBP, utilizando fwrite para gravar os dados. Fecha o arquivo após a escrita.

### Função carregar_arquivo_lbp

* ### A função carregar_arquivo_lbp carrega um histograma LBP a partir de um arquivo. Se o arquivo não puder ser aberto, retorna 0. Caso contrário, utiliza fread para ler os dados do histograma e retorna 1 se a leitura for bem-sucedida.

### Função ler_p2

* ### A função ler_p2 lê os dados da imagem em formato P2 (imagem ASCII). Para cada pixel, lê um valor inteiro e o converte para unsigned char, armazenando-o no array correspondente. Retorna 0 em caso de falha ou 1 em caso de sucesso.

## 2.3 operação.c

* ### Este conjunto de funções é responsável pela listagem de imagens no formato PGM em um diretório, verificação da existência de arquivos, extração do nome do arquivo a partir de um caminho, e busca de uma imagem similar com base no histograma LBP.

### Função listar_imagem_diretorio

* ### A função listar_imagem_diretorio abre um diretório e lista todos os arquivos com a extensão .pgm. A implementação realiza as seguintes etapas:

#### 1. Abre o Diretório: Utiliza opendir para abrir o diretório fornecido.
#### 2.    Ignora Entradas Padrão: Utiliza strcmp para ignorar as entradas . e ..
#### 3.    Verifica Extensão: Utiliza strstr e strcmp para garantir que apenas arquivos com a extensão .pgm sejam considerados.
#### 4.    Caminho Completo: Constrói o caminho completo do arquivo usando snprintf.
#### 5.    Verifica se é um Arquivo Regular: Utiliza stat para confirmar se a entrada é um arquivo regular.
#### 6.    Armazena na Lista: Copia o nome do arquivo para a lista e incrementa a contagem.

### Função verificar_arquivo_existe

* ### A função verificar_arquivo_existe utiliza stat para verificar se um arquivo existe no caminho fornecido. Retorna 1 se o arquivo existir ou 0 caso contrário.
### Função extrair_nome_arquivo

* ### A função extrair_nome_arquivo utiliza strrchr para localizar a última ocorrência do caractere / em um caminho de arquivo. Se encontrado, retorna o nome do arquivo; caso contrário, retorna o caminho completo.
### Função achar_imagem_similar

* ### A função achar_imagem_similar procura pela imagem mais similar a uma imagem de entrada em um diretório, utilizando a comparação de histogramas LBP. O fluxo de execução é o seguinte:

#### 1. Verificação de Existência: Verifica se a imagem de entrada existe utilizando verificar_arquivo_existe.
#### 2. Processamento da Imagem de Entrada: Processa a imagem de entrada e armazena o histograma LBP.
#### 3.    Verifica Processamento: Retorna se o histograma for um array de zeros.
#### 4.    Lista de Imagens: Lista as imagens no diretório usando listar_imagem_diretorio.
#### 5.    Comparação de Histogramas: Para cada imagem no diretório, calcula a distância Euclidiana entre os histogramas LBP e atualiza a imagem mais similar se a distância for menor que a mínima encontrada.
#### 6.    Exibição do Resultado: Se uma imagem similar for encontrada, imprime o nome da imagem e a distância.

### Função verificar_pgm

* ### A função verificar_pgm valida se a imagem de entrada possui a extensão .pgm e, se a validação for bem-sucedida, chama a função para gerar a imagem LBP correspondente.

## 2.4 processamento.c

* ### Este relatório descreve funções para calcular o Local Binary Pattern (LBP) de uma imagem, gerar e salvar histogramas LBP, além de manipular arquivos de imagem no formato PGM.

### Função calcular_lbp

* ### A função calcular_lbp determina o valor LBP de um pixel específico na imagem. O processo envolve:

#### 1. Deslocamentos dos Vizinhos: Define um array de deslocamentos para os 8 vizinhos do pixel central.
#### 2.    Comparação com o Pixel Central: Para cada vizinho, verifica se ele está dentro dos limites da imagem. Se o valor do vizinho for maior ou igual ao do pixel central, acumula um valor que corresponde a uma potência de 2.
#### 3.    Retorno do Valor LBP: A função retorna o valor LBP calculado.

### Função calcular_histograma

#### A função calcular_histograma gera o histograma LBP para toda a imagem:

#### 1. Iteração pelos Pixels: Percorre cada pixel, calculando seu valor LBP usando a função calcular_lbp.
#### 2.    Acumulação dos Valores: Incrementa a contagem correspondente no histograma para cada valor LBP encontrado.
#### 3.    Normalização: Divide cada entrada do histograma pelo total de pixels, normalizando os valores.

### Função distancia_euclidiana

* ### A função distancia_euclidiana calcula a distância Euclidiana entre dois histogramas LBP:

#### 1. Soma dos Quadrados: Percorre ambos os histogramas, calculando a soma dos quadrados das diferenças entre os valores correspondentes.
#### 2. Raiz Quadrada: Retorna a raiz quadrada da soma total, representando a distância entre os histogramas.

### Função processar_imagem

* ### A função processar_imagem carrega uma imagem, calcula e salva seu histograma LBP:

#### 1. Tentativa de Carregamento do Histograma: Primeiro, tenta carregar o histograma LBP de um arquivo.
#### 2.    Leitura do Arquivo de Imagem: Se o histograma não estiver disponível, lê o cabeçalho da imagem e os dados dos pixels.
#### 3.    Cálculo do Histograma: Chama calcular_histograma para gerar o histograma a partir dos dados da imagem.
#### 4.    Salvamento do Histograma: Salva o histograma em um arquivo para uso futuro.

### Função gerar_imagem_lbp

* ### A função gerar_imagem_lbp cria uma nova imagem LBP a partir de uma imagem de entrada:

#### 1. Leitura do Arquivo de Imagem: Lê a imagem de entrada e seus dados.
#### 2. Cálculo do LBP: Para cada pixel, calcula seu valor LBP e armazena na nova imagem.
#### 3. Salvamento da Imagem LBP: Salva a imagem LBP gerada no formato P5 ou P2, dependendo do formato da imagem de entrada.

### Funções salvar_imagem_p5 e salvar_imagem_p2

* ### Essas funções salvam os dados da imagem LBP em arquivos no formato P5 e P2, respectivamente. Elas escrevem o cabeçalho correto e os dados da imagem nos arquivos de saída.

